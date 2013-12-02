#include "Graph.h"
#include "GraphUtil.h"
#include "BinaryTree.h"
#include "GenericTree.h"
#include "BitOperators.h"
#include "FlatVector.h"
#include <string>
#include <set>
#include <queue>
#include <map>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>

using namespace std;

typedef pair<int, int> VertexDegreePair;//first is vertex ID; second is degree.


struct VertexDegreePairComp{
	bool operator() (const VertexDegreePair& p1, const VertexDegreePair&p2) const{
		if(p1.second<p2.second)
			return true;
		else
		{
			if(p1.second>p2.second)
				return false;
			else
			{
				if(p1.first<p2.first)
					return true;
				else 
					return false;
			}
		}
	}
};


struct vertex_mapping_info{
	vertex_mapping_info():level(-1), NodeBitLabel(0), offset(-1){};
	int level;//level of the mapped node
	MACHINE_WORD NodeBitLabel; //offset in the mapped node
	int offset;//offset in active list of the mapped node
};

inline
bool VertexDistanceCompare(const VertexDistancePair& i, const VertexDistancePair& j){
	return i.first<j.first;
}


bool buildTreeWidthDecomposition(Graph& g, GenericTree& twd, vector<set<int> >& nodedirectory, int& max_bag_size, const int& BAG_SIZE_LIMIT){//return the max_bag_size of this decomposition
	set<VertexDegreePair,VertexDegreePairComp> DegreeRank;//first is vertex ID; second is degree.
	vector<int> DegreeList(g.num_vertices(),0);
	max_bag_size=-1;//initialize this value to be nonvalid. 
	for(int i=0; i<g.num_vertices(); i++){
		int current_degree=g.get_degree(i);
		DegreeRank.insert(VertexDegreePair(i, current_degree));
		DegreeList[i]=current_degree;
	}
	
	while(DegreeRank.size()>0){
		set<VertexDegreePair,VertexDegreePairComp>::iterator minDegreeVertex=DegreeRank.begin();
		DegreeRank.erase(DegreeRank.begin());
		int current_vertexID=minDegreeVertex->first;
		//build treenode
		nodedirectory.push_back(set<int>());
		int current_nodeID=nodedirectory.size()-1;
		nodedirectory.back().insert(current_vertexID);

		for(EdgeList::iterator vit=g.get_neighbors(current_vertexID).begin(); vit<g.get_neighbors(current_vertexID).end(); vit++){
			nodedirectory.back().insert(*vit);
			g[*vit].nodes.push_back(current_nodeID);

			
			//update on graph
			EdgeList::iterator iit=g.get_neighbors(current_vertexID).begin();
			EdgeList::iterator jit=g.get_neighbors(*vit).begin();
			EdgeList combined;
			while(iit<g.get_neighbors(current_vertexID).end() && jit<g.get_neighbors(*vit).end()){
				
				//It is assumed that a vertex itself does not appear in its adjacent list and the adjacent list is in ascending order.
				if(*iit==*vit){
					iit++;
					continue;
				}else if (*jit==current_vertexID){
					jit++;
					continue;
				}
				
				if(*iit<*jit){
					combined.push_back(*iit);
					iit++;
				}else if(*iit>*jit){
					combined.push_back(*jit);
					jit++;
				}else{//implies *iit==*jit
					combined.push_back(*iit);
					iit++;
					jit++;
				}
			}
			
			if(iit<g.get_neighbors(current_vertexID).end() && jit<g.get_neighbors(*vit).end()){
				cout<<"Internal Logic Error. Exit."<<endl;
				exit(-1);
			}
			
			for(; iit<g.get_neighbors(current_vertexID).end(); iit++){
				if(*iit!=*vit)
					combined.push_back(*iit);
			}
			
			for(; jit<g.get_neighbors(*vit).end(); jit++){
				if(*jit!=current_vertexID)
					combined.push_back(*jit);
			}
			
			g.get_neighbors(*vit)=combined;
			//update on graph done
			
			//update on nodes
			vector<int> reduced;
			vector<int>::iterator itern=g[*vit].nodes.begin();
			vector<int>::iterator jtern=g[current_vertexID].nodes.begin();
			while(itern<g[*vit].nodes.end() && jtern<g[current_vertexID].nodes.end()){
				if(*itern<*jtern){
					reduced.push_back(*itern);
					itern++;
				}
				else if (*itern>*jtern){
					jtern++;
				}
				else{//implies *itern==*jtern
					itern++;
					jtern++;
				}
			}
			
			for(; itern<g[*vit].nodes.end(); itern++)
				reduced.push_back(*itern);
			
			g[*vit].nodes=reduced;
			//update on nodes done
			
			//other update
			set<VertexDegreePair,VertexDegreePairComp>::iterator fit=DegreeRank.find(VertexDegreePair(*vit, DegreeList[*vit]));
			DegreeRank.erase(fit);
			DegreeRank.insert(VertexDegreePair(*vit, g.get_degree(*vit)));
			DegreeList[*vit]=g.get_degree(*vit);
			//other update done
		}
		
		max_bag_size=int(nodedirectory.back().size())>max_bag_size? int(nodedirectory.back().size()) : max_bag_size;
		if(max_bag_size>BAG_SIZE_LIMIT)
			return false;		
		//build treenode done
		
		
		//build decomposition tree
		twd.addNode(twd.num_nodes());
		for(vector<int>::iterator nit=g[current_vertexID].nodes.begin(); nit<g[current_vertexID].nodes.end(); nit++){
			twd.addEdge(current_nodeID, *nit);//current_nodeID shall always be larger than *nit;
			twd.addEdge(*nit, current_nodeID);
		}
		//build decomposition tree done
		
		
		//clear deleted vertex
		g[current_vertexID].nodes.clear();
		g.get_neighbors(current_vertexID).clear();
		//clear deleted vertex
	}
	return true;
	
}

int SubtreeMeasure(GenericTree& gt, int& centroid, int& centroid_weight, const int& current_vertex, const int& from_vertex, const int& total_size, const vector<set<int> >& nodedirectory){
	
	TreeEdgeList::iterator iter_from;
	int branch_total_size_other_than_from=0;
	int max_branch=0;
	for(TreeEdgeList::iterator tit=gt.get_neighbors(current_vertex).begin(); tit!=gt.get_neighbors(current_vertex).end(); tit++){
		if(tit->nodeID!=from_vertex){
			tit->subtree_size=SubtreeMeasure(gt, centroid, centroid_weight, tit->nodeID, current_vertex, total_size, nodedirectory);
			max_branch=(max_branch>tit->subtree_size)? max_branch:tit->subtree_size; 
			branch_total_size_other_than_from+=tit->subtree_size;
		}
		else
			iter_from=tit;
	}
	if (from_vertex>=0){
		iter_from->subtree_size=total_size-branch_total_size_other_than_from-1;
		max_branch=(max_branch>iter_from->subtree_size)? max_branch:iter_from->subtree_size;
	}
	
	
	if(max_branch<=total_size/2){//use the centroid with smallest bag for tie break;
		if(int(nodedirectory[current_vertex].size())<centroid_weight){
			centroid=current_vertex;
			centroid_weight=nodedirectory[current_vertex].size();
		}
	}
	
	return branch_total_size_other_than_from+1;
}

