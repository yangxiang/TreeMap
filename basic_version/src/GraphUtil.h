#ifndef _GRAPH_UTIL_H_
#define _GRAPH_UTIL_H_

#include<map>
#include<queue>
#include "Graph.h"
#include <sys/time.h>

typedef pair<int, int> VertexDistancePair; //first is vertex ID; second is distance.

class GraphUtil {
	public:
		static void tarjan(Graph& g, int vid, int& index, hash_map< int, pair<int,int> >& order, vector<int>& sn, 
			multimap<int, int>& sccmap, int& scc);
		static void mergeSCC(Graph& g, vector<int>& on);
		static int BFSDistance(Graph& g, const int& source, const int& destination);
		static int BFSDistanceDirect(Graph& g, const int& source, const int& destination);
		static void generate_queryfile(vector<int>& src, vector<int>& trg, const int& gsize, const char* queryfile, const int& query_num);
		static void load_queryfile(vector<int>& src, vector<int>& trg, const char* queryfile);
		static void generate_query(vector<int>& src, vector<int>& trg, const int& gsize, const int& query_num);
};

#endif
