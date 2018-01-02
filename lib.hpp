#include <iostream>
#include <string>
#include <sstream>
#include <set>

#include <iterator>
using namespace std;
/* --------- BITMAP ---------*/
#define BITMAP_LEN 2
#define BYTE_LEN   8

/* --------- GRAPH ---------*/
// vertex representation that has a pointer to the next vertex
struct vertex_t {
  int                   name        ; //Key of the vertex
  int                   nontreeEdge ; //1 the connection is a nontreeEdge, 0 otherwise
  int                   level       ; //tell what level the edge has
  struct  vertex_t*     next        ; //Pointer to its connections
} ;

struct adjList_t {
  int size;
  struct vertex_t*      vertex      ; //Pointer to the head of the head vertex
} ;

// graph representation
struct graph_t {
  int                   size        ; //Number of vertices in graph
  struct adjList_t*     graphArr    ; //List of all vertices
  struct structTree_t*  tree        ; //The structural tree for the graph
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
  struct vertex_t*             to          ; //1 elem cluster (leaves) points to the vertices, to later see connections
} ;

// adj list for nodes
struct adjTreeList_t {
  int                   size        ; //Size of cluster
  struct node_t*        nodes       ; //Pointer to the node in structural tree

} ;

// the structural tree representation
struct structTree_t {
  int                   size        ; //Size of the structural tree
  struct adjTreeList_t* list        ; //List of nodes in structural tree
} ;

/* --------- LOCAL TREE ---------*/
// binary local tree representation
struct localNode_t {
  int                   name        ;
  unsigned char tree[BITMAP_LEN]    ; //the tree bitmap
  unsigned char nonTree[BITMAP_LEN] ; //the nontree bitmap
  int                   pNode       ; //tell whether it is a node that can be pared or not
  struct localNode_t*   left        ; //points to the left child
  struct localNode_t*   right       ; //points to the right child
  struct localNode_t*   parent      ; //points to the parent of local node a
  int                   size        ; //defines the number of nodes
  int                   rank        ; //log2(n), init: copy from struct forest,
                                      //when pair two nodes with rank r: give parent rank r+1
  struct localNode_t*   root        ; //points the local tree root
  struct connected_t*   conArr      ; //array that tells which vertices the nodes are connected to

} ;

struct connected_t{
  struct vertex_t*      to          ; //points at the node it is connected to
  int                   treeEdge    ; //1 if a tree edge, 0 if nontree edge
  int                   level       ; //tell which level edge it is
} ;

struct adjLTList_t {
  struct localNode_t*   node        ; //array of local nodes for a structural forest node
} ;

struct localTree_t {
  int                   size        ; //defines the amount of nodes in total without path nodes
  struct adjLTList_t*   list        ; //the array of local nodes
  int                   build       ; //tell if the tree is finished building when paring
  struct localNode_t*   root        ; //points to the root of the local tree
  int                   roots       ; //defines the number og nodes that are directly connected to the rank path
  int                   pNodes      ; //the number of rank nodes that are a direct children to path nodes
} ;

/* ------------------ FUNCTIONS ------------------*/

/* --------- GRAPH ---------*/
// building the graph
// calling initStructTree
struct graph_t* makeGraph(int size);

// building adj list for connections
struct vertex_t* makeAdjList(int v);

// adding an edge if there is no direct connection
// calling makeAdjList when making a connection
// calling addTree
void addEdge(graph_t graph, int u, int v);

// delete an edge and update
// calling delTree
void deleteEdge(graph_t* graph, vertex_t v, vertex_t u);

// searches for the connection (u,v) and remove connection
// TODO: split the function such that we have a search function and a remove function
void searchEdge(graph_t* graph, vertex_t* vertex, int u, int v);

// check if two vertices are connected
int isConnected(graph_t* graph, int u, int v);

/* --------- STRUCTURAL TREE ---------*/
// makes a new node for the structural tree and initializes the values
// also points to a vertex in the leaves
struct node_t* newNode(int name);

void updateRoot(node_t* newRoot, node_t* delRoot);

// bulding structural tree, initiated state
struct structTree_t* initStructTree(graph_t* graph);

//update level and root for structural tree
//void recurseLevel(node_t* root, node_t* currentRoot, int level);

// merging two nodes a and b in a in the structural tree
void mergeNodes(node_t* a, node_t* b);

// called from addEdge. Updates.
//
void addTree(graph_t* graph, int u, int v);

void delTree(graph_t* graph, int u, int v);

node_t* smallest(node_t* u, node_t* v);

int DFSsmallestTree(node_t* currentRoot, int count);

// replace
void replace(graph_t graph);

void Clusters(struct node_t* node);

int isConnected(graph_t* graph, int u, int v);

int search(adjTreeList_t* cluster, int elem);

void updateSiblings(graph_t* graph, node_t* p, node_t* tv, int level);
void updateClusters(adjTreeList_t* pCluster, adjTreeList_t* tvCluster, int removeElem);
void noReplacementUpdate(node_t* tv, node_t* firstCommonNode,
  graph_t* graph, int currentLevel);

int findReplacement(graph_t* graph, node_t* tv, node_t* tw);
void updateNonTree(int u, int v, graph_t* graph);
int graphConnected(graph_t* graph, int u, int v);
struct node_t* findFirstCommon(graph_t* graph, int u, int v);

/* --------- LOCAL TREE ---------*/
struct node_t* findMinLevelNode(graph_t* graph, int u, int v);

// initializes local tree
struct localTree_t* initLocalTree(node_t* node);

struct localNode_t* newLocalNode(int name);

struct localTree_t* makeLT(node_t* localRoot);

// merging nodes in LT
void mergeLT(node_t* u, node_t* v);

//bitmap updates
void tree(localNode_t* a, int level, int keep);

void nonTree(localNode_t* a, int level, int keep);

// search for connection

void updateLT(localTree_t* tree, node_t* node);

void pairNodes(localTree_t* tree, node_t* node, localNode_t* arr[]);

/*TRASH*/
/*void recurseLevel(node_t* root, node_t* currentRoot, int level){
  //printf("\n");
  node_t* nextSibling = currentRoot;
  while (nextSibling) {
    recurseLevel(root, nextSibling->sibling, level);
    nextSibling->level = level;
    nextSibling->root = root;
    printf("recurseLevel: Root for %d is: %d\n", nextSibling->name, nextSibling->root->name);
    printf("recurseLevel: %d's level is: %d\n", nextSibling->name, nextSibling->level);
    nextSibling = nextSibling->children;
    level++;
  }
} ;*/
