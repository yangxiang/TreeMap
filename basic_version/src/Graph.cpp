#include "Graph.h"

Graph::Graph() {
	graph = GRA();
	graph_direct = GRA_direct();
	vl = VertexList();
}

Graph::Graph(int size) {//For undirected
	vsize = size;
	vl = VertexList(size);
	graph = GRA(size, EdgeList());
}

Graph::Graph(int size, double c){//For random generating a directed graph
	int threshold = int(c*100);
	int rand_num;
	for (int i = 0; i < size; i++) 
		addVertex(i, DIRECT);

	srand(time(NULL));
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++)
			if (i != j) {
				rand_num = rand() % (size*100);
				if (rand_num < threshold){
					addEdge(i,j);
					addEdge(j,i);
					addEdgeDirect(i,j);
				}
			}
	}
	for(int i=0; i<int(vl.size()); i++){
		sortEdgeList(graph[i], i);
		sortEdgeList(graph_direct[i].inList, i);
		sortEdgeList(graph_direct[i].outList, i);
	}	
}

Graph::Graph(GRA& g, VertexList& vlist) {//For undirected
	vsize = vlist.size();
	graph = g;
	vl = vlist;
}

Graph::Graph(istream& in, const construction_type& graph_format, const graph_type& direct_info) {
	if (graph_format==ADJ_GRAPH)
		readGraphAdj(in, direct_info);
	else if (graph_format==MATRIX_GRAPH)
		readGraphMatrix(in, direct_info);
	else if (graph_format==TEDI_ZERO)
		readGraphTediZero(in, direct_info);
	else if (graph_format==TEDI_ONE)
		readGraphTediOne(in, direct_info);
	else
		cout<<"unrecognized graph construction type"<<endl;
	
	//Sort to make sure adjacent vertices in ascending order and eliminate self-inclusion (required by the following code).
	for(int i=0; i<int(vl.size()); i++){
		sortEdgeList(graph[i], i);
		if(direct_info==DIRECT){
			sortEdgeList(graph_direct[i].inList, i);
			sortEdgeList(graph_direct[i].outList, i);
		}
	}
}

Graph::Graph(hash_map<int,vector<int> >& inlist, hash_map<int,vector<int> >& outlist) {

	vsize = inlist.size();
	vl = VertexList(vsize);
	graph_direct = GRA_direct(vsize, In_OutList());
	graph = GRA (vsize, EdgeList());
	graph_type direct_info=DIRECT;
	for (int i = 0; i < vsize; i++)
		addVertex(i, direct_info);
	hash_map<int,vector<int> >::iterator hit, hit1;
	hash_map<int,int> indexmap;
	vector<int> vec;
	vector<int>::iterator vit;
	int k;

	for (hit = inlist.begin(), k = 0; hit != inlist.end(); hit++,k++) {
		indexmap[hit->first] = k;
	}

	for (hit = inlist.begin(), hit1 = outlist.begin(), k = 0; hit != inlist.end(); hit++, hit1++, k++) {
		vec = hit->second;
		for (vit = vec.begin(); vit != vec.end(); vit++){
			graph_direct[k].inList.push_back(indexmap[*vit]);
			graph[k].push_back(indexmap[*vit]);
			graph[indexmap[*vit]].push_back(k);
		}
		vec = hit1->second;
		for (vit = vec.begin(); vit != vec.end(); vit++){
			graph_direct[k].outList.push_back(indexmap[*vit]);
			graph[k].push_back(indexmap[*vit]);
			graph[indexmap[*vit]].push_back(k);
		}
	}

	//Sort to make sure adjacent vertices in ascending order and eliminate self-inclusion (required by the following code).
	for(int i=0; i<int(vl.size()); i++){
		sortEdgeList(graph[i], i);
		sortEdgeList(graph_direct[i].inList, i);
		sortEdgeList(graph_direct[i].outList, i);
	}
	//printGraph();

}

void Graph::sortEdgeList(EdgeList& EL, int& vertex){
	if(EL.size()==0)
		return;
	sort(EL.begin(), EL.end());
	EdgeList unrepeated;
	int previous_vertex=*(EL.begin());
	if(previous_vertex!=vertex)
		unrepeated.push_back(previous_vertex);
	for(EdgeList::iterator it=EL.begin(); it!=EL.end(); it++){
		if(*it==previous_vertex || *it==vertex)//remove repeat and self vertices.
			continue;
		
		unrepeated.push_back(*it);
		previous_vertex=*it;
		
	}
	EL=unrepeated;
}

void Graph::extract(hash_map<int,vector<int> >& inlist, hash_map<int,vector<int> >& outlist) {
	for (int i = 0; i < int(vl.size()); i++) {
		inlist[i] = graph_direct[i].inList;
		outlist[i] = graph_direct[i].outList;
	}
//	printMap(inlist,outlist);
}

