#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
using namespace std;

struct removedEdge_t {
  int level;
  int nontreeEdge;
  int hej;
} ;

struct replacement_t{
  int replacement;
  int numReplacements;
  int replacementNodeu;
  int replacementNodev;
} ;

struct tv_t {
  int                     size      ;
  int                     arrSize   ;
  struct adjTreeList_t*   arr       ;
  int *                   visited1  ;
  int *                   visited2  ;
  struct node_t*          node      ;
  struct node_t*          visited   ;
} ;

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
  struct  node_t*       structNode  ; //Points to the structural leaf that represents the vertex
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
  struct localNode_t*          parentLeaf; //Points to the local node leaf from the parents local tre
  int                          size      ; //num of children
  int                          n         ; //Number of leaves descending from node
  int                          rank      ; //Rank of node is log2(n)
  int                          level     ; //level of the node
  int                          leaf      ; //1 if the node is a leaf in structTree
  int                          height    ; //Height of the subtree rooted in node
  int                          name      ; //Key of the node
  struct vertex_t*             to        ; //1 elem cluster (leaves) points to the vertices, to later see connections
} ;

// adj list for nodes
struct adjTreeList_t {
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
  struct node_t*        belongsTo   ; //point to the structural node it belongs to
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
  struct adjLTList_t*   rankRoots   ;
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
void searchEdge(graph_t* graph, removedEdge_t* rem, vertex_t* vertex, int u, int v);

// check if two vertices are connected
int isConnected(graph_t* graph, int u, int v);
int countNonTreeEdges(graph_t* graph, int vertexIndex, int level);

/* --------- STRUCTURAL TREE ---------*/
//initial state
struct structTree_t* initStructTree(graph_t* graph);
struct node_t* newNode(int name);
struct tv_t* initTv();

//insert
void addTree(graph_t* graph, int u, int v);
void mergeNodes(node_t* a, node_t* b); // merging two nodes a and b in a in the structural tree

//delete
void delTree(graph_t* graph, int u, int v, int level);
int search(adjTreeList_t* cluster, int elem, int n);

//size procedure
void Size(tv_t* tv, node_t* x, node_t* y, int level);
void structSearchDown(tv_t* tv, node_t* yi1, int level);
void localSearchDown(tv_t* tv, localNode_t* currentLocal, int level);
int visited(int vis1[], int vis2[], int seek1, int seek2, int size);

//searching for a replacement
void findReplacement(graph_t* graph, int u, int v, node_t* tvNode, node_t* twNode, int level, replacement_t* rep);
void checkNonTreeEdges(node_t* tvNode, replacement_t* rep, int level);
void splitCluster(node_t* tv, node_t* parent);
void splitNodes(node_t* tv, node_t* parent);
void removeSibling(node_t* tv, node_t* parent);

//other functions
int findUpdateLevel(structTree_t* structTree, int u, int v);
void updateNonTree(int u, int v, graph_t* graph);
struct node_t* findLevelnode(node_t* node, int level);

/* --------- LOCAL TREE ---------*/
//initial states
struct localTree_t* initLocalTree(node_t* node);
struct localNode_t* newLocalNode(int name);

//adding nodes
void addLT(localTree_t* a, node_t* b);


//removing nodes
void delLT(localTree_t* a, node_t* b);
void updateRankRoots(localTree_t* tree, localNode_t* node, localNode_t* rem);

// merging nodes in LT
void mergeLT(localTree_t* Tu, localTree_t* Tv);
void updateLT(localTree_t* tree);
void pairNodes(localTree_t* Tu, localNode_t* arr[]);
void delRankPath(localTree_t* Tu, localTree_t* Tv);
void buildRankPath(localTree_t* Tu, localNode_t* arr[]);

//bitmap updates
void updateNonBitmaps(structTree_t* structTree, int node, int level);
void updateBitmaps(structTree_t* structTree, int node, int level);
void nonbitWiseAND(localNode_t* a, localNode_t* b, localNode_t* newNode);
void nonbitWiseOR(localNode_t* a, localNode_t* b, localNode_t* newNode);
void tree(localNode_t* a, int level, int keep);
void nonTree(localNode_t* a, int level, int keep);
void byte_as_bits(char bitmap);
