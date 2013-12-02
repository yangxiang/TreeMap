#ifndef _GENERICTREE_H
#define _GENERICTREE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <cmath>

using namespace std;

struct treenodeinfo {
	treenodeinfo():nodelevel(-1), visited(false), active(true){};
	int nodelevel;
	bool visited;
	bool active;
};
struct neighbor {
	neighbor():nodeID(-1), subtree_size(-1){};
	int nodeID;
	int subtree_size;
};

typedef vector<neighbor> TreeEdgeList;	// edge list represented by vertex id list
typedef vector<TreeEdgeList> GenericTreeStructure;	// tree index
typedef vector<treenodeinfo> NodeInfoList;	// Node list (store node property) indexing by id

class GenericTree {
	private:
		GenericTreeStructure tree_nlist;
		NodeInfoList nl;
		int tree_size;
		
	public:
		GenericTree();
		GenericTree(int);
		GenericTree(GenericTreeStructure&, NodeInfoList&);
		~GenericTree();
		void writeTree(ostream&);
		void printTree();
		void addNode(size_t);
		void addEdge(size_t, size_t);
		int num_nodes();
		int num_edges();
		TreeEdgeList& get_neighbors(int);
		treenodeinfo& operator[](const int);
		int get_degree(int);
		bool hasEdge(int, int);
		GenericTree& operator=(const GenericTree&);
		void clear();
};

inline
int GenericTree::get_degree(int src) {
	return tree_nlist[src].size();
}

// return neighbors of a specified vertex
inline
TreeEdgeList& GenericTree::get_neighbors(int src) {
	return tree_nlist[src];
}

inline
int GenericTree::num_nodes() {
	return tree_nlist.size();
}

// get a specified node property
inline
treenodeinfo& GenericTree::operator[](const int vid) {
	return nl[vid];
}

#endif
