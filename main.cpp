#include <iostream>
#include "lib.cpp"
using namespace std;

int main(int argc, char** argv)
  {
    graph_t* a;
    a = makeGraph(5);
    addEdge(a,1,2);
    addEdge(a,0,1);
    addEdge(a,4,2);
    //addEdge(a,4,3);
    prettyPrinting(a);
    int i;

  }
