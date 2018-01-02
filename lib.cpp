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
    graph->graphArr[i].size = 0;
  }
  structTree_t* a;
  graph->tree = initStructTree(graph);
  return graph;
}

// add edge, make connection between two vertex'
void addEdge(graph_t* graph, int u, int v){
  printf("\n---------------------------------------------\n");
  printf("\nInserting edge (%d, %d)\n", u, v);

  //first check if they share the same root in structural forest
  if(graph->tree->list[u].nodes->root == graph->tree->list[v].nodes->root){
    //check if they are directly connected
    if(isConnected(graph, u, v)){
      //do nothing
      printf("The vertices are already connected\n");
    }
    else{
      printf("Update nontree edge\n");
      //TODO: add nontree edge at level of their first common node
      
    }
  }
  //if they are not connected, connect and update structural forest
  else{
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

    // updates number of connections
    graph->graphArr[u].size++;
    graph->graphArr[v].size++;

    // Updates structural tree
    addTree(graph, u, v);
  }
}

// delete an edge
void deleteEdge(graph_t* graph, int u, int v){
  printf("\n---------------------------------------------\n");

  // checks if they are connected
  if(isConnected(graph, u, v)){
    printf("%d and %d are connected. Updating graph.\n", u, v);
    vertex_t* headu = graph->graphArr[u].vertex;
    vertex_t* headv = graph->graphArr[v].vertex;

    printf("Deleting edge (%d,%d)\n", u, v);
    //deleting v from u adj list
    searchEdge(graph, headu, u, v);
    //deleting u from v adj list
    searchEdge(graph, headv, v, u);

    //update structural forest
    //delTree(graph, u, v);

    //update the number of connections
    graph->graphArr[u].size--;
    graph->graphArr[v].size--;
  }
  else{
    printf("%d and %d are not connected. The job is done.\n", u, v);
  }
}

//searches for vertices u and v in graph adj list and move pointers st they are not connected
void searchEdge(graph_t* graph, vertex_t* vertex, int u, int v){
  if(vertex->name == v){
    graph->graphArr[u].vertex = vertex->next;
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

// checks if two vertices are directly connected
int isConnected(graph_t* graph, int u, int v){
  int i;
  int minSize = min(graph->graphArr[u].size, graph->graphArr[v].size);

  vertex_t* checkU = graph->graphArr[u].vertex;
  vertex_t* checkV = graph->graphArr[v].vertex;
  printf("Size: %d\n", minSize);
  for(i = 0; i<minSize; i++){
    if(checkU->name == v || checkV->name == u){
      return 1;
    }
    checkU = checkU->next;
    checkV = checkV->next;
  }
  return 0;
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
