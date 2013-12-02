#include "GenericTree.h"

GenericTree::GenericTree() {
	tree_nlist = GenericTreeStructure();
	nl = NodeInfoList();
}

GenericTree::GenericTree(int size) {
	tree_size = size;
	tree_nlist = GenericTreeStructure(size);
	nl = NodeInfoList(size);
}

GenericTree::GenericTree(GenericTreeStructure& t, NodeInfoList& nlist) {
	tree_nlist = t;
	nl=nlist;
	tree_size = tree_nlist.size();
}

GenericTree::~GenericTree() {}

void GenericTree::printTree() {
	writeTree(cout);
}

void GenericTree::clear() {
	tree_size = 0;
	tree_nlist.clear();
	nl.clear();
}

void GenericTree::writeTree(ostream& out) {
	cout << "Tree size = " << tree_nlist.size() << endl;
	out << "Tree Print out: " << endl;

	GenericTreeStructure::iterator git;
	TreeEdgeList el;
	TreeEdgeList::iterator eit;
	size_t i = 0;
	for (i = 0; i < tree_nlist.size(); i++) {
		out << i << " (level: "<<nl[i].nodelevel<<") : ";
		el = tree_nlist[i];
		for (eit = el.begin(); eit != el.end(); eit++)
			out << eit->nodeID << " (level: "<<nl[eit->nodeID].nodelevel<<"), ";
		out << "#" << endl;
	}
}

void GenericTree::addNode(size_t vid) {
	if (vid >= tree_nlist.size()) {
		size_t size = tree_nlist.size();
		for (size_t i = 0; i < (vid-size+1); i++) {
			tree_nlist.push_back(TreeEdgeList());
			nl.push_back(treenodeinfo());
		}
		tree_size = tree_nlist.size();
	}
}

void GenericTree::addEdge(size_t sid, size_t tid) {//only add one edge. To add a pair of edges, call it twice.
	if (sid >= tree_nlist.size())
		addNode(sid);
	if (tid >= tree_nlist.size())
		addNode(tid);
	// update edge list
	neighbor Node_to_add;
	Node_to_add.nodeID=tid;
	tree_nlist[sid].push_back(Node_to_add);
}

int GenericTree::num_edges() {
	TreeEdgeList el;
	GenericTreeStructure::iterator git;
	int num = 0;
	for (git = tree_nlist.begin(); git != tree_nlist.end(); git++) {
		el = *git;
		num += el.size();
	}
	return num/2;//each edge is calculated twice
}


// check whether the edge from src to trg is in the graph
bool GenericTree::hasEdge(int src, int trg) {
	TreeEdgeList el = tree_nlist[src];
	TreeEdgeList::iterator ei;
	for (ei = el.begin(); ei != el.end(); ei++)
		if (ei->nodeID == trg)
			return true;
	return false;

}


GenericTree& GenericTree::operator=(const GenericTree& t) {
	if (this != &t) {
		tree_nlist = t.tree_nlist;
		nl=t.nl;
		tree_size = t.tree_size;
	}
	return *this;
}