void SubtreeUpdateMeasure(GenericTree& twd, int& centroid, int& centroid_weight, const int& current_vertex, const int& from_vertex, const int& total_size, int level, const vector<set<int> >& nodedirectory){
	
	TreeEdgeList::iterator iter_from;
	int branch_total_size_other_than_from=0;
	int max_branch=0;
	for(TreeEdgeList::iterator tit=twd.get_neighbors(current_vertex).begin(); tit!=twd.get_neighbors(current_vertex).end(); tit++){
		if(tit->nodeID!=from_vertex && twd[tit->nodeID].active==true){
			SubtreeUpdateMeasure(twd, centroid, centroid_weight, tit->nodeID, current_vertex, total_size, level, nodedirectory);
			max_branch=(max_branch>tit->subtree_size)? max_branch:tit->subtree_size; 
			branch_total_size_other_than_from+=tit->subtree_size;

		}
		else if (tit->nodeID==from_vertex)//This condition test cannot be missed, otherwise results in an logic error. 
			iter_from=tit;
	}
	if (from_vertex>=0){
		iter_from->subtree_size=total_size-branch_total_size_other_than_from-1;
		max_branch=(max_branch>iter_from->subtree_size)? max_branch:iter_from->subtree_size;
	}
	
	if(max_branch<=total_size/2){//use the centroid with smallest bag for tie break;
		if(int(nodedirectory[current_vertex].size())<centroid_weight){
			centroid=current_vertex;
			centroid_weight=nodedirectory[current_vertex].size();
		}
	}
}

void BalancedTreeConstr(GenericTree& twd, GenericTree& btd, const int& centroid, int level, const vector<set<int> >& nodedirectory){
	twd[centroid].active=false;
	for(TreeEdgeList::iterator tit=twd.get_neighbors(centroid).begin(); tit!=twd.get_neighbors(centroid).end(); tit++){
		if(twd[tit->nodeID].active==true){
			int subcentroid=-1;
			int subcentroid_weight=MAX_VAL;
			SubtreeUpdateMeasure(twd, subcentroid, subcentroid_weight, tit->nodeID, centroid, tit->subtree_size, level, nodedirectory);
			neighbor btd_adj_node;
			neighbor btd_parent;
			btd_adj_node.nodeID=subcentroid;
			btd_parent.nodeID=centroid;
			btd.get_neighbors(centroid).push_back(btd_adj_node);
			btd.get_neighbors(btd_adj_node.nodeID).push_back(btd_parent);
			btd[centroid].nodelevel=level;
			btd[subcentroid].nodelevel=level+1;
			
			BalancedTreeConstr(twd, btd, subcentroid, level+1, nodedirectory);
		}
	}
}

void buildBalancedTreeDecomposition(GenericTree& twd, GenericTree& btd, const vector<set<int> >& nodedirectory){
	for(int i=0; i<twd.num_nodes(); i++){
		if(twd[i].visited==true)
			continue;
		
		//calculate the number of vertices in the connected component (BFS)
		twd[i].visited=true;
		int CC_size=1;
		queue<int> q;
		q.push(i);
		while(q.size()>0){
			int j=q.front();
			q.pop();
			for(TreeEdgeList::iterator tit=twd.get_neighbors(j).begin(); tit!=twd.get_neighbors(j).end(); tit++){
				if(twd[tit->nodeID].visited==true)
					continue;
				
				twd[tit->nodeID].visited=true;
				CC_size++;
				q.push(tit->nodeID);
			}
		}
		//calculate the number of vertices in the connected component done
		
		//Fill in the neighbor size info and locate the centroid
		int from_vertex=-1;
		int centroid=-1;
		int centroid_weight=MAX_VAL;
		SubtreeMeasure(twd, centroid, centroid_weight, i, from_vertex, CC_size, nodedirectory);
		//Fill in the neighbor size info and locate the centroid done
		
		//Iteratively build balanced decomposition tree
		BalancedTreeConstr(twd, btd, centroid, 0, nodedirectory);
		SubtreeMeasure(btd, centroid, centroid_weight, i, from_vertex, CC_size, nodedirectory);
		//Iteratively build balanced decomposition tree done
	}
}

void buildBinaryTree(GenericTree& btd, BinaryTree& btree, vector<int>& virtualnode_to_realnode){
	multimap<int, int> subsize_root;////The first int is subtree size, and the second is node ID.
	for(int current_node=0; current_node<btd.num_nodes(); current_node++){
		
		int current_node_level=btd[current_node].nodelevel;
		multimap<int, int> subsize_node;//The first int is subtree size, and the second is node ID.
		int parent=-1;
		int total_size=0;
		for(TreeEdgeList::iterator tit=btd.get_neighbors(current_node).begin(); tit!=btd.get_neighbors(current_node).end(); tit++){
			if(btd[tit->nodeID].nodelevel>current_node_level){
				subsize_node.insert(pair<int, int>(tit->subtree_size, tit->nodeID));
				total_size+=tit->subtree_size;
			}else if (btd[tit->nodeID].nodelevel==current_node_level){
				cout<<"Internal logic error 1 ocurrs on binary tree construction."<<endl;
			}else{//implies btd[tit->nodeID].nodelevel<current_node_level
				if (parent==-1){
					parent=tit->nodeID;
				}else{
					cout<<"Internal logic error 2 (multiple parents) ocurrs on binary tree construction."<<endl;
				}
			}
		}
		
		if (parent==-1){//implies root node
			subsize_root.insert(pair<int, int>(total_size+1, current_node));
		}
		
		btree[current_node].parent=parent;
		if(subsize_node.size()==1){
			btree[current_node].left_child=(subsize_node.begin())->second;
		}else if (subsize_node.size()==2){
			multimap<int, int>::iterator mit=subsize_node.begin();
			btree[current_node].left_child=mit->second;
			mit++;
			btree[current_node].right_child=mit->second;
		}else if (subsize_node.size()>2){ 
			while(subsize_node.size()>2){
				int first_size=(subsize_node.begin())->first;
				int first_node=(subsize_node.begin())->second;
				subsize_node.erase(subsize_node.begin());
				int second_size=(subsize_node.begin())->first;
				int second_node=(subsize_node.begin())->second;
				subsize_node.erase(subsize_node.begin());
				int new_node_id=btree.num_nodes();
				//add a virtual node
				btree.addNode(new_node_id);
				virtualnode_to_realnode.push_back(current_node);
				//add a virtual node done
				btree[new_node_id].left_child=first_node;
				btree[new_node_id].right_child=second_node;
				btree[first_node].parent=new_node_id;
				btree[second_node].parent=new_node_id;
				subsize_node.insert(pair<int, int>(first_size+second_size, new_node_id));
			}
			multimap<int, int>::iterator mit=subsize_node.begin();			
			btree[current_node].left_child=mit->second;
			btree[mit->second].parent=current_node;
			
			mit++;
			btree[current_node].right_child=mit->second;
			btree[mit->second].parent=current_node;
		}
		
	}

	int root=-1;
	if(subsize_root.size()==1){
		root=(subsize_root.begin())->second;
	}else if (subsize_root.size()>1){
		while(subsize_root.size()>1){
			int first_size=(subsize_root.begin())->first;
			int first_node=(subsize_root.begin())->second;
			subsize_root.erase(subsize_root.begin());
			int second_size=(subsize_root.begin())->first;
			int second_node=(subsize_root.begin())->second;
			subsize_root.erase(subsize_root.begin());
			int new_node_id=btree.num_nodes();
			//add a virtual node
			btree.addNode(new_node_id);
			virtualnode_to_realnode.push_back(-1);//If the root node is a virtual node, there is no real node can be associated with it.
			//add a virtual node done
			btree[new_node_id].left_child=first_node;
			btree[new_node_id].right_child=second_node;
			btree[first_node].parent=new_node_id;
			btree[second_node].parent=new_node_id;
			subsize_root.insert(pair<int, int>(first_size+second_size, new_node_id));
			root=new_node_id;
		}
	}
	
	if(root<0){
		cout<<"The btree is empty. Please check input."<<endl;
		return;
	}else{
		btree.access_root()=root;
		btree[root].level=0;
		int maximum_level=0;
		queue<int> q;
		q.push(root);
		while(q.size()>0){
			int j=q.front();
			q.pop();
			maximum_level=maximum_level>btree[j].level?maximum_level:btree[j].level;

			if (btree[j].left_child>=0){
				btree[btree[j].left_child].level=btree[j].level+1;
				q.push(btree[j].left_child);
			}
			
			if (btree[j].right_child>=0){
				btree[btree[j].right_child].level=btree[j].level+1;
				q.push(btree[j].right_child);
			}
			
		}
		btree.access_height()=maximum_level;
	}
}

