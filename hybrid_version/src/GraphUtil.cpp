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

void GraphUtil::generate_queryfile(vector<int>& src, vector<int>& trg, Graph& g, const char* queryfile, const int& query_num)
{
	ofstream outquery(queryfile);
	int gsize=g.num_vertices();
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
		if (g.vertexIDMap.size()==0)
			outquery<<ss<<"	"<<tt<<endl;
		else
			outquery<<g.vertexIDMap[ss]<<"	"<<g.vertexIDMap[tt]<<endl;
		++left;
	}
	outquery.close();
	
	cout<<"query num:"<<query_num<<endl;	
}

void GraphUtil::load_queryfile(vector<int>& src, vector<int>& trg, Graph& g, const char* queryfile)
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
		if(g.vertexIDReverseMap.size()==0){
			src.push_back(ss);
			trg.push_back(tt);
		}else{
			src.push_back(g.vertexIDReverseMap[ss]);
			trg.push_back(g.vertexIDReverseMap[tt]);
		}
		left++;
	}
	inputquery.close();
	cout<<"query num:"<<left<<endl;
}

void GraphUtil::generate_query(vector<int>& src, vector<int>& trg, Graph& g, const int& query_num)
{
	int gsize=g.num_vertices();
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

void GraphUtil::DecentralizeDirect(Graph& g, const int& starting_vertex, vector<vector<VertexDistancePair> >& LabelDLS_From, vector<vector<VertexDistancePair> >& LabelDLS_To)
{
	vector<bool> visited_list_From(g.num_vertices(), false);
	queue<VertexDistancePair> q;//First is vertex ID, second is distance;
	//cout<<"decentralizing vertex "<<starting_vertex<<endl;
	LabelDLS_From[starting_vertex].push_back(pair<int, int>(starting_vertex, 0));
	visited_list_From[starting_vertex]=true;
	q.push(pair<int, int>(starting_vertex,0));
	while(q.size()>0){
		pair<int, int> vertex_distance=q.front();
		q.pop();
		EdgeList::iterator START, END;
		START=g.out_edges(vertex_distance.first).begin();//This is very trick. Remind: This is a decentralize vertex, not the from or to vertex.
		END=g.out_edges(vertex_distance.first).end();
		for(EdgeList::iterator it=START; it!=END; it++){
			if(visited_list_From[*it]==true)
				continue;
				
			visited_list_From[*it]=true;
			LabelDLS_From[*it].push_back(pair<int, int>(starting_vertex, vertex_distance.second+1));
			q.push(pair<int, int>(*it, vertex_distance.second+1));
		}
	}
	
	
	if (!q.empty()){
		cout<<"Internal Logic Error: queue should be empty."<<endl;
		exit(-1);
	}
	
	vector<bool> visited_list_To(g.num_vertices(), false);
	LabelDLS_To[starting_vertex].push_back(pair<int, int>(starting_vertex, 0));
	visited_list_To[starting_vertex]=true;
	q.push(pair<int, int>(starting_vertex,0));	
	while(q.size()>0){
		pair<int, int> vertex_distance=q.front();
		q.pop();
		EdgeList::iterator START, END;
		START=g.in_edges(vertex_distance.first).begin();
		END=g.in_edges(vertex_distance.first).end();
		for(EdgeList::iterator it=START; it!=END; it++){
			if(visited_list_To[*it]==true)
				continue;
				
			visited_list_To[*it]=true;
			LabelDLS_To[*it].push_back(pair<int, int>(starting_vertex, vertex_distance.second+1));
			q.push(pair<int, int>(*it, vertex_distance.second+1));
		}
	}
	g.clearVertex(starting_vertex);
}

void GraphUtil::Decentralize(Graph& g, const int& starting_vertex, vector<vector<VertexDistancePair> >& LabelDLS)
{
	vector<bool> visited_list(g.num_vertices(), false);
	queue<VertexDistancePair> q;//First is vertex ID, second is distance;
	//cout<<"decentralizing vertex "<<starting_vertex<<endl;
	LabelDLS[starting_vertex].push_back(pair<int, int>(starting_vertex, 0));
	visited_list[starting_vertex]=true;
	q.push(pair<int, int>(starting_vertex,0));
	while(q.size()>0){
		pair<int, int> vertex_distance=q.front();
		q.pop();
		for(EdgeList::iterator it=g.get_neighbors(vertex_distance.first).begin(); it!=g.get_neighbors(vertex_distance.first).end(); it++){
			if(visited_list[*it]==true)
				continue;
				
			visited_list[*it]=true;
			LabelDLS[*it].push_back(pair<int, int>(starting_vertex, vertex_distance.second+1));
			q.push(pair<int, int>(*it, vertex_distance.second+1));
		}
	}
	g.clearVertex(starting_vertex);
}

void GraphUtil::loadDLS(vector<vector<VertexDistancePair> >& LabelDLS, string& input_LabelDLS){
	ifstream if_LabelDLS(input_LabelDLS.c_str());
	int first, second;
	int left=0;
	cout<<"load LabelDLS..."<<endl;
	string sub, buf;
	while (getline(if_LabelDLS, buf)) {
		size_t separator=buf.find("	");
		while (separator!=string::npos){
			sub=buf.substr(0, separator);
			buf.erase(0, sub.length()+1);
			size_t split=sub.find(" ");
			string sub_1=sub.substr(0, split);
			string sub_2=sub.substr(split+1);
			istringstream(sub_1) >> first;
			istringstream(sub_2) >> second;
			LabelDLS[left].push_back(VertexDistancePair(first, second));
			separator=buf.find("	");
		}
		size_t split=buf.find(" ");
		if(split!=string::npos){
			string sub_1=buf.substr(0, split);
			string sub_2=buf.substr(split+1);
			istringstream(sub_1) >> first;
			istringstream(sub_2) >> second;
			LabelDLS[left].push_back(VertexDistancePair(first, second));
		}
		LabelDLS[left].resize(LabelDLS[left].size());
		left++;
	}
	if_LabelDLS.close();
	cout<<"loaded LabelDLS size:"<<left<<endl;
}

void GraphUtil::saveDLS(vector<vector<VertexDistancePair> >& LabelDLS, string& output_LabelDLS){
	ofstream of_LabelDLS(output_LabelDLS.c_str());
	for(vector<vector<VertexDistancePair> >::iterator vvit=LabelDLS.begin(); vvit!=LabelDLS.end(); vvit++){
		vector<VertexDistancePair>::iterator vit1=vvit->begin();
		vit1++;
		for(vector<VertexDistancePair>::iterator vit=vvit->begin(); vit!=vvit->end(); vit++){
			of_LabelDLS<<vit->first<<" "<<vit->second;
			if (vit1!=vvit->end()){
				of_LabelDLS<<"	";
				vit1++;
			}
		}
		of_LabelDLS<<endl;
	}
}

void GraphUtil::saveDLSDirect(vector<vector<VertexDistancePair> >& LabelDLS_From, vector<vector<VertexDistancePair> >& LabelDLS_To, string& output_LabelDLS_From, string& output_LabelDLS_To){
	ofstream of_LabelDLS_From(output_LabelDLS_From.c_str());
	for(vector<vector<VertexDistancePair> >::iterator vvit=LabelDLS_From.begin(); vvit!=LabelDLS_From.end(); vvit++){
		vector<VertexDistancePair>::iterator vit1=vvit->begin();
		vit1++;
		for(vector<VertexDistancePair>::iterator vit=vvit->begin(); vit!=vvit->end(); vit++){
			of_LabelDLS_From<<vit->first<<" "<<vit->second;
			if (vit1!=vvit->end()){
				of_LabelDLS_From<<"	";
				vit1++;
			}
		}
		of_LabelDLS_From<<endl;
	}
	
	ofstream of_LabelDLS_To(output_LabelDLS_To.c_str());
	for(vector<vector<VertexDistancePair> >::iterator vvit=LabelDLS_To.begin(); vvit!=LabelDLS_To.end(); vvit++){
		vector<VertexDistancePair>::iterator vit1=vvit->begin();
		vit1++;
		for(vector<VertexDistancePair>::iterator vit=vvit->begin(); vit!=vvit->end(); vit++){
			of_LabelDLS_To<<vit->first<<" "<<vit->second;
			if (vit1!=vvit->end()){
				of_LabelDLS_To<<"	";
				vit1++;
			}
		}
		of_LabelDLS_To<<endl;
	}	
}

void GraphUtil::loadDLSDirect(vector<vector<VertexDistancePair> >& LabelDLS_From, vector<vector<VertexDistancePair> >& LabelDLS_To, string& input_LabelDLS_From, string& input_LabelDLS_To){
	ifstream if_LabelDLS_From(input_LabelDLS_From.c_str());
	int first, second;
	int left=0;
	cout<<"load LabelDLS_From..."<<endl;
	string sub, buf;
	while (getline(if_LabelDLS_From, buf)) {
		size_t separator=buf.find("	");
		while (separator!=string::npos){
			sub=buf.substr(0, separator);
			buf.erase(0, sub.length()+1);
			int split=sub.find(" ");
			string sub_1=sub.substr(0, split);
			string sub_2=sub.substr(split+1);
			istringstream(sub_1) >> first;
			istringstream(sub_2) >> second;
			LabelDLS_From[left].push_back(VertexDistancePair(first, second));
			separator=buf.find("	");
		}
		size_t split=buf.find(" ");
		if(split!=string::npos){
			string sub_1=buf.substr(0, split);
			string sub_2=buf.substr(split+1);
			istringstream(sub_1) >> first;
			istringstream(sub_2) >> second;
			LabelDLS_From[left].push_back(VertexDistancePair(first, second));
		}
		LabelDLS_From[left].resize(LabelDLS_From[left].size());
		left++;
	}
	if_LabelDLS_From.close();
	cout<<"loaded LabelDLS_From size:"<<left<<endl;
	
	ifstream if_LabelDLS_To(input_LabelDLS_To.c_str());
	left=0;
	cout<<"load LabelDLS_To..."<<endl;
	while (getline(if_LabelDLS_To, buf)) {
		size_t separator=buf.find("	");
		while (separator!=string::npos){
			sub=buf.substr(0, separator);
			buf.erase(0, sub.length()+1);
			size_t split=sub.find(" ");
			string sub_1=sub.substr(0, split);
			string sub_2=sub.substr(split+1);
			istringstream(sub_1) >> first;
			istringstream(sub_2) >> second;
			LabelDLS_To[left].push_back(VertexDistancePair(first, second));
			separator=buf.find("	");
		}
		size_t split=buf.find(" ");
		if(split!=string::npos){
			string sub_1=buf.substr(0, split);
			string sub_2=buf.substr(split+1);
			istringstream(sub_1) >> first;
			istringstream(sub_2) >> second;
			LabelDLS_To[left].push_back(VertexDistancePair(first, second));
		}
		LabelDLS_To[left].resize(LabelDLS_To[left].size());
		left++;
	}
	if_LabelDLS_To.close();
	cout<<"loaded LabelDLS_To size:"<<left<<endl;	
}

int GraphUtil::BiBFSDistance(Graph& g, const int& source, const int& destination){
	if (source==destination)
		return 0;
	vector<bool> visited_list_source(g.num_vertices(), false);
	vector<bool> visited_list_destination(g.num_vertices(), false);
	queue<VertexDistancePair> q;//vertex distance pair;
	q.push(pair<int,int>(source, 0));
	visited_list_source[source]=true;
	q.push(pair<int,int>(destination, 0));
	visited_list_destination[destination]=true;
	bool is_source_turn=true;
	int source_count=1;
	int dest_count=1;
	while(q.size()>0){
		if(is_source_turn){
			int next_round_count=0;
			while(source_count>0){
				pair<int, int> vertex_distance=q.front();
				source_count--;
				q.pop();
				for(EdgeList::iterator it=g.get_neighbors(vertex_distance.first).begin(); it!=g.get_neighbors(vertex_distance.first).end(); it++){
					if(visited_list_destination[*it]==true)
						return vertex_distance.second*2+1;
					if(visited_list_source[*it]==true)
						continue;
					visited_list_source[*it]=true;
					q.push(pair<int,int>(*it,vertex_distance.second+1));
					next_round_count++;
				}
			}
			if(next_round_count==0)
				return MAX_VAL;
			else
				source_count=next_round_count;
		}else{
			int next_round_count=0;
			while(dest_count>0){
				pair<int, int> vertex_distance=q.front();
				dest_count--;
				q.pop();
				for(EdgeList::iterator it=g.get_neighbors(vertex_distance.first).begin(); it!=g.get_neighbors(vertex_distance.first).end(); it++){
					if(visited_list_source[*it]==true)
						return (vertex_distance.second+1)*2;
					if(visited_list_destination[*it]==true)
						continue;
					visited_list_destination[*it]=true;
					q.push(pair<int,int>(*it,vertex_distance.second+1));
					next_round_count++;
				}
			}
			if(next_round_count==0)
				return MAX_VAL;
			else
				dest_count=next_round_count;
		}
		is_source_turn=!is_source_turn;
	}

	return MAX_VAL;
}

int GraphUtil::BiBFSDistanceDirect(Graph& g, const int& source, const int& destination){//pair<int,int> version
	if (source==destination)
		return 0;
	vector<bool> visited_list_source(g.num_vertices(), false);
	vector<bool> visited_list_destination(g.num_vertices(), false);
	queue<VertexDistancePair> q;//vertex distance pair;
	q.push(pair<int,int>(source, 0));
	visited_list_source[source]=true;
	q.push(pair<int,int>(destination, 0));
	visited_list_destination[destination]=true;
	bool is_source_turn=true;
	int source_count=1;
	int dest_count=1;
	while(q.size()>0){
		if(is_source_turn){
			int next_round_count=0;
			while(source_count>0){
				pair<int, int> vertex_distance=q.front();
				source_count--;
				q.pop();
				for(EdgeList::iterator it=g.out_edges(vertex_distance.first).begin(); it!=g.out_edges(vertex_distance.first).end(); it++){
					if(visited_list_destination[*it]==true)
						return vertex_distance.second*2+1;
					if(visited_list_source[*it]==true)
						continue;
					visited_list_source[*it]=true;
					q.push(pair<int,int>(*it,vertex_distance.second+1));
					next_round_count++;
				}
			}
			if(next_round_count==0)
				return MAX_VAL;
			else
				source_count=next_round_count;
		}else{
			int next_round_count=0;
			while(dest_count>0){
				pair<int, int> vertex_distance=q.front();
				dest_count--;
				q.pop();
				for(EdgeList::iterator it=g.in_edges(vertex_distance.first).begin(); it!=g.in_edges(vertex_distance.first).end(); it++){
					if(visited_list_source[*it]==true)
						return (vertex_distance.second+1)*2;
					if(visited_list_destination[*it]==true)
						continue;
					visited_list_destination[*it]=true;
					q.push(pair<int,int>(*it,vertex_distance.second+1));
					next_round_count++;
				}
			}
			if(next_round_count==0)
				return MAX_VAL;
			else
				dest_count=next_round_count;
		}
		is_source_turn=!is_source_turn;
	}

	return MAX_VAL;
}