bool Graph::isSymmetric() const{
//The function is a simple version and it is only used occasionally for debugging.
//we can speed up this function by using sorting and binary search, and adjusting visit status of a vertex. 
	for(GRA::const_iterator adj_it=graph.begin(); adj_it!=graph.end(); adj_it++){
		int current_vertex_id=distance(graph.begin(), adj_it);
		//cout<<"current_vertex_id: "<<current_vertex_id<<endl;
		//cout<<"adj_it->size(): "<<adj_it->size()<<endl;
		for(EdgeList::const_iterator ite=adj_it->begin(); ite<adj_it->end(); ite++){
			//cout<<"*ite: "<<*ite<<endl;
			if(*ite==current_vertex_id){
				cout<<"Format error: a vertex edge list contains itself."<<endl;
				cout<<"*ite: "<<*ite;
				exit(-1);
			}
			else {
				if(*ite>=int(graph.size())){
					cout<<"Error in graph adjacent list."<<endl;
					//cout<<*ite;
					exit(-1);
				}
				
				EdgeList::const_iterator jte=graph[*ite].begin();
				//cout<<"jte: ";
				for(; jte<graph[*ite].end(); jte++){
					//cout<<*jte<<" ";
					if(*jte==current_vertex_id)
						break;
				}
				//cout<<endl;
				if (jte==graph[*ite].end()){
					cout<<"unsymmetric detected: "<<current_vertex_id<<", "<<*ite<<endl;
					return false;
				}
			}
		}	
	}
	return true;
}

Graph::~Graph() {}

void Graph::printGraph() {
	writeGraph(cout);
}

void Graph::clear() {
	vsize = 0;
	graph.clear();
	vl.clear();
}

void Graph::strTrimRight(string& str) {
	string whitespaces(" \t");
	size_t index = str.find_last_not_of(whitespaces);
	if (index != string::npos) 
		str.erase(index+1);
	else
		str.clear();
}

void Graph::readGraphAdj(istream& in, const graph_type& direct_info) {
	string buf;
	getline(in, buf);

	strTrimRight(buf);
	if (buf != "graph_for_indexing") {
		cout << "BAD FILE FORMAT!" << endl;
	//	exit(0);
	}
	
	int n;
	getline(in, buf);
	istringstream(buf) >> n;
	// initialize
	vsize = n;
	vl = VertexList(n);
	graph = GRA(n, EdgeList());	
	if(direct_info==DIRECT)
		graph_direct=GRA_direct(n, In_OutList());

	for (int i = 0; i < n; i++)
		addVertex(i, direct_info);

	string sub;
	int idx;
	int sid = 0;
	int tid = 0;
	while (getline(in, buf)) {
		strTrimRight(buf);
		idx = buf.find(":");
		buf.erase(0, idx+2);
		while (buf.find(" ") != string::npos) {
			sub = buf.substr(0, buf.find(" "));
			istringstream(sub) >> tid;
			buf.erase(0, buf.find(" ")+1);
			addEdge(sid, tid);
			//cout<<"add edge: "<<sid<<","<<tid<<endl;
			addEdge(tid, sid);
			if(direct_info==DIRECT){
				addEdgeDirect(sid, tid);
			}
		}
		++sid;
	}
}

void Graph::readGraphTediZero(istream& in, const graph_type& direct_info){
	int n;
	string buf;
	getline(in, buf);
	istringstream(buf) >> n;
	// initialize
	vsize = n;
	vl = VertexList(n);
	graph = GRA(n, EdgeList());
	if(direct_info==DIRECT)
		graph_direct=GRA_direct(n, In_OutList());	

	for (int i = 0; i < n; i++)
		addVertex(i, direct_info);

	string sub;
	int sid = 0;
	int tid = 0;
	while (getline(in, buf)) {
		sub = buf.substr(0, buf.find(" "));
		istringstream(sub) >> sid;
		buf.erase(0, buf.find(" ")+1);
		istringstream(buf) >>tid;
		addEdge(sid, tid);
		addEdge(tid, sid);//for the DBLP data
		if(direct_info==DIRECT){
			addEdgeDirect(sid, tid);
		}
	}
}

void Graph::readGraphTediOne(istream& in, const graph_type& direct_info){
	int n;
	string buf;
	getline(in, buf);
	istringstream(buf) >> n;
	// initialize
	vsize = n;
	vl = VertexList(n);
	graph = GRA(n, EdgeList());
	if(direct_info==DIRECT)
		graph_direct=GRA_direct(n, In_OutList());	

	for (int i = 0; i < n; i++)
		addVertex(i, direct_info);

	string sub;
	int sid = 0;
	int tid = 0;
	while (getline(in, buf)) {
		sub = buf.substr(0, buf.find(" "));
		istringstream(sub) >> sid;
		buf.erase(0, buf.find(" ")+1);
		istringstream(buf) >>tid;
		addEdge(sid-1, tid-1);//start from 1
		addEdge(tid-1, sid-1);//start from 1
		if(direct_info==DIRECT){
			addEdgeDirect(sid, tid);
		}
	}
}

