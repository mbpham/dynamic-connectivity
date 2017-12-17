#include <iostream>
#include "lib.cpp"
using namespace std;

int main(int argc, char** argv)
  {
    graph_t* a;
    a = makeGraph(5);
    addEdge(a,1,2);
    addEdge(a,3,4);
    addEdge(a,2,3);
    addEdge(a,4,3);
    //prettyPrinting(a);
    int i;

  }