void BFSLabeling(Graph& g, const int& starting_vertex, vector<vector<VertexDistancePair> >& LabelGT){
	vector<bool> visited_list(g.num_vertices(), false);
	queue<VertexDistancePair> q;//First is vertex ID, second is distance;
	LabelGT[starting_vertex].push_back(pair<int, int>(starting_vertex, 0));	
	q.push(pair<int, int>(starting_vertex,0));
	while(q.size()>0){
		pair<int, int> vertex_distance=q.front();
		q.pop();
		for(EdgeList::iterator it=g.get_neighbors(vertex_distance.first).begin(); it!=g.get_neighbors(vertex_distance.first).end(); it++){
			if(visited_list[*it]==true || g[*it].active==false)
				continue;
				
			visited_list[*it]=true;
			LabelGT[*it].push_back(pair<int, int>(starting_vertex, vertex_distance.second+1));
			q.push(pair<int, int>(*it, vertex_distance.second+1));
		}
	}

}

void BFSLabelingDirect(Graph& g, const int& starting_vertex, vector<vector<VertexDistancePair> >& LabelGT_direct, const int& direct_type){
	vector<bool> visited_list(g.num_vertices(), false);
	queue<VertexDistancePair> q;//First is vertex ID, second is distance;
	LabelGT_direct[starting_vertex].push_back(pair<int, int>(starting_vertex, 0));	
	q.push(pair<int, int>(starting_vertex,0));
	while(q.size()>0){
		pair<int, int> vertex_distance=q.front();
		q.pop();
		EdgeList::iterator START, END;
		if (direct_type==0){
			START=g.out_edges(vertex_distance.first).begin();//This is very trick. Remind: This is a decentralize vertex, not the from or to vertex.
			END=g.out_edges(vertex_distance.first).end();
		}
		else if (direct_type==1){
			START=g.in_edges(vertex_distance.first).begin();
			END=g.in_edges(vertex_distance.first).end();		
		}else{
			cout<<"Parameter using Error in BFSLabelingDirect."<<endl;
			exit(-1);
		}
		for(EdgeList::iterator it=START; it!=END; it++){
			if(visited_list[*it]==true || g[*it].active==false)
				continue;
				
			visited_list[*it]=true;
			LabelGT_direct[*it].push_back(pair<int, int>(starting_vertex, vertex_distance.second+1));
			q.push(pair<int, int>(*it, vertex_distance.second+1));
		}
	}

}

void BuildLabelGTDirect(Graph& g, BinaryTree& btree, const vector<set<int> >& nodedirectory, vector<vector<int> >& node_mapping_directory, vector<vector<VertexDistancePair> >& LabelGT_From, vector<vector<VertexDistancePair> >& LabelGT_To, vector<vertex_mapping_info>& vertex_mapping_table, vector<int>& vertex_to_nodeID){
	
	//calculate the number of vertices in the connected component (BFS)
	queue<int> q;
	if(btree.access_root()<0){
		cout<<"Btree has not been fully constructed. Exit."<<endl;
		exit(-1);
	}
	q.push(btree.access_root());
	while(q.size()>0){
		size_t j=q.front();
		q.pop();
		if (j<nodedirectory.size()){
			int i=0;
			for(set<int>::const_iterator sit=nodedirectory[j].begin(); sit!=nodedirectory[j].end(); sit++){
				if(g[*sit].active==true){
					vertex_to_nodeID[*sit]=j;
					vertex_mapping_table[*sit].level=btree[j].level;
					vertex_mapping_table[*sit].offset=i;
					node_mapping_directory[j].push_back(*sit);
					g[*sit].active=false;
					BFSLabelingDirect(g, *sit, LabelGT_From, 0);//0 means from and in edges;
					BFSLabelingDirect(g, *sit, LabelGT_To, 1);//1 means to and out edges;
					i++;
				}
			}
		}
		
		if (btree[j].left_child>=0){
			q.push(btree[j].left_child);
		}
		
		if (btree[j].right_child>=0){
			q.push(btree[j].right_child);
		}
	}
	//calculate the number of vertices in the connected component done
}

void BuildLabelGT(Graph& g, BinaryTree& btree, const vector<set<int> >& nodedirectory, vector<vector<int> >& node_mapping_directory, vector<vector<VertexDistancePair> >& LabelGT, vector<vertex_mapping_info>& vertex_mapping_table, vector<int>& vertex_to_nodeID){
	
	//calculate the number of vertices in the connected component (BFS)
	queue<int> q;
	if(btree.access_root()<0){
		cout<<"Btree has not been fully constructed. Exit."<<endl;
		exit(-1);
	}
	q.push(btree.access_root());
	while(q.size()>0){
		size_t j=q.front();
		q.pop();
		if (j<nodedirectory.size()){
			int i=0;
			for(set<int>::const_iterator sit=nodedirectory[j].begin(); sit!=nodedirectory[j].end(); sit++){
				if(g[*sit].active==true){
					vertex_to_nodeID[*sit]=j;
					vertex_mapping_table[*sit].level=btree[j].level;
					vertex_mapping_table[*sit].offset=i;
					node_mapping_directory[j].push_back(*sit);
					g[*sit].active=false;
					BFSLabeling(g, *sit, LabelGT);
					i++;
				}
			}
		}
		
		if (btree[j].left_child>=0){
			q.push(btree[j].left_child);
		}
		
		if (btree[j].right_child>=0){
			q.push(btree[j].right_child);
		}
	}
	//calculate the number of vertices in the connected component done
}

void BuildTreeNodeBitlabel(BinaryTree& btree, int& current_node, vector<MACHINE_WORD>& TreeNodeBitLabel){
	if (btree[current_node].left_child>=0){
		TreeNodeBitLabel[btree[current_node].left_child]=TreeNodeBitLabel[current_node];
		BitOperators::set_zero(TreeNodeBitLabel[btree[current_node].left_child], btree.access_height()-btree[btree[current_node].left_child].level+1);
		BitOperators::set_one(TreeNodeBitLabel[btree[current_node].left_child], btree.access_height()-btree[btree[current_node].left_child].level);
		BuildTreeNodeBitlabel(btree, btree[current_node].left_child, TreeNodeBitLabel);
	}
	
	if(btree[current_node].right_child>=0){
		TreeNodeBitLabel[btree[current_node].right_child]=TreeNodeBitLabel[current_node];
		BitOperators::set_one(TreeNodeBitLabel[btree[current_node].right_child], btree.access_height()-btree[btree[current_node].right_child].level);
		BuildTreeNodeBitlabel(btree, btree[current_node].right_child, TreeNodeBitLabel);
	}
}

int prequery(const vector<vector<VertexDistancePair> >& LabelGT, const int& source_vertex, const int& destination_vertex){
	vector<VertexDistancePair>::const_iterator it_source=LabelGT[source_vertex].begin();
	vector<VertexDistancePair>::const_iterator it_destination=LabelGT[destination_vertex].begin();
	int distance=MAX_VAL;
	while(it_source<LabelGT[source_vertex].end() && it_destination<LabelGT[destination_vertex].end()){
		VertexDistancePair vd_source=*it_source;
		VertexDistancePair vd_destination=*it_destination;
		if(vd_source.first==vd_destination.first){
			distance=vd_source.second+vd_destination.second<distance ? vd_source.second+vd_destination.second : distance;
			it_source++;
			it_destination++;
		}
		else if (vd_source.first<vd_destination.first)
			it_source++;
		else
			it_destination++;
	}
	return distance;
}

