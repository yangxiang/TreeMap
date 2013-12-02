#ifndef _BINARYTREE_H
#define _BINARYTREE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <cmath>

using namespace std;

struct treenode{
	treenode():parent(-1), left_child(-1), right_child(-1), level(-1){};
	int parent;
	int left_child;
	int right_child;
	int level;
};

typedef vector<treenode> BinaryTreeStructure;	// tree index

class BinaryTree {
	private:
		BinaryTreeStructure tree_nlist;
		int tree_size;
		int root;
		int height;
		
	public:
		BinaryTree();
		BinaryTree(int);
		BinaryTree(BinaryTreeStructure&);
		~BinaryTree();
		void writeTree(ostream&);
		void printTree();
		void addNode(size_t);
		int num_nodes();
		int num_edges();
		BinaryTree& operator=(const BinaryTree&);
		treenode& operator[](const int);
		int& access_root();
		int& access_height();
		void clear();
};	

// get a specified vertex property
inline
treenode& BinaryTree::operator[](const int vid) {
	return tree_nlist[vid];
}

inline
int BinaryTree::num_nodes() {
	return tree_nlist.size();
}

inline
void BinaryTree::printTree() {
	writeTree(cout);
}

inline
void BinaryTree::clear() {
	tree_size = 0;
	tree_nlist.clear();
	root=-1;
	height=-1;
}

inline
int& BinaryTree::access_root(){
	return root;
}

inline
int& BinaryTree::access_height(){
	return height;
}
#endif
