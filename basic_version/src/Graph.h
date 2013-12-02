#ifndef _GRAPH_H
#define _GRAPH_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <cmath>
#include <hash_map.h>

using namespace std;

#define MAX_VAL 10000000
#define MIN_VAL -10000000
#define WEIGHT_CUTOFF 0.5

struct Vertex {
	Vertex():active(true), visited(false){};
	bool active;//reserved
	vector<int> nodes;
	bool visited;//for scc merging
};

typedef vector<int> EdgeList;	// edge list represented by vertex id list
typedef vector<Vertex> VertexList;	// vertices list (store real vertex property) indexing by id
typedef vector<EdgeList> GRA;	// index graph
typedef float WeightMeasure; //for handling weighed graphs in matrix representation

struct In_OutList {
	EdgeList inList;
	EdgeList outList;
};
typedef vector<In_OutList> GRA_direct;	// index graph

enum construction_type {ADJ_GRAPH=0, MATRIX_GRAPH=1, TEDI_ZERO=2, TEDI_ONE=3};
enum graph_type {UNDIRECT=0, DIRECT=1};

class Graph {
	private:
		GRA graph;
		GRA_direct graph_direct;
		VertexList vl;
		int vsize;
		void readGraphAdj(istream&, const graph_type&);
		void readGraphMatrix(istream&, const graph_type&);
		void readGraphTediZero(istream&, const graph_type&);//means the smallest vertex is 0.
		void readGraphTediOne(istream&, const graph_type&);//means the smallest verex is 1.
		void addVertex(size_t, const graph_type&);
		void addEdge(size_t, size_t);
		void addEdgeDirect(size_t, size_t);
		void sortEdgeList(EdgeList&, int&);
		
	public:
		Graph();
		Graph(int);
		Graph(int size, double c);
		Graph(GRA&, VertexList&);
		Graph(istream&, const construction_type&, const graph_type&);
		~Graph();
		bool isSymmetric() const;
		void writeGraph(ostream&);
		void printGraph();
		int num_vertices();
		int num_edges();
		int num_directed_edges();
		VertexList& vertices();
		EdgeList& get_neighbors(int);
		EdgeList& out_edges(int);
		EdgeList& in_edges(int);
		int get_degree(int);
		bool hasEdge(int, int);	
		Graph& operator=(const Graph&);
		Vertex& operator[](const int);
		void clear();
		void strTrimRight(string& str);
		Graph(hash_map<int,vector<int> >& inlist, hash_map<int,vector<int> >& outlist);		
		void extract(hash_map<int,vector<int> >& inlist, hash_map<int,vector<int> >& outlist);		
};	

// return neighbors of a specified vertex
inline
EdgeList& Graph::get_neighbors(int src) {
	return graph[src];
}

// return vertex list of graph
inline
VertexList& Graph::vertices() {
	return vl;
}

// get a specified vertex property
inline
Vertex& Graph::operator[](const int vid) {
	return vl[vid];
}

inline
int Graph::get_degree(int src) {
	return graph[src].size();
}

inline
int Graph::num_vertices() {
	return vl.size();
}

inline
EdgeList& Graph::out_edges(int src) {
	return graph_direct[src].outList;
}

// return in edges of specified vertex
inline
EdgeList& Graph::in_edges(int trg) {
	return graph_direct[trg].inList;
}

#endif