int prequeryDirect(const vector<vector<VertexDistancePair> >& LabelGT_From, const vector<vector<VertexDistancePair> >& LabelGT_To, const int& source_vertex, const int& destination_vertex){
	vector<VertexDistancePair>::const_iterator it_source=LabelGT_To[source_vertex].begin();
	vector<VertexDistancePair>::const_iterator it_destination=LabelGT_From[destination_vertex].begin();
	int distance=MAX_VAL;
	while(it_source<LabelGT_To[source_vertex].end() && it_destination<LabelGT_From[destination_vertex].end()){
		VertexDistancePair vd_source=*it_source;
		VertexDistancePair vd_destination=*it_destination;	
		if(vd_source.first==vd_destination.first){
			distance=vd_source.second+vd_destination.second<distance ? vd_source.second+vd_destination.second : distance;
			it_source++;
			it_destination++;
		}
		else if (vd_source.first<vd_destination.first)
			it_source++;
		else
			it_destination++;
	}
	return distance;
}


inline
int query(vector<vertex_mapping_info>& vertex_mapping_table, hash_map<int, int>& BitLabelTreeNode, vector<vector<int> >& node_directory, vector<vector<vector<int> > >& LabelGTUpdate, int& tree_height, const int& source_vertex, const int& destination_vertex){
	
	
	int min_level=-1;
	int LCA_nodeID=-1;
	vertex_mapping_info srcinfo=vertex_mapping_table[source_vertex];
	vertex_mapping_info dstinfo=vertex_mapping_table[destination_vertex];	
	if(srcinfo.NodeBitLabel==dstinfo.NodeBitLabel){
		min_level=srcinfo.level;//determined
		LCA_nodeID=BitLabelTreeNode[srcinfo.NodeBitLabel];
	}

	if(srcinfo.level<dstinfo.level){
		min_level=srcinfo.level;//indetermined
		LCA_nodeID=BitLabelTreeNode[srcinfo.NodeBitLabel];
	}else if (srcinfo.level>dstinfo.level){
		min_level=dstinfo.level;//indetermined
		LCA_nodeID=BitLabelTreeNode[dstinfo.NodeBitLabel];
	}else{
		min_level=srcinfo.level;
		//If LCA_nodeID has not been assigned a value, then it shall not be assigned a value here.
	}
	
	if(srcinfo.NodeBitLabel>>tree_height+1-min_level != dstinfo.NodeBitLabel>>tree_height+1-min_level ){//otherwise, LCA_nodeID contains the one of the query vertex, a case handled above.
		int HLCA=int(log(srcinfo.NodeBitLabel ^ dstinfo.NodeBitLabel)/log(2));
		int LCA=((srcinfo.NodeBitLabel | dstinfo.NodeBitLabel)>>HLCA)<<HLCA;
		LCA_nodeID=BitLabelTreeNode[LCA];
		if(LCA_nodeID==-1){//The two vertices do not have a real LCA node.
			return MAX_VAL;
		}
	}
	
	int distance=MAX_VAL;
	size_t nodewidth=node_directory[LCA_nodeID].size();
	for(size_t i=0; i<nodewidth; i++){
		int mapping_level=vertex_mapping_table[node_directory[LCA_nodeID][i]].level;
		int mapping_offset=vertex_mapping_table[node_directory[LCA_nodeID][i]].offset;
		int tmp_distance=LabelGTUpdate[source_vertex][mapping_level][mapping_offset]+LabelGTUpdate[destination_vertex][mapping_level][mapping_offset];
		distance=tmp_distance<distance?tmp_distance:distance;
	}
	return distance;
}

inline
int queryDirectMask(vector<vertex_mapping_info>& vertex_mapping_table, hash_map<int, int>& BitLabelTreeNode, vector<int>& TreeNodeLevel, vector<vector<int> >& node_directory, vector<vector<vector<int> > >& LabelGT_From_Update, vector<vector<vector<int> > >& LabelGT_To_Update, vector<vector<FlatVector> >& LabelGT_From_Mask, vector<vector<FlatVector> >& LabelGT_To_Mask, int& tree_height, const int& source_vertex, const int& destination_vertex){
	
	int min_level=-1;
	int LCA_nodeID=-1;
	vertex_mapping_info srcinfo=vertex_mapping_table[source_vertex];
	vertex_mapping_info dstinfo=vertex_mapping_table[destination_vertex];	
	if(srcinfo.NodeBitLabel==dstinfo.NodeBitLabel){
		min_level=srcinfo.level;//determined
		LCA_nodeID=BitLabelTreeNode[srcinfo.NodeBitLabel];
	}

	if(srcinfo.level<dstinfo.level){
		min_level=srcinfo.level;//indetermined
		LCA_nodeID=BitLabelTreeNode[srcinfo.NodeBitLabel];
	}else if (srcinfo.level>dstinfo.level){
		min_level=dstinfo.level;//indetermined
		LCA_nodeID=BitLabelTreeNode[dstinfo.NodeBitLabel];
	}else{
		min_level=srcinfo.level;
		//If LCA_nodeID has not been assigned a value, then it shall not be assigned a value here.
	}
	
	if(srcinfo.NodeBitLabel>>tree_height+1-min_level != dstinfo.NodeBitLabel>>tree_height+1-min_level ){//otherwise, LCA_nodeID contains the one of the query vertex, a case handled above.
		int HLCA=int(log(srcinfo.NodeBitLabel ^ dstinfo.NodeBitLabel)/log(2));
		int LCA=((srcinfo.NodeBitLabel | dstinfo.NodeBitLabel)>>HLCA)<<HLCA;
		LCA_nodeID=BitLabelTreeNode[LCA];
		if(LCA_nodeID==-1){
			return MAX_VAL;
		}
		min_level=TreeNodeLevel[LCA_nodeID];
	}
	
	
	if(!LabelGT_To_Mask[source_vertex][min_level].LOGICAND(LabelGT_From_Mask[destination_vertex][min_level]))
		return MAX_VAL;
		
	
	int distance=MAX_VAL;
	size_t nodewidth=node_directory[LCA_nodeID].size();
	for(size_t i=0; i<nodewidth; i++){
		int mapping_level=vertex_mapping_table[node_directory[LCA_nodeID][i]].level;
		int mapping_offset=vertex_mapping_table[node_directory[LCA_nodeID][i]].offset;
		int tmp_distance=LabelGT_To_Update[source_vertex][mapping_level][mapping_offset]+LabelGT_From_Update[destination_vertex][mapping_level][mapping_offset];
		distance=tmp_distance<distance?tmp_distance:distance;
	}
	return distance;
}


inline
int queryDirectDLS(vector<vector<VertexDistancePair> >& LabelDLS_From, vector<vector<VertexDistancePair> >& LabelDLS_To, const int& source_vertex, const int& destination_vertex){
	vector<VertexDistancePair>::iterator iter_To=LabelDLS_To[source_vertex].begin();
	vector<VertexDistancePair>::iterator iter_From=LabelDLS_From[destination_vertex].begin();

	int distance=MAX_VAL;
	while(true)
	{		            
		if (iter_To==LabelDLS_To[source_vertex].end() || iter_From==LabelDLS_From[destination_vertex].end())
			break;
		
		if(iter_To->first<iter_From->first)
			iter_To++;
		else if(iter_To->first>iter_From->first)
			iter_From++;
		else
		{
			int temp_distance=iter_To->second+iter_From->second;
			if(temp_distance<distance)
				distance=temp_distance;
			
			iter_To++;
			iter_From++;
		}	
	}
	
	return distance;
}

