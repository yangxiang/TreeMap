#ifndef _GRAPH_UTIL_H_
#define _GRAPH_UTIL_H_

#include<map>
#include<queue>
#include<tr1/unordered_set>
#include "Graph.h"
#include <sys/time.h>

typedef pair<int,  int> VertexDistancePair; //first is vertex ID; second is distance.

class GraphUtil {
	public:
		static void tarjan(Graph& g, int vid, int& index, hash_map< int, pair<int,int> >& order, vector<int>& sn, 
			multimap<int, int>& sccmap, int& scc);
		static void mergeSCC(Graph& g, vector<int>& on);
		static int BFSDistance(Graph& g, const int& source, const int& destination);
		static int BiBFSDistance(Graph& g, const int& source, const int& destination);
		static int BiBFSDistanceDirect(Graph& g, const int& source, const int& destination);
		static int BFSDistanceDirect(Graph& g, const int& source, const int& destination);
		static void generate_queryfile(vector<int>& src, vector<int>& trg, Graph& g, const char* queryfile, const int& query_num);
		static void load_queryfile(vector<int>& src, vector<int>& trg, Graph& g, const char* queryfile);
		static void generate_query(vector<int>& src, vector<int>& trg, Graph& g, const int& query_num);
		static void DecentralizeDirect(Graph& g, const int& v, vector<vector<VertexDistancePair> >& LabelDLS_From, vector<vector<VertexDistancePair> >& LabelDLS_To);
		static void Decentralize(Graph& g, const int& v, vector<vector<VertexDistancePair> >& LabelDLS);
		static void loadDLS(vector<vector<VertexDistancePair> >& LabelDLS, string& input_LabelDLS);
		static void saveDLS(vector<vector<VertexDistancePair> >& LabelDLS, string& output_LabelDLS);
		static void saveDLSDirect(vector<vector<VertexDistancePair> >& LabelDLS_From, vector<vector<VertexDistancePair> >& LabelDLS_To, string& output_LabelDLS_From, string& output_LabelDLS_To);
		static void loadDLSDirect(vector<vector<VertexDistancePair> >& LabelDLS_From, vector<vector<VertexDistancePair> >& LabelDLS_To, string& input_LabelDLS_From, string& input_LabelDLS_To);
};

#endif
