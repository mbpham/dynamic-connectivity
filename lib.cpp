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

  //printing roots of all trees
  for(i = 0;i<size;i++){
  printf("%d\n", graph->tree->list[i].nodes->root->name);
  }
  return graph;
}

// add edge, make connection between two vertex'
void addEdge(graph_t* graph, int u, int v){
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
}

// delete an edge
void deleteEdge(graph_t graph, int v, int u){

}


/* --------- LOCAL TREE ---------*/
// building local tree
void makeLocalTree(){

};


// Insert
void insert(int u, int v){
  int connected = isConnected(u, v);
  if (not connected){
  //   Insert the edge (u,v)
  }
};

// Delete

// replace
void replace(graph_t graph){

};

// merging nodes when running edge delete
void merge(){

};

// search for connection
void search(){

};

int isConnected(int u, int v) {
  return 0;
};


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

// Printing cluster connections