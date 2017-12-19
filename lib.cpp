#include <iostream>
#include <string>
#include <sstream>
#include "stlib.cpp"
#include <math.h>
using namespace std;

/* --------- GRAPH ---------*/
// building adj list
struct vertex_t* makeAdjList(int v){
  //make pointer to a vertex w and allocate memory
  struct vertex_t* w = (struct vertex_t*) malloc(sizeof(struct vertex_t));
  w->next = NULL ;
  w->name = v;
  return w;
}

// building a graph by making adj list for each vertex
struct graph_t* makeGraph(int size){
  // make a new graph and allocate memory
  struct graph_t* graph = (struct graph_t*) malloc(sizeof(struct graph_t));
  // giving number of vertices to the graph
  graph->size = size;
  // allocate memory for adj list in array
  graph->graphArr = (struct adjList_t*) malloc(size * sizeof(struct adjList_t));
  // adding init adj list to the array
  int i;
  for(i = 0; i<size; i++){
    graph->graphArr[i].vertex = NULL;
  }
  structTree_t* a;
  graph->tree = initStructTree(graph);
  return graph;
}

// add edge, make connection between two vertex'
void addEdge(graph_t* graph, int u, int v){
  /* CHANGE: check if there is an direct connection and
             not just any connection */
  printf("\nInserting edge (%d, %d)\n", u, v);         
  if(1) {
    if(isConnected(graph, u, v) == 0){
      int i;
      // connection from u to v
      // making a pointer to a new vertex
      struct vertex_t* newVertex = makeAdjList(v);
      // makes v point to u
      newVertex->next = graph->graphArr[u].vertex;
      // makes the new vertex the head of the adj. list
      graph->graphArr[u].vertex = newVertex;

      // connection from v to u
      newVertex = makeAdjList(u);
      newVertex->next = graph->graphArr[v].vertex;
      graph->graphArr[v].vertex = newVertex;

      //Updates structural tree
      updateTree(graph, u, v);

      //update tree bitmap at level i
    }
    else{
      //update nontree bitmap at level i

    }
  }

}

// delete an edge
void deleteEdge(graph_t* graph, int v, int u){
  if(isConnected(graph, u, v)){
    vertex_t* nexts = graph->graphArr[u].vertex;
    printf("1\n");
    while(nexts) {
      printf("2\n");
      if(nexts->name == v){
        printf("4\n");
        if((nexts->next) == NULL){
          printf("5\n");
          nexts = NULL;
        }
        else{
          printf("3\n");
          nexts->next = nexts->next->next;
        }

      }
      nexts = nexts->next;
    }

    graph->graphArr[u].vertex->next = graph->graphArr[u].vertex->next->next;
    //graph->graphArr[v].vertex->next = graph->graphArr[v].vertex->next->next;
  }
}


/* --------- PRINTS ---------*/
// Printing vertex connections
void prettyPrinting(graph_t* graph) {
  int i;
  int size = graph->size;
  for (i = 0; i<size; i++){
    struct vertex_t* nexts = graph->graphArr[i].vertex;
    printf("\nConnections to %d\n", i);
    printf("head ");
    while(nexts){
      printf("-> %d", nexts->name);
      nexts = nexts->next;
    }
  }
}
