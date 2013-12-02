#include "BinaryTree.h"

BinaryTree::BinaryTree() {
	tree_nlist = BinaryTreeStructure();
	root=-1;
	height=-1;
}

BinaryTree::BinaryTree(int size) {
	tree_size = size;
	tree_nlist = BinaryTreeStructure(size);
	root=-1;
	height=-1;
}

BinaryTree::BinaryTree(BinaryTreeStructure& t) {
	tree_nlist = t;
	tree_size = tree_nlist.size();
	root=-1;
	height=-1;
}

BinaryTree::~BinaryTree() {}

void BinaryTree::writeTree(ostream& out) {
	cout << "Tree size = " << tree_nlist.size() << endl;
	cout << "Tree height = "<<height<<endl;
	out << "Tree Print out: " << endl;

	BinaryTreeStructure::iterator git;
	size_t i = 0;
	for (i = 0; i < tree_nlist.size(); i++) {
		out << i << ": ";
		out << "parent: " <<tree_nlist[i].parent<<"; left_child: "<<tree_nlist[i].left_child<<"; right_child: "<<tree_nlist[i].right_child;
		out << "#" << endl;
	}
}

void BinaryTree::addNode(size_t vid) {
	if (vid >= tree_nlist.size()) {
		size_t size = tree_nlist.size();
		for (size_t i = 0; i < (vid-size+1); i++) {
			tree_nlist.push_back(treenode());
		}
		tree_size = tree_nlist.size();
	}
	
}

int BinaryTree::num_edges() {
	BinaryTreeStructure::iterator git;
	int num = 0;
	for (git = tree_nlist.begin(); git != tree_nlist.end(); git++) {
		if (git->parent>= 0)
			num++;
		if (git->left_child>=0)
			num++;
		if (git->right_child>=0)
			num++;
	}
	return num/2;//each edge is calculated twice
}

BinaryTree& BinaryTree::operator=(const BinaryTree& t) {
	if (this != &t) {
		tree_nlist = t.tree_nlist;
		tree_size = t.tree_size;
		root=t.root;
		height=t.height;
	}
	return *this;
}


