#include <iostream>
#include <string>
#include <sstream>
#include "stlib.cpp"
#include <math.h>
using namespace std;

/* --------- GRAPH INITIALIZING AND HELPER FUNCTIONS ---------*/
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

/* --------- UPDATES AFTER INSERTION OR DELETION --------- */
// add edge, make connection between two vertex'
void addEdge(graph_t* graph, int u, int v){
  printf("\n---------------------------------------------\n");
  printf("\n---------------------------------------------\n");
  printf("\nInserting edge (%d, %d)\n", u, v);

  /*--------- CASES FOR STRUCTURAL FOREST ---------*/
  //first check if they share the same root in structural forest
  if(graph->tree->list[u].nodes->root == graph->tree->list[v].nodes->root){
    //check if they are directly connected
    if(isConnected(graph, u, v)){
      printf("addEdge: The vertices are already connected\n");
    }
    else{
      //Add a non tree edge
      //finding first common node
      int level = findUpdateLevel(graph->tree, u, v);
      printf("addEdge: Update nontree edge at level %d\n", level);

      //update nontree bitmap for u and v at level
      nonTree(graph->tree->list[u].nodes->localTree->root, level, 1);
      nonTree(graph->tree->list[v].nodes->localTree->root, level, 1);

      updateNonBitmaps(graph->tree, u, 0);
      updateNonBitmaps(graph->tree, v, 0);

      /* --------- ADD TO THE GRAPH ARRAY ---------*/
      // connection from u to v
      // making a pointer to a new vertex
      // makes v point to u
      struct vertex_t* newVertex = makeAdjList(v);
      newVertex->next = graph->graphArr[u].vertex;
      newVertex->level = level;
      newVertex->nontreeEdge = 1;
      newVertex->structNode = graph->tree->list[v].nodes;
      graph->tree->list[u].nodes->to = newVertex;
      graph->graphArr[u].vertex = newVertex;


      // connection from v to u
      newVertex = makeAdjList(u);
      newVertex->next = graph->graphArr[v].vertex;
      newVertex->level = level;
      newVertex->nontreeEdge = 1;
      newVertex->structNode = graph->tree->list[u].nodes;
      graph->tree->list[v].nodes->to = newVertex;
      graph->graphArr[v].vertex = newVertex;

      // updates number of connections
      graph->graphArr[u].size++;
      graph->graphArr[v].size++;


    }
  }
  //if they are not connected, connect and update structural forest
  else{
    // Updates structural tree
    addTree(graph, u, v);

    /* --------- ADD TO THE GRAPH ARRAY ---------*/
    // connection from u to v
    // making a pointer to a new vertex
    // makes v point to u
    struct vertex_t* newVertex = makeAdjList(v);
    newVertex->next = graph->graphArr[u].vertex;
    newVertex->level = 0;
    newVertex->nontreeEdge = 0;
    newVertex->structNode = graph->tree->list[v].nodes;
    graph->tree->list[u].nodes->to = newVertex;
    graph->graphArr[u].vertex = newVertex;

    // connection from v to u
    newVertex = makeAdjList(u);
    newVertex->next = graph->graphArr[v].vertex;
    newVertex->level = 0;
    newVertex->nontreeEdge = 0;
    newVertex->structNode = graph->tree->list[u].nodes;
    graph->tree->list[v].nodes->to = newVertex;
    graph->graphArr[v].vertex = newVertex;

    // updates number of connections
    graph->graphArr[u].size++;
    graph->graphArr[v].size++;
  }
}

// delete an edge
void deleteEdge(graph_t* graph, int u, int v){
  printf("\n---------------------------------------------\n");
  printf("\n---------------------------------------------\n");

  // checks if they are connected
  if(isConnected(graph, u, v)){
    /* --------- DELETE FROM GRAPH ---------*/
    printf("deleteEdge: %d and %d are connected.\n", u, v);
    vertex_t* headu = graph->graphArr[u].vertex;
    vertex_t* headv = graph->graphArr[v].vertex;


    //deleting v from u adj list
    removedEdge_t* rem = searchEdge(graph, headu, u, v);
    //deleting u from v adj list
    searchEdge(graph, headv, v, u);

    printf("Deleting the level %d edge (%d,%d), nontreeEdge: %d\n", rem->level, u, v, rem->nontreeEdge);

    /* --------- STRUCTURAL FOREST ---------*/

    /* CASES: TREE OR NON TREE EDGE REMOVED */
    if(rem->nontreeEdge){
      //TODO: make updated for nontree bitmaps
    }
    else{
      //TODO: Implement delTree

      delTree(graph, u, v, rem->level);
    }

    //update the number of connections
    graph->graphArr[u].size--;
    graph->graphArr[v].size--;





    //update structural forest
    //delTree(graph, u, v);



  }
  else{
    printf("%d and %d are not connected. The job is done.\n", u, v);
  }
}

/* --------- SEARCHES --------- */
//searches for vertices u and v in graph adj list and move pointers st they are not connected
//returns the level of the removed edge
struct removedEdge_t* searchEdge(graph_t* graph, vertex_t* vertex, int u, int v){
  removedEdge_t* rem;
  if(vertex->name == v){
    graph->graphArr[u].vertex = vertex->next;
    rem->level = vertex->level;
    rem->nontreeEdge = vertex->nontreeEdge;
  }

  vertex_t* prev = vertex;
  vertex = vertex->next;
  while(vertex != NULL){
    //printf("deleteEdge: Head is %d\n", vertex->name);
    if(vertex->name == v){
      //printf("Deleting from %d: %d\n", u, vertex->name);
      prev->next = vertex->next;

      rem->level = vertex->level;
      rem->nontreeEdge = vertex->nontreeEdge;
      return rem;
    }
    prev = vertex;
    vertex = vertex->next;
  }
  return rem;
}

// checks if two vertices are directly connected
int isConnected(graph_t* graph, int u, int v){
  int i;
  int minSize = min(graph->graphArr[u].size, graph->graphArr[v].size);

  vertex_t* checkU = graph->graphArr[u].vertex;
  vertex_t* checkV = graph->graphArr[v].vertex;
  for(i = 0; i<minSize; i++){
    if(checkU->name == v || checkV->name == u){
      return 1;
    }
    checkU = checkU->next;
    checkV = checkV->next;
  }
  return 0;
}

/* --------- UPDATES --------- */


/* --------- PRINTS ---------*/
// Printing vertex connections
void prettyPrinting(graph_t* graph) {
  printf("\n---------------------------------------------\n");
  printf("\n---------------------------------------------\n");
  int i;
  int size = graph->size;
  for (i = 0; i<size; i++){
    struct vertex_t* nexts = graph->graphArr[i].vertex;
    printf("\nConnections to %d\n", i);
    printf("head ");
    while(nexts){
      printf("-> %d(NonTree: %d)(Level: %d)", nexts->name, nexts->nontreeEdge, nexts->level);
      nexts = nexts->next;
    }
  }
}
