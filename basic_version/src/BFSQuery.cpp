#include "Graph.h"
#include "GraphUtil.h"
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
typedef pair<int, int> VertexDistancePair; //first is vertex ID; second is distance.


static void usage() {
	cout << "\nUsage:\n"
		"	BFSQuery [-h] [-g graph_input_format] [-d direct_info] [-l rw_queryfile] [-q query_filename] [-n query_num] input_filename\n"
		"Description:\n"
		"	-h	Print the help message.\n"
		"	-g	graph_input_format. default value 0 (adjacent list); range graph_input_format={0,1,2,3}.\n"
		"	-d	direct_info. default value 0 (treat as undirected); range direct_info={0,1}.\n"
		"	-l	rw_queryfile. default value -1 (do nothing); range rw_queryfile={-1,0,1}.\n"
		"	-q	query_filename.\n"
		"	-n	query_num. default value 100,000.\n"
		"	filename	The graph file.\n"
		<< endl;
}


int main(int argc, char* argv[]) {
	if (argc == 1) {
		usage();
		return 1;
	}
	string query_filename, input_filename;
	int input_para_counter=1;
//	int source_vertex=-1;
//	int destination_vertex=-1;
	int rw_queryfile=-1;
	construction_type graph_input_format=ADJ_GRAPH;
	graph_type	direct_info=UNDIRECT;
	int query_num=100000;
	
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
	//exit(0);
	//g.writeGraph(outfile);
	
	
	if(rw_queryfile==1){
		GraphUtil::generate_queryfile(src, trg, g.num_vertices(), query_filename.c_str(), query_num);
	}else if(rw_queryfile==0){
		GraphUtil::load_queryfile(src, trg, query_filename.c_str());
	}else{//generate query without loading from or saving into files.
		GraphUtil::generate_query(src, trg, g.num_vertices(), query_num);
	}
	
	
	
	//Symmetric test is enforced right now. In the future, we can allowed directed graphs
	//Note the program requires that a vertex itself does not appear in its adjacent list and the adjacent list is in ascending order.
	if(!g.isSymmetric()){
		cout<<"Internal Error: Intend to treat a directed graph as undirected."<<endl;
		exit(-1);
	}
	
	
	cout<<"Start query..."<<endl;	
	struct timeval after_time, before_time;
	int bfs_distance;
	vector<int>::iterator sit, tit;	
	
	if(direct_info==DIRECT){
		gettimeofday(&before_time,NULL);
		for(sit = src.begin(), tit = trg.begin(); 		sit != src.end(); ++sit, ++tit)
			bfs_distance=GraphUtil::BFSDistanceDirect(g, *sit, *tit);
		gettimeofday(&after_time,NULL);
		float query_time=(after_time.tv_sec - before_time.tv_sec)*1000.0 + (after_time.tv_usec - before_time.tv_usec)*1.0/1000.0;
		cout<<"bfs query time: "<<query_time<<endl;	
	
	}else{
		gettimeofday(&before_time,NULL);
		for(sit = src.begin(), tit = trg.begin(); 		sit != src.end(); ++sit, ++tit)
			bfs_distance=GraphUtil::BFSDistance(g, *sit, *tit);
		gettimeofday(&after_time,NULL);
		float query_time=(after_time.tv_sec - before_time.tv_sec)*1000.0 + (after_time.tv_usec - before_time.tv_usec)*1.0/1000.0;
		cout<<"bfs query time: "<<query_time<<" (ms)"<<endl;
	}
	cout<<"End Query..."<<endl;
	
	
	//find the minimum distance
	cout<<"end of the program"<<endl;
	

}