void Graph::readGraphMatrix(istream& in, const graph_type& direct_info) {
  string fi_line;
  uint64_t row_index=1;
  uint64_t previous_col_index=0;
  cout<<"start reading matrix"<<endl;
  while (getline(in, fi_line))
  {

    int item_digit_len;

    char *fi_line_scan=new char[fi_line.size()+1]; 
	strcpy(fi_line_scan, fi_line.c_str());
	uint64_t col_index=1;
	WeightMeasure weight;
	char *fi_line_pointer=fi_line_scan;
    while (sscanf(fi_line_pointer, "%f%n", &weight, &item_digit_len) == 1)
    {
      if (weight < -std::numeric_limits<WeightMeasure>::max() || weight > std::numeric_limits<WeightMeasure>::max())
      {
		std::cout << "weight out of range: " << weight << "\n";
		exit(1);
      }
	  else
	  {
		if ((weight>=WEIGHT_CUTOFF)&&(row_index!=col_index)){
			addEdge(row_index-1,col_index-1);//vertex id start from 0
			if(direct_info==DIRECT){
				addEdgeDirect(row_index-1, col_index-1);
			}			
		}
	  }
      fi_line_pointer += item_digit_len;
	  col_index++;
    }
	delete[] fi_line_scan;
	fi_line_pointer=NULL;
	
	//Check equality of column numbers.
	if(previous_col_index==0)
	{
		previous_col_index=col_index;
	}
	else
	{
		if(previous_col_index!=col_index)
		{
			std::cout << "Dataset format is wrong. Column numbers are not equal. Exit \n";
			exit(1);
		}
	}
	//Check equality of column numbers done.

	row_index++;
  }
}

void Graph::writeGraph(ostream& out) {
	cout << "Graph size = " << graph.size() << endl;
	out << "graph_for_indexing" << endl;
	out << vl.size() << endl;

	GRA::iterator git;
	EdgeList el;
	EdgeList::iterator eit;
	VertexList::iterator vit;
	size_t i = 0;
	if(graph_direct.size()>0){//Output the direct version when available 
		for (i = 0; i < vl.size(); i++) {
			out << i << ": ";
			el = graph_direct[i].outList;
			for (eit = el.begin(); eit != el.end(); eit++)
				out << (*eit) << " ";
			out << "#" << endl;
		}
	}else{//output the undirect version if the direct version is not available
		for (i = 0; i < vl.size(); i++) {
			out << i << ": ";
			el = graph[i];
			for (eit = el.begin(); eit != el.end(); eit++)
				out << (*eit) << " ";
			out << "#" << endl;
		}		
	}
}

void Graph::addVertex(size_t vid, const graph_type& direct_info) {
	if (vid >= vl.size()) {
		size_t size = vl.size();
		for (size_t i = 0; i < (vid-size+1); i++) {
			graph.push_back(EdgeList());
			if(direct_info==DIRECT){
				graph_direct.push_back(In_OutList());
			}
			vl.push_back(Vertex());
		}
		vsize = vl.size();
	}
}

void Graph::addEdge(size_t sid, size_t tid) {//only add one edge. To add a pair of edges, call it twice.
	graph_type direct_info=UNDIRECT;
	if (sid >= vl.size())
		addVertex(sid, direct_info);
	if (tid >= vl.size())
		addVertex(tid, direct_info);
	// update edge list
	graph[sid].push_back(tid);
}

void Graph::addEdgeDirect(size_t sid, size_t tid) {
	graph_type direct_info=DIRECT;
	if (sid >= vl.size())
		addVertex(sid, direct_info);
	if (tid >= vl.size())
		addVertex(tid, direct_info);
	// update edge list
	graph_direct[sid].outList.push_back(tid);
	graph_direct[tid].inList.push_back(sid);

}


int Graph::num_edges() {
	EdgeList el;
	GRA::iterator git;
	int num = 0;
	for (git = graph.begin(); git != graph.end(); git++) {
		el = *git;
		num += el.size();
	}
	return num/2;//each edge is calculated twice
}

int Graph::num_directed_edges() {
	EdgeList el;
	GRA_direct::iterator git;
	int num = 0;
	for (git = graph_direct.begin(); git != graph_direct.end(); git++) {
		el = git->outList;
		num += el.size();
	}
	return num;
}

// check whether the edge from src to trg is in the graph
bool Graph::hasEdge(int src, int trg) {
	EdgeList el = graph[src];
	EdgeList::iterator ei;
	for (ei = el.begin(); ei != el.end(); ei++)
		if ((*ei) == trg)
			return true;
	return false;

}

Graph& Graph::operator=(const Graph& g) {
	if (this != &g) {
		graph = g.graph;
		graph_direct = g.graph_direct;
		vl = g.vl;
		vsize = g.vsize;
	}
	return *this;
}
