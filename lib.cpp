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
  printf("\nInserting edge (%d, %d), roots are %d and %d\n", u, v, graph->tree->list[u].nodes->root->name, graph->tree->list[v].nodes->root->name);

  /*--------- CASES FOR STRUCTURAL FOREST ---------*/
  //first check if they share the same root in structural forest
  if(graph->tree->list[u].nodes->root == graph->tree->list[v].nodes->root){
    //check if they are directly connected
    printf("addEdge: see if %d and %d are directly connected\n", u, v);
    if(isConnected(graph, u, v)){
      printf("addEdge: The vertices are already connected\n");
    }
    else{
      printf("addEdge: Not directly connected. Add as non-tree edge\n");
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
    printf("addEdge: not connected. Merge roots.\n");
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
  printf("isConnected: size of %d is %d\n", u, graph->graphArr[u].size);
  printf("isConnected: size of %d is %d\n", v, graph->graphArr[v].size);
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
    //TODO: fix
    removedEdge_t* remu = (struct removedEdge_t*) malloc(sizeof(removedEdge_t));
    removedEdge_t* remv = (struct removedEdge_t*) malloc(sizeof(removedEdge_t));

    searchEdge(graph, remu, headu, u, v);
    //deleting u from v adj list
    searchEdge(graph, remv, headv, v, u);

    printf("Deleting the level %d edge (%d,%d), nontreeEdge: %d\n", remu->level, u, v, remu->nontreeEdge);

    /* --------- STRUCTURAL FOREST ---------*/

    /* CASES: TREE OR NON TREE EDGE REMOVED */
    if(remu->nontreeEdge){
      //TODO: make updates for nontree bitmaps
      //count non tree edges
      if(!countNonTreeEdges(graph, u, remu->level)){
        printf("deleteEdge: There are no more non-tree edges for %d level %d. Update non-tree bitmap\n", u, remu->level);
        nonTree(graph->tree->list[u].nodes->localTree->root, remu->level,0);
        updateNonBitmaps(graph->tree, u, 0);
      }
      if(!countNonTreeEdges(graph, v, remv->level)){
        printf("deleteEdge: There are no more non-tree edges for %d level %d. Update non-tree bitmap\n", v, remv->level);
        nonTree(graph->tree->list[v].nodes->localTree->root, remv->level,0);
        updateNonBitmaps(graph->tree, v, 0);
      }


    }
    else{
      //TODO: Implement delTree
      delTree(graph, u, v, remu->level);
    }

    //update the number of connections
    graph->graphArr[u].size--;
    graph->graphArr[v].size--;


  }
  else{
    printf("%d and %d are not connected. The job is done.\n", u, v);
  }

}

/* --------- SEARCHES --------- */
//searches for vertices u and v in graph adj list and move pointers st they are not connected
//returns the level of the removed edge
void searchEdge(graph_t* graph, removedEdge_t* rem, vertex_t* vertex, int u, int v){
  if(vertex->name == v){
    graph->graphArr[u].vertex = vertex->next;
    rem->level = vertex->level;
    rem->nontreeEdge = vertex->nontreeEdge;
  }
  vertex_t* prev = vertex;
  vertex = vertex->next;
  while(vertex != NULL){
    if(vertex->name == v){
      prev->next = vertex->next;
      int size = graph->graphArr[u].size;
      rem->level = vertex->level;
      graph->graphArr[u].size = size;
      rem->nontreeEdge = vertex->nontreeEdge;
      return;
    }
    prev = vertex;
    vertex = vertex->next;
  }
  return;
}

// checks if two vertices are directly connected
int isConnected(graph_t* graph, int u, int v){
  int i;
  int minSize = min(graph->graphArr[u].size, graph->graphArr[v].size);

  vertex_t* checkU = graph->graphArr[u].vertex;
  vertex_t* checkV = graph->graphArr[v].vertex;
  printf("isConnected: size of %d is %d\n", u, graph->graphArr[u].size);
  printf("isConnected: size of %d is %d\n", v, graph->graphArr[v].size);

  for(i = 0; i<minSize; i++){
    if(checkU->name == v || checkV->name == u){
      return 1;
    }
    checkU = checkU->next;
    checkV = checkV->next;
  }
  return 0;
}

int countNonTreeEdges(graph_t* graph, int vertexIndex, int level){
  vertex_t* nexts = graph->graphArr[vertexIndex].vertex;
  while(nexts){
    if((nexts->nontreeEdge == 1)||(nexts->level == 1)){
      return 1;
    }
    nexts = nexts->next;
  }
  return 0;
}


int treeConnected(structTree_t* tree, int u, int v){
  printf("\n----------------------------------------------------- \n");
  node_t* rootu = tree->list[u].nodes->root;
  node_t* rootv = tree->list[v].nodes->root;
  if(rootv == rootu){
    printf("treeConnected: %d and %d are connected\n", u, v);
    return 1;
  }
  printf("treeConnected: %d and %d are not connected\n", u, v);
  return 0;
}

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
