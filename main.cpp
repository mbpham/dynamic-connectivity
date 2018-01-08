#include <iostream>
#include "lib.cpp"
using namespace std;


int main(int argc, char** argv){
    graph_t* a;
    a = makeGraph(10);
    addEdge(a,1,2);
    addEdge(a,3,4);
    addEdge(a,1,3);
    addEdge(a,2,4);
    deleteEdge(a,3,1);
    addEdge(a,1,5);
    addEdge(a,4,5);
    deleteEdge(a,4,5);
    addEdge(a,4,5);
    addEdge(a,7,8);
    addEdge(a,7,3);
    deleteEdge(a,7,8);
    addEdge(a,3,5);
    addEdge(a,9,8);
    addEdge(a,1,3);
    deleteEdge(a,4,3);
    addEdge(a,2,5);
    addEdge(a,1,4);
    addEdge(a,4,7);
    addEdge(a,3,8);
    addEdge(a,2,5);
    addEdge(a,5,4);
    addEdge(a,3,7);
    addEdge(a,0,8);
    addEdge(a,6,0);
    prettyPrinting(a);
    treeConnected(a->tree,1,4);
    treeConnected(a->tree,3,4);
    treeConnected(a->tree,9,6);
  }
