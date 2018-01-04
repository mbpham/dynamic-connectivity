#include <iostream>
#include "lib.cpp"
using namespace std;


int main(int argc, char** argv){

    graph_t* a;
    a = makeGraph(10);
    addEdge(a,1,2);
    addEdge(a,3,4);
    addEdge(a,1,3);
    deleteEdge(a,3,1);
    //deleteEdge(a,8,1);
    //deleteEdge(a,1,5);
    prettyPrinting(a);

    //addEdge(a,4,3);
    //addEdge(a,1,4);
    //addEdge(a,4,7);
  }
