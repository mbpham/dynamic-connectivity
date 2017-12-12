#include <iostream>
#include <string>
#include <sstream>
using namespace std;

/* --------- GRAPH ---------*/
// vertex representation that has a pointer to the next vertex
struct vertex_t {
  int               name  ;
  char              label ;
  struct  vertex_t* next  ;
} ;

// adj list for where list.vertex is the vertex that has connections to the others
// points to the head of the head vertex
struct adjList_t {
  struct vertex_t*  vertex ;
} ;

// graph representation
// size is number of vertex' in the graph
struct graph_t {
  int                  size     ;
  struct adjList_t*    graphArr ;
  struct structTree_t* tree     ;
} ;

/* --------- STRUCTURAL TREE ---------*/
// structural tree representation
// every node has a parent
struct node_t {
  int                  name    ;
  char                 key     ;
  struct node_t*       parent  ;
  int                  level   ;
  int                  leaf    ;
  struct node_t*       root    ;
  struct localTree_t*  cluster ;
} ;

// adj list for nodes
struct adjTreeList_t {
  struct node_t*  nodes   ;
} ;

struct node* root = 0;



// the structural tree representation
struct structTree_t {
  int                   size     ;
  struct adjTreeList_t* list     ;
} ;




/* --------- LOCAL TREE ---------*/
// binary local tree representation
struct localTree_t {

} ;

/* --------- FUNCTIONS ---------*/
// building adj list for connections
struct vertex_t* makeAdjList(int v);

struct node_t* newNode(int name);

// building the graph
struct graph_t* makeGraph(int size);

// bulding structural tree
struct structTree_t* initStructTree(graph_t* graph);

// building local tree
void makeLocalTree(structTree_t structTree);

void addEdge(graph_t graph, int u, int v);

void updateTree(graph_t* graph, int u, int v);

// delete an edge and update
void deleteEdge(graph_t graph, vertex_t v, vertex_t u);

// replace
void replace(graph_t graph);

// merging vertex'
void merge();

// search for connection
void search();

int isConnected(int u, int v);
