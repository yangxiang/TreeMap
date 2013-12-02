LDFLAGS	=-Wall -O3
CPPFLAGS=-Wall -Wno-deprecated -O3

TreeMapF: bin/TreeMapF

bin/TreeMapF: .obj/GenericTree.o .obj/BinaryTree.o .obj/BitOperators.o .obj/FlatVector.o .obj/Graph.o .obj/GraphUtil.o .obj/main.o
	g++ $(LDFLAGS) $^ -o bin/TreeMapF
	
TreeMapC: bin/TreeMapC

bin/TreeMapC: .obj/GenericTree.o .obj/BinaryTree.o .obj/BitOperators.o .obj/FlatVector.o .obj/Graph.o .obj/GraphUtil.o .obj/TreeMapC.o
	g++ $(LDFLAGS) $^ -o bin/TreeMapC	
	
BFSQuery: bin/BFSQuery

bin/BFSQuery: .obj/Graph.o .obj/GraphUtil.o .obj/BFSQuery.o
	g++ $(LDFLAGS) $^ -o bin/BFSQuery
	
BiBFSQuery: bin/BiBFSQuery

bin/BiBFSQuery: .obj/Graph.o .obj/GraphUtil.o .obj/BiBFSQuery.o
	g++ $(LDFLAGS) $^ -o bin/BiBFSQuery
	
.obj/%.o: src/%.cpp
	g++ $(CPPFLAGS) $< -c -o $@
	
clean:
	rm -f .obj/*.o 