inline
int queryDLS(vector<vector<VertexDistancePair> >& LabelDLS, const int& source_vertex, const int& destination_vertex){
	vector<VertexDistancePair>::iterator iter_To=LabelDLS[source_vertex].begin();
	vector<VertexDistancePair>::iterator iter_From=LabelDLS[destination_vertex].begin();

	int distance=MAX_VAL;
	while(true)
	{		            
		if (iter_To==LabelDLS[source_vertex].end() || iter_From==LabelDLS[destination_vertex].end())
			break;
		
		if(iter_To->first<iter_From->first)
			iter_To++;
		else if(iter_To->first>iter_From->first)
			iter_From++;
		else
		{
			int temp_distance=iter_To->second+iter_From->second;
			if(temp_distance<distance)
				distance=temp_distance;
			
			iter_To++;
			iter_From++;
		}	
	}
	
	return distance;
}

void loopdetect(GenericTree& twd){
	vector<bool> visited(twd.num_nodes(), false);
	for(int i=0; i<twd.num_nodes(); i++){
		if(visited[i]==true)
			continue;
		
		visited[i]=true;
		queue<pair<int, int> > q;
		q.push(pair<int, int>(-1,i));//first is father, second is current_node
		while(q.size()>0){
			pair<int, int> father_current=q.front();
			q.pop();
			for(TreeEdgeList::iterator it=twd.get_neighbors(father_current.second).begin(); it!=twd.get_neighbors(father_current.second).end(); it++){
				if(it->nodeID==father_current.first)
					continue;
				
				if(visited[it->nodeID]==true){
					cout<<"Loop Detected at: "<<it->nodeID<<endl;
					exit(-1);
				}
	
					
				visited[it->nodeID]=true;
				q.push(pair<int, int>(father_current.second, it->nodeID));
			}
		}
	}
}

static void usage() {
	cout << "\nUsage:\n"
		"	TreeMapC [-h] [-g graph_input_format] [-d direct_info] [-l rw_queryfile] [-q query_filename] [-n query_num] [-v verify_enable] [-b BAG_SIZE_LIMIT] [-c max_degree_reduction_cutoff] [-s output_LabelDLS] [-s1 output_LabelDLS_From] [-s2 output_LabelDLS_To] [-s3 output_graph] [-ld input_LabelDLS] [-ld1 input_LabelDLS_From] [-ld2 input_LabelDLS_To] [-ld3 input_graph] input_filename\n"
		"Description:\n"
		"	-h	Print the help message.\n"
		"	-g	graph_input_format. default value 0 (adjacent list); range graph_input_format={0,1,2,3}.\n"
		"	-d	direct_info. default value 0 (treat as undirected); range direct_info={0,1}.\n"
		"	-l	rw_queryfile. default value -1 (do nothing); range rw_queryfile={-1,0,1}.\n"
		"	-q	query_filename.\n"
		"	-n	query_num. default value 100,000.\n"
		"	-v	verify_enable. default value 1 (yes); range verify_enable={0,1}.\n"
		"	-b	BAG_SIZE_LIMIT. default value MAX_VAL; range BAG_SIZE_LIMIT=[1, MAX_VAL].\n"
		"	-c	max_degree_reduction_cutoff. default value 0.5. max_degree_reduction_cutoff=[0,1].\n"
		"	-s output_LabelDLS.\n"
		"	-s1 output_LabelDLS_From.\n"
		"	-s2 output_LabelDLS_To.\n"
		"	-s3 output_graph.\n"
		"	-ld input_LabelDLS.\n"
		"	-ld1 input_LabelDLS_From.\n"
		"	-ld2 input_LabelDLS_To.\n"
		"	-ld3 input_graph.\n"
		"	input_filename	The graph file.\n"
		<< endl;
}


