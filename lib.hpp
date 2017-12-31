#include <iostream>
#include <string>
#include <sstream>
#include <set>

#include <iterator>
using namespace std;

/* --------- GRAPH ---------*/
// vertex representation that has a pointer to the next vertex
struct vertex_t {
  int               name    ; //Key of the vertex
  int               nontreeEdge;
  int               level   ;
  struct  vertex_t* next    ; //Pointer to its connections
} ;

struct adjList_t {
  int size;
  struct vertex_t*  vertex ; //Pointer to the head of the head vertex
} ;

// graph representation
struct graph_t {
  int                  size     ; //Number of vertices in graph
  struct adjList_t*    graphArr ; //List of all vertices
  struct structTree_t* tree     ; //The structural tree for the graph
} ;

/* --------- STRUCTURAL TREE ---------*/
// structural tree representation
// every node has a parent
struct node_t {
  struct localNode_t*          localRoot ; //Points to its local root
  struct localTree_t*          localTree ; //Localtree of node a, where a is the local root
  struct node_t*               parent    ; //Points to the nodes parent
  struct node_t*               root      ; //Points to the root of the tree
  struct node_t*               children  ; //Points to the first child that points to its siblings
  struct node_t*               sibling   ; //Points to the next sibling
  struct node_t*               last      ; //Points to the last sibling
  struct adjTreeList_t*        cluster   ; //List with pointers to descending leaves
  int                          size      ; //num of children
  int                          n         ; //Number of leaves descending from node
  int                          rank      ; //Rank of node is log2(n)
  int                          level     ; //level of the node
  int                          leaf      ; //1 if the node is a leaf in structTree
  int                          height    ; //Height of the subtree rooted in node
  int                          name      ; //Key of the node
} ;

// adj list for nodes
struct adjTreeList_t {
  int             size    ; //Size of cluster
  struct node_t*  nodes   ; //Pointer to the node in structural tree
} ;

// the structural tree representation
struct structTree_t {
  int                   size     ; //Size of the structural tree
  struct adjTreeList_t* list     ; //List of nodes in structural tree
} ;

/* --------- LOCAL TREE ---------*/
// binary local tree representation
struct localNode_t {
  int                   name      ;
  char                   tree      ;
  char                   nonTree   ;
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

/* ------------------ FUNCTIONS ------------------*/
/* --------- GRAPH ---------*/
// building the graph
struct graph_t* makeGraph(int size);
// building adj list for connections
struct vertex_t* makeAdjList(int v);
void addEdge(graph_t graph, int u, int v);
// delete an edge and update
void deleteEdge(graph_t* graph, vertex_t v, vertex_t u);

void searchEdge(graph_t* graph, vertex_t* vertex, int u, int v);

/* --------- STRUCTURAL TREE ---------*/
struct node_t* newNode(int name);

struct node_t* newClusterNode(node_t* name);

// bulding structural tree
struct structTree_t* initStructTree(graph_t* graph);

//update level and root for structural tree
void recurseLevel(node_t* root, node_t* currentRoot, int level);

void mergeNodes(node_t* a, node_t* b);

void addTree(graph_t* graph, int u, int v);

void delTree(graph_t* graph, int u, int v);

node_t* smallest(node_t* u, node_t* v);

int DFSsmallestTree(node_t* currentRoot, int count);

// replace
void replace(graph_t graph);

void Clusters(struct node_t* node);

int isConnected(graph_t* graph, int u, int v);

int search(adjTreeList_t* cluster, int elem);

void updateSiblings();
void updateClusters();
void noReplacementUpdate(node_t* tv, node_t* firstCommonNode,
  graph_t* graph, int currentLevel);

int findReplacement(graph_t* graph, node_t* tv, node_t* tw);
void updateNonTree(int u, int v, graph_t* graph);

/* --------- LOCAL TREE ---------*/
struct node_t* findMinLevelNode(graph_t* graph, int u, int v);

// initializes local tree


struct localTree_t* initLocalTree(node_t* node);

struct localNode_t* newLocalNode(int name);

struct localTree_t* makeLT(node_t* localRoot);

// merging nodes in LT
void mergeLT(node_t* u, node_t* v);

// search for connection

void updateLT(localTree_t* tree, node_t* node);

void pairNodes(localTree_t* tree, node_t* node, localNode_t* arr[]);
