#include "GraphUtil.h"

// implement tarjan's algorithm to find Strongly Connected Component from a given start node
void GraphUtil::tarjan(Graph& g, int vid, int& index, hash_map< int, pair<int,int> >& order, 
	vector<int>& sn, multimap<int,int>& sccmap, int& scc) {

	order[vid].first = index;
	order[vid].second = index;
	index++;
	sn.push_back(vid);
	g[vid].visited = true;
	EdgeList el = g.out_edges(vid);
	EdgeList::iterator eit;
	for (eit = el.begin(); eit != el.end(); eit++) {
		if (!g[*eit].visited) {
			tarjan(g, *eit, index, order, sn, sccmap, scc);
			order[vid].second = min(order[*eit].second, order[vid].second);
		}
		else if (find(sn.begin(), sn.end(), (*eit)) != sn.end()) {
			order[vid].second = min(order[*eit].first, order[vid].second);
		}
	}

	if (order[vid].first == order[vid].second) {
		vector<int>::reverse_iterator rit;
		for (rit = sn.rbegin(); rit != sn.rend(); rit++) {
			if ((*rit) != vid) {
				sccmap.insert(make_pair(scc, *rit));
			//	sccmap[*rit] = scc;
				sn.pop_back();
			}
			else {
				sccmap.insert(make_pair(scc, *rit));
			//	sccmap[*rit] = scc;
				sn.pop_back();
				break;
			}
		}
		scc++;
	}
}

// merge Strongly Connected Component
// return vertex map between old vertex and corresponding new merged vertex
void GraphUtil::mergeSCC(Graph& g, vector<int>& on) {

	vector<int> sn;
	hash_map< int, pair<int, int> > order;
	int ind = 0;
	multimap<int, int> sccmap;	// each vertex id correspond with a scc num 
	int scc = 0;
	int vid;
	int origsize = g.num_vertices();
	
	for (int i = 0; i < origsize; i++) {
		vid = i;
		if (g[vid].visited)
			continue;
		tarjan(g, vid, ind, order, sn, sccmap, scc);
	//	cout << "here " << i << endl;
	}
	// no component need to merge
	if (scc == origsize) {
		for (int i = 0; i < origsize; i++)
			on[i] = i;
		return;
	}


	hash_map<int, vector<int> > inlist, outlist;
	g.extract(inlist, outlist);
	
	multimap<int,int>::iterator mit;
	mit = sccmap.begin();
	int num_comp;
	int maxid = g.num_vertices()-1;
	while (mit != sccmap.end()) {
		num_comp = mit->first;
		
		if (++sccmap.lower_bound(num_comp) == sccmap.upper_bound(num_comp)) {
			on[mit->second] = mit->second;
			++mit;
			continue;
		}

		maxid++;
		inlist[maxid] = vector<int>();
		outlist[maxid] = vector<int>();
		
		for (; mit != sccmap.upper_bound(num_comp); ++mit) {
			on[mit->second] = maxid;

			vector<int> vec = inlist[mit->second];
			vector<int>::iterator vit, vit1;
			vector<int> vec1;
			bool hasEdge = false;

			// copy all incoming edges
			for (vit = vec.begin(); vit != vec.end(); vit++) {
				hasEdge = false;
				vec1 = outlist[*vit];
				for (vit1 = vec1.begin(); vit1 != vec1.end(); vit1++) {
					if (*vit1 == maxid) {
						hasEdge = true;
						break;
					}
				}
				if (!hasEdge && (*vit != maxid)) {
					inlist[maxid].push_back(*vit);
					outlist[*vit].push_back(maxid);
				}
			}

			// copy all outgoing edges
			vec = outlist[mit->second];
			for (vit = vec.begin(); vit != vec.end(); vit++) {
				hasEdge = false;
				vec1 = inlist[*vit];
				for (vit1 = vec1.begin(); vit1 != vec1.end(); vit1++)
					if (*vit1 == maxid) {
						hasEdge = true;
						break;
					}
				if (!hasEdge && (*vit != maxid)) {
					outlist[maxid].push_back(*vit);
					inlist[*vit].push_back(maxid);
				}
			}
			
			// delete old vertex
			vec = inlist[mit->second];
			for (vit = vec.begin(); vit != vec.end(); vit++) {
				for (vit1 = outlist[*vit].begin(); vit1 != outlist[*vit].end(); )
					if (*vit1 == mit->second)
						outlist[*vit].erase(vit1);
					else
						vit1++;
			}
			vec = outlist[mit->second];
			for (vit = vec.begin(); vit != vec.end(); vit++) {
				for (vit1 = inlist[*vit].begin(); vit1 != inlist[*vit].end(); )
					if (*vit1 == mit->second)
						inlist[*vit].erase(vit1);
					else
						vit1++;
			}
			outlist.erase(mit->second);
			inlist.erase(mit->second);
		}
	}			

	g = Graph(inlist, outlist);

	
	// update index map
	hash_map<int,int> indexmap;
	hash_map<int, vector<int> >::iterator hit;
	int k;
	for (hit = outlist.begin(), k=0; hit != outlist.end(); hit++, k++) {
		indexmap[hit->first] = k;
	}

	for (k = 0; k < origsize; k++)
		on[k] = indexmap[on[k]];
}