int main(int argc, char* argv[]) {
	if (argc == 1) {
		usage();
		return 1;
	}
	string query_filename, input_filename, output_LabelDLS, output_LabelDLS_From, output_LabelDLS_To, output_graph, input_LabelDLS,  input_LabelDLS_From, input_LabelDLS_To, input_graph;
	int input_para_counter=1;
//	int source_vertex=-1;
//	int destination_vertex=-1;
	int rw_queryfile=-1;
	construction_type graph_input_format=ADJ_GRAPH;
	graph_type	direct_info=UNDIRECT;
	int query_num=100000;
	int BAG_SIZE_LIMIT=MAX_VAL;
	float max_degree_reduction_cutoff=0.5;
	int verify_enable=1;
	while (input_para_counter < argc) {
		if (strcmp("-h", argv[input_para_counter]) == 0) {
			usage();
			return 1;
		}
		
		if (strcmp("-g", argv[input_para_counter]) == 0) {
			input_para_counter++;
			graph_input_format=static_cast<construction_type>(atoi(argv[input_para_counter++]));			
		}else if (strcmp("-d", argv[input_para_counter]) == 0) {
			input_para_counter++;
			direct_info=static_cast<graph_type>(atoi(argv[input_para_counter++]));			
		}else if (strcmp("-l", argv[input_para_counter]) == 0){
			input_para_counter++;
			rw_queryfile= atoi(argv[input_para_counter++]);
		}else if (strcmp("-q", argv[input_para_counter]) == 0){
			input_para_counter++;
			query_filename= argv[input_para_counter++];
		}else if (strcmp("-n", argv[input_para_counter]) == 0){
			input_para_counter++;
			query_num= atoi(argv[input_para_counter++]);
		}else if (strcmp("-v", argv[input_para_counter]) == 0){
			input_para_counter++;
			verify_enable= atoi(argv[input_para_counter++]);
		}else if (strcmp("-b", argv[input_para_counter]) == 0){
			input_para_counter++;
			BAG_SIZE_LIMIT= atoi(argv[input_para_counter++]);
		}else if (strcmp("-c", argv[input_para_counter]) == 0){
			input_para_counter++;
			max_degree_reduction_cutoff= atof(argv[input_para_counter++]);
		}else if (strcmp("-s", argv[input_para_counter]) == 0){
			input_para_counter++;
			output_LabelDLS= argv[input_para_counter++];
		}else if (strcmp("-s1", argv[input_para_counter]) == 0){
			input_para_counter++;
			output_LabelDLS_From= argv[input_para_counter++];
		}else if (strcmp("-s2", argv[input_para_counter]) == 0){
			input_para_counter++;
			output_LabelDLS_To= argv[input_para_counter++];
		}else if (strcmp("-s3", argv[input_para_counter]) == 0){
			input_para_counter++;
			output_graph= argv[input_para_counter++];
		}else if (strcmp("-ld", argv[input_para_counter]) == 0){
			input_para_counter++;
			input_LabelDLS= argv[input_para_counter++];
		}else if (strcmp("-ld1", argv[input_para_counter]) == 0){
			input_para_counter++;
			input_LabelDLS_From= argv[input_para_counter++];
		}else if (strcmp("-ld2", argv[input_para_counter]) == 0){
			input_para_counter++;
			input_LabelDLS_To= argv[input_para_counter++];
		}else if (strcmp("-ld3", argv[input_para_counter]) == 0){
			input_para_counter++;
			input_graph= argv[input_para_counter++];
		}else{
			input_filename = argv[input_para_counter++];
//			source_vertex = atoi(argv[input_para_counter++]);
//			destination_vertex = atoi(argv[input_para_counter++]);
		}
	}
	
	
	ifstream infile(input_filename.c_str());
	if (!infile) {
		cout << "Error: Cannot open " << input_filename << endl;
		return -1;
	}
	
	vector<int> src;
	vector<int> trg;
	

	Graph g(infile, graph_input_format, direct_info);
	if(direct_info==UNDIRECT)
		cout << "#vertex size:" << g.num_vertices() << "\t#edge size:" << g.num_edges() << endl;
	else
		cout << "#vertex size:" << g.num_vertices() << "\t#undirect edge size:" << g.num_edges() << "\t#direct edge size:"<< g.num_directed_edges() << endl;
		

	//Symmetric test is enforced right now. In the future, we can allowed directed graphs
	//Note the program requires that a vertex itself does not appear in its adjacent list and the adjacent list is in ascending order.
	if(!g.isSymmetric()){
		cout<<"Internal Error: Intend to treat a directed graph as undirected."<<endl;
		exit(-1);
	}	
	
	if(rw_queryfile==1){
		GraphUtil::generate_queryfile(src, trg, g, query_filename.c_str(), query_num);
	}else if(rw_queryfile==0){
		GraphUtil::load_queryfile(src, trg, g, query_filename.c_str());
	}else{//generate query without loading from or saving into files.
		GraphUtil::generate_query(src, trg, g, query_num);
	}	

	
	//Start index construction
	struct timeval after_index_const_time, before_index_const_time;
	gettimeofday(&before_index_const_time, NULL);
	
	GenericTree twd;
	vector<set<int> > nodedirectory; //middle result.
	vector<vector<VertexDistancePair> > LabelDLS(g.num_vertices()); //DLS labels for handling large graphs
	vector<vector<VertexDistancePair> > LabelDLS_From(g.num_vertices()); //DLS labels for handling large graphs
	vector<vector<VertexDistancePair> > LabelDLS_To(g.num_vertices()); //DLS labels for handling large graphs

	Graph g_orig=g;
	Graph g1;
	bool Direct_DLS_Activated=false;
	bool DLS_Activated=false;	
	if (input_graph.length()>0){//loading DLS labels and the half processed graph
		construction_type graph_input_format1=ADJ_GRAPH;
		ifstream infile_graph(input_graph.c_str());
		g1=Graph(infile_graph, graph_input_format1, direct_info);
		if(direct_info==UNDIRECT){
			GraphUtil::loadDLS(LabelDLS, input_LabelDLS);
			DLS_Activated=true;
		}else{
			GraphUtil::loadDLSDirect(LabelDLS_From, LabelDLS_To, input_LabelDLS_From, input_LabelDLS_To);
			Direct_DLS_Activated=true;
		}
		BAG_SIZE_LIMIT=MAX_VAL;//opt out DLS labels because it has been done;
	}else{
		g1=g;
	}
	int max_bag_size=-1;
	
	set<VertexDegreePair,VertexDegreePairComp> DegreeRank;//first is vertex ID; second is degree.
	for(int i=0; i<g.num_vertices(); i++){
		int current_degree;
		if(direct_info==UNDIRECT) 
			current_degree=g.get_degree(i);
		else
			current_degree=g.get_directed_degree(i);
		DegreeRank.insert(VertexDegreePair(i, current_degree));
	}
	
	int number_of_decentralized_vertices=0;
	while(!buildTreeWidthDecomposition(g1, twd, nodedirectory, max_bag_size, BAG_SIZE_LIMIT)){
		int largest_degree=-1;

		while(true){
			int vertex_to_be_decentralized=-1;
			int updated_degree=-1;
			if(DegreeRank.size()>=2){
				while (true){
					set<VertexDegreePair,VertexDegreePairComp>::iterator DegreeRank_it1=DegreeRank.end();
					set<VertexDegreePair,VertexDegreePairComp>::iterator DegreeRank_it2=DegreeRank.end();
					DegreeRank_it1--;
					DegreeRank_it2--;
					DegreeRank_it2--;
					if(direct_info==UNDIRECT)
						updated_degree=g.get_degree(DegreeRank_it1->first);
					else
						updated_degree=g.get_directed_degree(DegreeRank_it1->first);
						
					if(updated_degree>=DegreeRank_it2->second){//degree will only decrease. Therefore this guarantees updated degree for DegreeRank_it1->first is no smaller than updated degree for DegreeRank_it2->first.
						vertex_to_be_decentralized=DegreeRank_it1->first;
						DegreeRank.erase(DegreeRank_it1);
						break;
					}
					else{
						int vertex_to_be_inserted=DegreeRank_it1->first;
						DegreeRank.erase(DegreeRank_it1);
						DegreeRank.insert(VertexDegreePair(vertex_to_be_inserted, updated_degree));
					}
				}
			}else if (DegreeRank.size()==1){
				vertex_to_be_decentralized=(DegreeRank.begin())->first;
				DegreeRank.clear();
			}else{//DegreeRank.size()==0
				break;
			}
			if(direct_info==UNDIRECT){
				GraphUtil::Decentralize(g, vertex_to_be_decentralized, LabelDLS);
				number_of_decentralized_vertices++;
				DLS_Activated=true;
			}
			else{
				GraphUtil::DecentralizeDirect(g, vertex_to_be_decentralized, LabelDLS_From, LabelDLS_To);
				number_of_decentralized_vertices++;
				Direct_DLS_Activated=true;
			}
			
			if(largest_degree==-1){
				largest_degree=updated_degree;
				cout<<"Largest degree of vertices being decentralized in this round: "<<largest_degree<<endl;
			}else{
				if(updated_degree<=largest_degree*max_degree_reduction_cutoff)
					break;
			}
		}
			
		//recover the parameters for new tree decomposition
		g1=g;
		twd.clear();
		nodedirectory.clear();
		max_bag_size=-1;
		cout<<"Start rebuilding tree decomposition."<<endl;
	}
	if(DLS_Activated || Direct_DLS_Activated)
		cout<<"Total number of decentralized vertices: "<<number_of_decentralized_vertices<<endl;
	
	if(output_LabelDLS.length()>0)
		GraphUtil::saveDLS(LabelDLS, output_LabelDLS);
	
	if(output_LabelDLS_From.length()>0){
		GraphUtil::saveDLSDirect(LabelDLS_From, LabelDLS_To, output_LabelDLS_From, output_LabelDLS_To);
	}
	
	if(output_graph.length()>0){
		ofstream outfile(output_graph.c_str());
		cout<<"save g with edge #"<<g.num_edges()<<endl;
		g.writeGraph(outfile, direct_info);
		outfile.close();
	}
	
	if(DLS_Activated){
	//Reorganize DLS labels from small to large
		for(size_t i=0; i<LabelDLS.size(); i++)
			sort(LabelDLS[i].begin(), LabelDLS[i].end(), VertexDistanceCompare);
	}
	
	if(Direct_DLS_Activated){
	//Reorganize Direct DLS labels from small to large
		for(size_t i=0; i<LabelDLS_From.size(); i++)
			sort(LabelDLS_From[i].begin(), LabelDLS_From[i].end(), VertexDistanceCompare);
			
		for(size_t i=0; i<LabelDLS_To.size(); i++)
			sort(LabelDLS_To[i].begin(), LabelDLS_To[i].end(), VertexDistanceCompare);
	}
	
	vector<vector<int> > node_mapping_directory(nodedirectory.size(), vector<int>());
	if((!DLS_Activated) && (!Direct_DLS_Activated))
		cout<<"max_bag_size of this decomposition: "<<max_bag_size<<endl;
	//twd.printTree();
	
	
	cout<<"Tree Width Decomposition successful. "<<endl;
	cout<<"twd.num_nodes(): "<<twd.num_nodes()<<endl;

	
	GenericTree btd(twd.num_nodes());
	buildBalancedTreeDecomposition(twd, btd, nodedirectory);
	cout<<"Balanced Tree decomposition successful. "<<endl;
	cout<<"btd.num_nodes(): "<<btd.num_nodes()<<endl;
	//btd.printTree();
	
	BinaryTree btree(btd.num_nodes());
	vector<int> virtualnode_to_realnode;//The virtual node to real node mapping table. //middle result.
	
	buildBinaryTree(btd, btree, virtualnode_to_realnode);
	cout<<"Binary Tree construction successful. "<<endl;
	cout<<"btree.num_nodes(): "<<btree.num_nodes()<<endl;
	cout<<"btree.access_height(): "<<btree.access_height()<<endl;	
	//btree.printTree();
	
	
	
	if(btree.access_height()>63){
		cout<<"The current program maximally supports a balanced binary tree with height no more than 63, i.e., about 2^61 vertices in the original graph."<<endl;
		cout<<"This number shall satisfy almost all current applications. Please verify you really need to run such a giant graph. If yes, a simple update on the Machine_WORD data structure will make it work."<<endl;
		cout<<"Program exit."<<endl;
		exit(0);
	}
	
	vector<MACHINE_WORD> TreeNodeBitLabel(btree.num_nodes(), 0);//middle result
	BitOperators::set_one(TreeNodeBitLabel[btree.access_root()], btree.access_height());
	cout<<"Start building Tree Node Bit label"<<endl;
	BuildTreeNodeBitlabel(btree, btree.access_root(), TreeNodeBitLabel);
	cout<<"End building Tree Node Bit label"<<endl;
	
	//hashmap implementation
	hash_map<int, int> BitLabelTreeNode;//Map bit label to real tree node. Part of final label.
	for (int i=0; i<int(TreeNodeBitLabel.size()); i++){
		if (i>=btd.num_nodes()){
			BitLabelTreeNode[TreeNodeBitLabel[i]]=virtualnode_to_realnode[i-btd.num_nodes()];//virtualnode_to_realnode gets -1 if no real node can be mapped. 
		}
		else
			BitLabelTreeNode[TreeNodeBitLabel[i]]=i;
	}
	
	vector<vector<VertexDistancePair> > LabelGT(g.num_vertices());//The first is vertex ID, and the second is distance. //middle result
	vector<vector<VertexDistancePair> > LabelGT_From(g.num_vertices());//middle result
	vector<vector<VertexDistancePair> > LabelGT_To(g.num_vertices());//middle result
	vector<vertex_mapping_info> vertex_mapping_table(g.num_vertices());//The table recording the mapping between vertex, and btree level and label offset. //Part of final label.
	vector<int> vertex_to_nodeID(g.num_vertices());//middle result	
	cout<<"Start building LabelGT"<<endl;
	if(direct_info==DIRECT)
		BuildLabelGTDirect(g, btree, nodedirectory, node_mapping_directory, LabelGT_From, LabelGT_To, vertex_mapping_table, vertex_to_nodeID);
	else
		BuildLabelGT(g, btree, nodedirectory, node_mapping_directory, LabelGT, vertex_mapping_table, vertex_to_nodeID);

	cout<<"End building LabelGT"<<endl;
	for(size_t i=0; i<vertex_mapping_table.size(); i++){
		vertex_mapping_table[i].NodeBitLabel=TreeNodeBitLabel[vertex_to_nodeID[i]];
	}
	
	//sort and printout LabelGT or LabelGT_From, LabelGT_To
	if(direct_info==DIRECT){
		for(size_t i=0; i<LabelGT_From.size(); i++){
			sort(LabelGT_From[i].begin(), LabelGT_From[i].end(), VertexDistanceCompare);
		}
		
		for(size_t i=0; i<LabelGT_To.size(); i++){
			sort(LabelGT_To[i].begin(), LabelGT_To[i].end(), VertexDistanceCompare);
		}			
	
	}else{
		for(size_t i=0; i<LabelGT.size(); i++){
			sort(LabelGT[i].begin(), LabelGT[i].end(), VertexDistanceCompare);
		}
	}
	//printout LabelGT done
	
	
	//release memory first time
	g1.clear();
	TreeNodeBitLabel.clear();
	virtualnode_to_realnode.clear();
	btd.clear();
	twd.clear();
	
	cout<<"Start prequery"<<endl;
	vector<vector<vector<int> > > LabelGTUpdate(g.num_vertices());//It is a partial update in this version
	vector<vector<vector<int> > > LabelGT_From_Update(g.num_vertices());
	vector<vector<FlatVector> >  LabelGT_From_Mask(g.num_vertices());
	vector<vector<vector<int> > > LabelGT_To_Update(g.num_vertices());
	vector<vector<FlatVector> >  LabelGT_To_Mask(g.num_vertices());

	for (int v=0; v<g.num_vertices(); v++){
		if(v%1000==0)
			cout<<"prequery on "<<v<<endl;
		if(direct_info==DIRECT){
			LabelGT_From_Update[v].resize(vertex_mapping_table[v].level+1);
			LabelGT_From_Mask[v].resize(vertex_mapping_table[v].level+1);
			LabelGT_To_Update[v].resize(vertex_mapping_table[v].level+1);
			LabelGT_To_Mask[v].resize(vertex_mapping_table[v].level+1);
		}else
			LabelGTUpdate[v].resize(vertex_mapping_table[v].level+1);
			
		int current_node=int(vertex_to_nodeID[v]);
		while(current_node!=-1){
			if(current_node<int(nodedirectory.size())){
				if(direct_info==DIRECT){
					set<int>::iterator	sit=nodedirectory[current_node].begin();
					vector<int>::iterator vit=node_mapping_directory[current_node].begin();
					int nodedirectory_offset=0;
					while(sit!=nodedirectory[current_node].end()){
						int To_distance=prequeryDirect(LabelGT_From, LabelGT_To, v, *sit);
						int From_distance=prequeryDirect(LabelGT_From, LabelGT_To, *sit, v);
						if(vit!=node_mapping_directory[current_node].end()){
							if(*sit==*vit){
								LabelGT_To_Update[v][btree[current_node].level].push_back(To_distance);
								if (To_distance<MAX_VAL)
									LabelGT_To_Mask[v][btree[current_node].level].insert(nodedirectory_offset);
									
								LabelGT_From_Update[v][btree[current_node].level].push_back(From_distance);
								if (From_distance<MAX_VAL)
									LabelGT_From_Mask[v][btree[current_node].level].insert(nodedirectory_offset);
							
								sit++;
								vit++;
							}else if (*sit<*vit){
								if (To_distance<MAX_VAL)
									LabelGT_To_Mask[v][btree[current_node].level].insert(nodedirectory_offset);
									
								if (From_distance<MAX_VAL)
									LabelGT_From_Mask[v][btree[current_node].level].insert(nodedirectory_offset);
							
								sit++;
							}else{
								cout<<"This should not happen because one is the subset of the other."<<endl;
								exit(-1);
							}
						}else{
							if (To_distance<MAX_VAL)
								LabelGT_To_Mask[v][btree[current_node].level].insert(nodedirectory_offset);
								
							if (From_distance<MAX_VAL)
								LabelGT_From_Mask[v][btree[current_node].level].insert(nodedirectory_offset);
							sit++;
						}
						nodedirectory_offset++;

					}
				
				}else{
					for(vector<int>::iterator vit=node_mapping_directory[current_node].begin(); vit!=node_mapping_directory[current_node].end(); vit++){
						LabelGTUpdate[v][btree[current_node].level].push_back(prequery(LabelGT, v, *vit));
						
					}
				}
			}
			current_node=btree[current_node].parent;
		}
	}
	cout<<"End prequery"<<endl;
	
	
	//release memory second time
	LabelGT.clear();
	LabelGT_From.clear();
	LabelGT_To.clear();
	vertex_to_nodeID.clear();
	node_mapping_directory.clear();
	
	int tree_height=btree.access_height();
	if(int(BitLabelTreeNode.size())<btree.num_nodes())
		cout<<"HashMap has compressed the index size"<<endl;
	else if (int(BitLabelTreeNode.size())>btree.num_nodes())
		cout<<"HashMap has introduced more index size than the tree node number."<<endl;
	btree.clear();
	
	vector<vector<int> > node_directory(nodedirectory.size());
	vector<int>	TreeNodeLevel(nodedirectory.size());//Only real node levels are necessary.
	int node_directory_size=0;
	for(size_t i=0; i<nodedirectory.size(); i++){
		TreeNodeLevel[i]=btree[i].level;
		for(set<int>::iterator sit=nodedirectory[i].begin(); sit!=nodedirectory[i].end(); sit++){
			node_directory[i].push_back(*sit);
			node_directory_size++;
		}
	}
	nodedirectory.clear();
	//release memory done
	//End index construction
	gettimeofday(&after_index_const_time, NULL);
	double index_construct_time=(after_index_const_time.tv_sec - before_index_const_time.tv_sec)*1000.0 + (after_index_const_time.tv_usec - before_index_const_time.tv_usec)*1.0/1000.0;
	cout<<"Index construction completed in : "<<index_construct_time<<" (ms). This time includes outputting the running information."<<endl;
	
	
	
	//Data structure must be used for ALL query: vertex_mapping_table, BitLabelTreeNode, tree_height;
	
	//Data structure must be used for distance query on undirected graphs:  LabelGTUpdate, node_directory;

	//Data structure must be used for distance query on directed graphs: LabelGT_From_Update, LabelGT_To_Update, node_directory;
	//Optional Data structure for distance query on directed graphs: LabelGT_From_Mask, LabelGT_To_Mask, TreeNodeLevel;
	
	//Calculate TC size:
	long int TC_size=0;
	long int DLS_size=0;//optional
	if(direct_info==DIRECT){
		int From_size_1=0;
		int From_size_2=0;
		for (size_t i=0; i<LabelGT_From_Update.size(); i++){
			for(size_t j=0; j<LabelGT_From_Update[i].size(); j++){
				From_size_1+=LabelGT_From_Update[i][j].size();
				From_size_2+=LabelGT_From_Mask[i][j].size();
			}
		}
		int To_size_1=0;
		int To_size_2=0;
		for (size_t i=0; i<LabelGT_To_Update.size(); i++){
			for(size_t j=0; j<LabelGT_To_Update[i].size(); j++){
				To_size_1+=LabelGT_To_Update[i][j].size();
				To_size_2+=LabelGT_To_Mask[i][j].size();
			}
		}
		if(From_size_1!=To_size_1){
			cout<<"From size and To size do not match. Error in direct labeling."<<endl;
			exit(-1);
		}else{
			TC_size=From_size_1+To_size_1+From_size_2+To_size_2;
		}
		
		if(Direct_DLS_Activated){
			int From_DLS_size=0;
			for (size_t i=0; i<LabelDLS_From.size(); i++){
				From_DLS_size+=LabelDLS_From[i].size()*2; //each is a pair of int;
			}
			int To_DLS_size=0;
			for (size_t i=0; i<LabelDLS_To.size(); i++){
				To_DLS_size+=LabelDLS_To[i].size()*2; //each is a pair of int;
			}
			DLS_size=From_DLS_size+To_DLS_size;
		}
	}else{
		for (size_t i=0; i<LabelGTUpdate.size(); i++){
			for(size_t j=0; j<LabelGTUpdate[i].size(); j++){
				TC_size+=LabelGTUpdate[i][j].size();
			}
		}
		if(DLS_Activated){
			for (size_t i=0; i<LabelDLS.size(); i++){
				DLS_size+=LabelDLS[i].size()*2; //each is a pair of int;
			}
		}
	}
	
	if(direct_info==DIRECT)
		cout<<"Total indexing size: "<<TC_size+DLS_size+vertex_mapping_table.size()*3+BitLabelTreeNode.size()+TreeNodeLevel.size()+node_directory_size+1<<endl;
	else
		cout<<"Total indexing size: "<<TC_size+DLS_size+vertex_mapping_table.size()*3+BitLabelTreeNode.size()+node_directory_size+1<<endl;
	

	cout<<"Start query..."<<endl;	
	struct timeval after_time, before_time;
	int twd_distance, bfs_distance;
	vector<int>::iterator sit, tit;	
	
	if(direct_info==DIRECT){
		gettimeofday(&before_time,NULL);	
		for(sit = src.begin(), tit = trg.begin(); 		sit != src.end(); ++sit, ++tit){
			twd_distance=queryDirectMask(vertex_mapping_table, BitLabelTreeNode, TreeNodeLevel, node_directory, LabelGT_From_Update, LabelGT_To_Update, LabelGT_From_Mask, LabelGT_To_Mask, tree_height, *sit, *tit);
			if (Direct_DLS_Activated==true){
				int dls_distance=queryDirectDLS(LabelDLS_From, LabelDLS_To, *sit, *tit);
				twd_distance=dls_distance<twd_distance? dls_distance : twd_distance;
			}
		}
		gettimeofday(&after_time,NULL);
		float query_time=(after_time.tv_sec - before_time.tv_sec)*1000.0 + (after_time.tv_usec - before_time.tv_usec)*1.0/1000.0;
		cout<<"twd distance query time: "<<query_time<<endl;
		
		
	}else{
		gettimeofday(&before_time,NULL);	
		for(sit = src.begin(), tit = trg.begin(); 		sit != src.end(); ++sit, ++tit){
			twd_distance=query(vertex_mapping_table, BitLabelTreeNode, node_directory, LabelGTUpdate, tree_height, *sit, *tit);
			if(DLS_Activated==true){
				int dls_distance=queryDLS(LabelDLS, *sit, *tit);
				twd_distance=dls_distance<twd_distance? dls_distance : twd_distance;
			}
		}
		gettimeofday(&after_time,NULL);
		float query_time=(after_time.tv_sec - before_time.tv_sec)*1000.0 + (after_time.tv_usec - before_time.tv_usec)*1.0/1000.0;
		cout<<"twd distance query time: "<<query_time<<endl;
		
	}

	cout<<"End Query..."<<endl;
	
	if(verify_enable==1){
		cout<<"Start query verification."<<endl;
		if(direct_info==DIRECT){
			for(sit = src.begin(), tit = trg.begin(); 		sit != src.end(); ++sit, ++tit){
				twd_distance=queryDirectMask(vertex_mapping_table, BitLabelTreeNode, TreeNodeLevel, node_directory, LabelGT_From_Update, LabelGT_To_Update, LabelGT_From_Mask, LabelGT_To_Mask, tree_height, *sit, *tit);
				if (Direct_DLS_Activated==true){
					int dls_distance=queryDirectDLS(LabelDLS_From, LabelDLS_To, *sit, *tit);
					twd_distance=dls_distance<twd_distance? dls_distance : twd_distance;
				}
				bfs_distance=GraphUtil::BFSDistanceDirect(g_orig, *sit, *tit);
				if(twd_distance!=bfs_distance){
					cout<<"twd_distance direct: "<<twd_distance<<endl;
					cout<<"bfs_distance direct: "<<bfs_distance<<endl;
				}
			}
		
		}else{
			for(sit = src.begin(), tit = trg.begin(); 		sit != src.end(); ++sit, ++tit){
				twd_distance=query(vertex_mapping_table, BitLabelTreeNode, node_directory, LabelGTUpdate, tree_height, *sit, *tit);
				if(DLS_Activated==true){
					int dls_distance=queryDLS(LabelDLS, *sit, *tit);
					twd_distance=dls_distance<twd_distance? dls_distance : twd_distance;
				}
				bfs_distance=GraphUtil::BFSDistance(g_orig, *sit, *tit);
				if(twd_distance!=bfs_distance){
					cout<<"twd_distance: "<<twd_distance<<endl;
					cout<<"bfs_distance: "<<bfs_distance<<endl;
				}
			}
		}

		cout<<"end query verification."<<endl;
	}
	else{
		cout<<"verification option waived."<<endl;
	}
	
	//find the minimum distance
	cout<<"end of the program"<<endl;
	

}
