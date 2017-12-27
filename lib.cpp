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
  printf("\n---------------------------------------------\n");
  printf("\nInserting edge (%d, %d)\n", u, v);
  //CHANGE: if the two vertices are directly connected, then skip this step
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
  addTree(graph, u, v);
}

// delete an edge
void deleteEdge(graph_t* graph, int u, int v){
  printf("\n---------------------------------------------\n");
  vertex_t* headu = graph->graphArr[u].vertex;
  vertex_t* headv = graph->graphArr[v].vertex;
  //deleting v from u adj list
  printf("Deleting edge (%d,%d)\n", u, v);
  searchEdge(graph, headu, u, v);
  //deleting u from v adj list
  searchEdge(graph, headv, v, u);
  delTree(graph, u, v);

}

void searchEdge(graph_t* graph, vertex_t* vertex, int u, int v){
  if(vertex->name == v){
    graph->graphArr[u].vertex = vertex->next;
    //printf("First vertex in %d list: %d\n", u, vertex->name);
  }
  vertex_t* prev = vertex;
  vertex = vertex->next;
  while(vertex != NULL){
    //printf("deleteEdge: Head is %d\n", vertex->name);
    if(vertex->name == v){
      //printf("Deleting from %d: %d\n", u, vertex->name);
      prev->next = vertex->next;
      break;
    }
    prev = vertex;
    vertex = vertex->next;
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