int GraphUtil::BFSDistance(Graph& g, const int& source, const int& destination){
	if (source==destination)
		return 0;
	vector<bool> visited_list(g.num_vertices(), false);
	queue<VertexDistancePair> q;//First is vertex ID, second is distance;
	//cout<<"starting vertex "<<source<<endl;
	q.push(pair<int, int>(source,0));
	while(q.size()>0){
		pair<int, int> vertex_distance=q.front();
		q.pop();
		for(EdgeList::iterator it=g.get_neighbors(vertex_distance.first).begin(); it!=g.get_neighbors(vertex_distance.first).end(); it++){
			if(visited_list[*it]==true)
				continue;
			
			if (*it==destination)
				return vertex_distance.second+1;
				
			visited_list[*it]=true;
			q.push(pair<int, int>(*it, vertex_distance.second+1));
		}
	}
	return MAX_VAL;
}

int GraphUtil::BFSDistanceDirect(Graph& g, const int& source, const int& destination){
	if (source==destination)
		return 0;
	vector<bool> visited_list(g.num_vertices(), false);
	queue<VertexDistancePair> q;//First is vertex ID, second is distance;
	//cout<<"starting vertex "<<source<<endl;
	q.push(pair<int, int>(source,0));
	while(q.size()>0){
		pair<int, int> vertex_distance=q.front();
		q.pop();
		for(EdgeList::iterator it=g.out_edges(vertex_distance.first).begin(); it!=g.out_edges(vertex_distance.first).end(); it++){
			if(visited_list[*it]==true)
				continue;
			
			if (*it==destination)
				return vertex_distance.second+1;
				
			visited_list[*it]=true;
			q.push(pair<int, int>(*it, vertex_distance.second+1));
		}
	}
	return MAX_VAL;
}

void GraphUtil::generate_queryfile(vector<int>& src, vector<int>& trg, const int& gsize, const char* queryfile, const int& query_num)
{
	ofstream outquery(queryfile);
	int ss,tt;
	int left=0;
	srand( time(NULL));
	cout << "generating queries..." << endl;
	while (left < query_num) {
		ss = rand() % gsize;
		tt = rand() % gsize;
		if (ss == tt) continue;
		src.push_back(ss);
		trg.push_back(tt);
		outquery<<ss<<"	"<<tt<<endl;
		++left;
	}
	outquery.close();
	
	cout<<"query num:"<<query_num<<endl;	
}

void GraphUtil::load_queryfile(vector<int>& src, vector<int>& trg, const char* queryfile)
{
	ifstream inputquery(queryfile);
	int ss,tt;
	int left=0;
	cout<<"load query file..."<<endl;
	string sub, buf;
	while (getline(inputquery, buf)) {
		int separator=buf.find("	");
		string sub_s=buf.substr(0, separator);
		string sub_t=buf.substr(separator+1);
		istringstream(sub_s) >> ss;
		istringstream(sub_t) >> tt;
		src.push_back(ss);
		trg.push_back(tt);
		left++;
	}
	inputquery.close();
	cout<<"query num:"<<left<<endl;
}

void GraphUtil::generate_query(vector<int>& src, vector<int>& trg, const int& gsize, const int& query_num)
{
	int ss,tt;
	int left=0;
	srand( time(NULL));
	cout << "generating queries..." << endl;
	while (left < query_num) {
		ss = rand() % gsize;
		tt = rand() % gsize;
		if (ss == tt) continue;
		src.push_back(ss);
		trg.push_back(tt);
		++left;
	}
	cout<<"query num:"<<query_num<<endl;	
}
