#include <iostream>
#include "lib.cpp"
using namespace std;


int main(int argc, char** argv){
    //std::set<int> first;
    graph_t* a;
    a = makeGraph(10);
    addEdge(a,1,2);
    printf("%d\n", a->tree->list[2].nodes->parent->name);
    addEdge(a,1,5);
    addEdge(a,1,3);
    addEdge(a,3,4);
    addEdge(a,3,2);

    deleteEdge(a,2,1);
    deleteEdge(a,1,5);
    prettyPrinting(a);
    //addEdge(a,4,3);
    //addEdge(a,1,4);
    //addEdge(a,4,7);
  }
