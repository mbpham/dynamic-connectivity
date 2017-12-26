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
  int                   name      ;
  struct node_t*        parent    ;
  int                   level     ;
  int                   leaf      ;
  struct node_t*        root      ;
  struct localTree_t*   localTree ;
  int                   height    ;
  struct node_t*        children  ;
  int                   size      ; //num of children
  int                   n         ;
  int                   rank      ;
  struct node_t*        sibling   ;
  struct node_t*        last      ;
  struct localNode_t*   localRoot ;
} ;

// adj list for nodes
struct adjTreeList_t {
  struct node_t*  nodes   ;
} ;

// the structural tree representation
struct structTree_t {
  int                   size     ;
  struct adjTreeList_t* list     ;
} ;

/* --------- LOCAL TREE ---------*/
// binary local tree representation

struct localNode_t {
  int                   name      ;
  int                   tree      ;
  int                   nonTree   ;
  int                   pNode     ;
  struct localNode_t*   left      ;
  struct localNode_t*   right     ;
  struct localNode_t*   parent    ;
  int                   size      ; //defines the number of nodes
  int                   rank      ;
  struct localNode_t*   root      ;
  int                   newNode   ;

} ;

struct adjLTList_t {
  struct localNode_t*   node  ;
} ;

struct localTree_t {
  int                   size  ; //defines the amount of nodes in total without path nodes
  struct adjLTList_t*   list  ;
  int                   build ;
  struct localNode_t*   root  ;
  int                   roots ; //defines the number og nodes that are directly connected to the rank path
  int                   pNodes;
} ;


/* --------- FUNCTIONS ---------*/
// building adj list for connections
struct vertex_t* makeAdjList(int v);

//update level and root for structural tree
void recurseLevel(node_t* root, node_t* currentRoot, int level);

struct node_t* newNode(int name);

// building the graph
struct graph_t* makeGraph(int size);

// bulding structural tree
struct structTree_t* initStructTree(graph_t* graph);

// initializes local tree
struct localTree_t* initLocalTree(node_t* node);

struct localNode_t* newLocalNode(int name);

//updates children for root
void updateChildren(graph_t* graph, node_t* currentRoot, int u, int v, int Case);

// building local tree
//struct localTree_t* makeLT(node_t* localRoot);
void mergeNodes(node_t* a, node_t* b);

void addEdge(graph_t graph, int u, int v);

void updateTree(graph_t* graph, int u, int v);

// delete an edge and update
void deleteEdge(graph_t* graph, vertex_t v, vertex_t u);

// replace
void replace(graph_t graph);

// merging nodes in LT
void mergeLT(node_t* u, node_t* v);

// search for connection
void search();
void Clusters(struct node_t* node);

int isConnected(graph_t* graph, int u, int v);

void updateLT(localTree_t* tree, node_t* node);

struct localTree_t* makeLT(node_t* localRoot);

void pairNodes(localTree_t* tree, node_t* node, localNode_t* arr[]);
