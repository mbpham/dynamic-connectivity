#include <iostream>
#include <string>
#include <sstream>
using namespace std;
#include "ltlib.cpp"
#include <math.h>

/* --------- STRUCTURAL TREE INITIALIZING AND HELPER FUNCTIONS ---------*/
//Function for making a structural node
struct node_t* newNode(int name){
  struct node_t* node = (struct node_t*) malloc(sizeof(node_t));
  node->name = name;
  node->parent = NULL;
  node->root = node;
  node->sibling = NULL;
  node->last = node;
  node->leaf = 0;
  node->size = 0;
  node->rank = 0;
  //CHANGE: try realloc when adding
  node->cluster = (struct adjTreeList_t*) malloc(10*sizeof(adjTreeList_t));
  return node;
};

// initialize struct tree
// Build the structural tree
struct structTree_t* initStructTree(graph_t* graph){
  int i, j;

  //allocate memory in the structural tree
  structTree_t* structTree = (struct structTree_t*) malloc(sizeof(struct structTree_t));
  structTree->list = (struct adjTreeList_t*) malloc(graph->size * sizeof(struct adjTreeList_t));

  //initialize structural forest. The leaves of C/vertices of G are represented in structTree->list
  for(i = 0; i< graph->size; i++){
    structTree->list[i].nodes = newNode(i);
    structTree->list[i].nodes->leaf = 1;



    //make parents for the node until the forest depth is reached
    int depth = ceil(log2(graph->size));
    node_t* node = structTree->list[i].nodes;
    for(j = 0; j<depth; j++){
      node->parent = newNode(i);
      node->level = depth - (j+1);
      node->parent->children = node;
      node->localTree = initLocalTree(node);
      if(j == depth-1){
        break;
      }
      node = node->parent;
    }
    //update root for all descending nodes
    node_t* root = node;
    for(j = 0; j<depth; j++){
      node->root = root;
      node->n = 1;
      node->cluster[0].nodes = structTree->list[i].nodes;
      node->cluster->size = 1;
      node = node->children;
    }
  }

  structTree->size = graph->size;
  return structTree;
};

/* --------- UPDATES AFTER INSERTION OR DELETION --------- */

//updates structural tree after edge insertion
void addTree(graph_t* graph, int u, int v){
  //updates local tree in mergeNodes
  mergeNodes(graph->tree->list[u].nodes->root, graph->tree->list[v].nodes->root);
  //TODO: update tree bitmap
  //TODO: update tree bitmap for graph->tree->list[u].nodes in level i
  tree(graph->tree->list[u].nodes->localTree->list[0].node, 0, 1);
  //TODO: update tree bitmap for graph->tree->list[v].nodes in level i
  tree(graph->tree->list[v].nodes->localTree->list[0].node, 0, 1);

  //TODO: update bitmaps to root of structural tree
  updateBitmaps(graph->tree, u);
  updateBitmaps(graph->tree, v);

} ;

//Updates the structural forest after a level i deletion
void delTree(graph_t* graph, int u, int v, int level){

  printf("\ndelTree: Running size procedure\n");
  tv_t* tvp = (struct tv_t*) malloc(sizeof(tv_t));
  tvp->size = 0;
  tvp->arr = (struct adjTreeList_t*) malloc(100* sizeof(adjTreeList_t));
  tvp->visited1 = (int*) malloc(50*sizeof(int));
  tvp->visited2 = (int*) malloc(50*sizeof(int));

  tv_t* tvu = (struct tv_t*) malloc(sizeof(tv_t));
  tvu->size = 0;
  tvu->arr = (struct adjTreeList_t*) malloc(100* sizeof(adjTreeList_t));
  tvu->visited1 = (int*) malloc(sizeof(int));
  tvu->visited2 = (int*) malloc(sizeof(int));

  Size(tvp, graph->tree->list[u].nodes, graph->tree->list[v].nodes, level);
  Size(tvu, graph->tree->list[v].nodes, graph->tree->list[u].nodes, level);

  //defining tv
  node_t* tv;

  int minSize = min(tvu->size, tvp->size);
  if(tvu->size == minSize){
    tv = tvu->node;
    printf("%d\n", tvu->arr[0].nodes->name);
  }
  else{
    tv = tvp->node;
    printf("%d\n", tvp->arr[0].nodes->name);
  }


    //TODO: Increse levels of level i edges in smaller tree saved before

  //TODO: structural changes
    //TODO: search for a replacement for (u,v) on level i
    //TODO: CASES: if replacement found or not
      //TODO: CASES: if i=0, i>0

}

/* --------- SEARCHES ---------*/

//Finds the number of nodes connected to y in F_i \ (x,y)
void Size(tv_t* tv, node_t* x, node_t* y, int level){
  //Seeking for y^(i+1)
  //Insert (x,y) to list of visited edges
  tv->visited1[tv->arrSize] = x->name;
  tv->visited2[tv->arrSize] = y->name;
  printf("\nSize: visited(%d,%d)\n", x->name, y->name);
  printf("Size: searching for y^(i+1)\n");

  node_t* yi1 = y;
  while(yi1->level != level+1){
    yi1 = yi1->parent;
  }

  //Update
  tv->size = tv->size + yi1->n;


  //search down from y^i+1 in local and structural tree using the bitmaps
  structSearchDown(tv, yi1, level);
  //printf("Size of array: %d\n", tv->arrSize);
  tv->node = yi1;

}

//searchDown the structural nodes
void structSearchDown(tv_t* tv, node_t* yi1, int level){
    printf("structSearchDown: searching in structural nodes, %d\n", yi1->level);
    localNode_t* currentLocal = yi1->localTree->root;
    //Check if the local node rooted in yi1 has an incident tree edge in level i
    if(isEdge(currentLocal->tree, level)){
      //Check if we have reached the leaf of the structural tree
      if(yi1->leaf == 1){
        printf("structSearchDown: found a leaf with level %d tree edge\n", level);
        printf("structSearchDown: add node %d, level %d to arr\n", yi1->name, yi1->level);
        tv->arr[tv->arrSize].nodes = yi1;
        tv->arrSize = tv->arrSize + 1;

        printf("structSearchDown: search for connections\n");
        vertex_t* connections = yi1->to;
        while(connections){
          printf("structSearchDown: found a connection (%d,%d) \n", yi1->name, connections->name);
          //recursive call to Size for each level i edge !(x,y)
          if(!(visited(tv->visited1, tv->visited2, yi1->name, connections->name, tv->arrSize))) {
            if(connections->level == level && connections->nontreeEdge == 0){
              printf("structSearchDown: edge (%d,%d) has not been visited, call size.\n", yi1->name, connections->name);
              Size(tv, yi1, connections->structNode, level);
            }
            else{
              printf("structSearchDown: edge (%d,%d) is a non-tree edge\n", yi1->name, connections->name);
            }
          }
          else{
            printf("structSearchDown: (%d,%d) already visited\n", yi1->name, connections->name);
          }
          connections = connections->next;
        }
        return;
      }
      //move to its child
      currentLocal = currentLocal->right;

      //While there are still local nodes we search down breadth first
      localSearchDown(tv, currentLocal, level);


    }
    else{
      return ;
    }
}

//search down the local trees
void localSearchDown(tv_t* tv, localNode_t* currentLocal, int level){
  if(isEdge(currentLocal->tree, level)){
    //return local nodes corresponding structural node if it is a leaf in the local tree
    if(!(currentLocal->right&&currentLocal->left)){
      structSearchDown(tv, currentLocal->belongsTo, level) ;
    }
    else{
      if(isEdge(currentLocal->right->tree, level)){
        localSearchDown(tv, currentLocal->right, level);

      }
      if(isEdge(currentLocal->left->tree, level)){
        localSearchDown(tv, currentLocal->left, level);
      }
    }
  }
  else{
    return;
  }

}

int visited(int vis1[], int vis2[], int seek1, int seek2, int size){
  for(int i = 0 ; i<size ; i++){
    if(vis1[i] == seek1 && vis2[i] == seek2){
      return 1;
    }
    if(vis1[i] == seek2 && vis2[i] == seek1){
      return 1;
    }
  }
  return 0;
}

//Look for two vertices first common node in structural forest
/*struct node_t* findFirstCommon(graph_t* graph, int u, int v){
  node_t* minLevelNode;
  minLevelNode = findMinLevelNode(graph, u, v);

  //First the first common node
  minLevelNode = minLevelNode->parent;
  while(minLevelNode){
    if(search(minLevelNode->cluster, u) == 1 && search(minLevelNode->cluster, v) == 1){
      printf("\ndelTree: Matching node found: %d at level %d\n", minLevelNode->name, minLevelNode->level);
      return minLevelNode;
    }
    minLevelNode = minLevelNode->parent;
  }
  return minLevelNode;
}*/

struct node_t* findMinLevelNode(graph_t* graph, int u, int v){
  int high = min(graph->tree->list[u].nodes->level, graph->tree->list[v].nodes->level);
  if (graph->tree->list[u].nodes->level == high){
    //printf("Level of %d is %d\n", graph->tree->list[u].nodes->name, high);
    return graph->tree->list[u].nodes;
  }
  else{
    //printf("Level of %d is %d\n", graph->tree->list[v].nodes->name, high);
    return graph->tree->list[v].nodes;
  }
};

//TODO: after collecting T_v: test if a nontree edge (x,y) is a replacement edge
//TODO: test if x^i+1 != y^i+1
int findReplacement(graph_t* graph, node_t* tv, node_t* tw){
  //looking at the linked list for tw
  //and see if there is anything from the list that is connected to tv
  vertex_t* twConnections = graph->graphArr[tw->name].vertex;
  while (twConnections) {
    //Look at the connections in the tw connections
    vertex_t* posRepl = graph->graphArr[twConnections->name].vertex;
    while(posRepl){
      //If we find the same node in any of the connections
      //we have found a replacement
      if(posRepl->name == tv->name){
        printf("findReplacement: Replacement found at %d\n", twConnections->name);

        updateNonTree(posRepl->name, twConnections->name, graph);
        return 1;
      }
      posRepl = posRepl->next;
    }
    twConnections = twConnections->next;
  }
  return 0;

}

//Return 1 if elem is in the cluster, return 0 otherwise
int search(adjTreeList_t* cluster, int elem){
  int size = cluster->size;
  int i = 0;
  while (i < size && elem != cluster[i].nodes->name) {
      i++;
  }
  if(i < size){
    return 1;
  }
  else{
    return 0;
  }
}

//find first common node for node u and v
int findUpdateLevel(structTree_t* structTree, int u, int v){
  node_t* node = structTree->list[u].nodes;
  while(node){
    if(search(node->cluster, v) == 1){
      return node->level;
    }
    node = node->parent;
  }

  return 0;
}

/* --------- UPDATES ---------*/
struct node_t* smallest(node_t* u, node_t* v){
  int uSize = DFSsmallestTree(u, 0);
  int vSize = DFSsmallestTree(v, 0);

  if(uSize < vSize){
    return u;
  }
  else if (vSize < uSize){
    return v;
  }
  else{
    return u;
  }
}
//removes nontree edges and add them as tree edges
void updateNonTree(int u, int v, graph_t* graph){
  vertex_t* nextu = graph->graphArr[u].vertex;
  vertex_t* nextv = graph->graphArr[v].vertex;
  //delete edge from nontree edge set in the other direction
  while(nextu){
    if(nextu->name == v){
      printf("delTree: Change (%d, %d) to a tree edge\n", nextu->name, v);
      nextu->nontreeEdge = 0;
      break;
    }
    nextu = nextu->next;
  }
  while(nextv){
    if(nextv->name == u){
      printf("delTree: Change (%d, %d) to a tree edge\n", nextv->name, u);
      nextv->nontreeEdge = 0;
      break;
    }
    nextv = nextv->next;
  }
}

void Clusters(struct node_t* node){
  int i;
  /*struct node_t* child = node->children;
  while(child){
    printf("Node that belongs to %d: %d\n", node->name, child->name);
    child = child->sibling;
  }*/
  printf("\n");
  for(i = 0; i<node->cluster->size;i++){
    printf("Clusters: Node that belongs to %d: %d\n", node->name, node->cluster[i].nodes->name);
  }
}

void updateRoot(node_t* newRoot, node_t* delRoot){
  node_t* child = delRoot;
  while(child){
    child->root = newRoot;
    updateRoot(newRoot, child->sibling);
    child = child->children;
  }

}

//merge nodes a and b, when adding an edge
void mergeNodes(node_t* a, node_t* b){
  printf("mergeNodes: Merging %d and %d in %d\n", a->name, b->name, a->name);
  struct node_t* child = b->children;

  //make the last child of a be a sibling to first child of b
  a->children->last->sibling = b->children;
  a->children->last = b->children->last;

  //update root pointer
  updateRoot(a, b);

  //make each child of b be a child of a
  while(child){
    a->size = a->size +1;
    child->parent = a;
    addLT(a->localTree, child);
    child = child->sibling;
  }

  //Updates the number of descending leaves
  a->n = a->n + b->n;
  //Updates rank
  a->rank = floor(log2(a->n));

  //Updates cluster and cluster size
  int i;
  for(i = 0; i<b->cluster->size;i++){
    a->cluster[a->cluster->size].nodes = b->cluster[i].nodes;
    a->cluster->size++;
  }

  b = NULL;

}

int DFSsmallestTree(node_t* currentRoot, int count){
  node_t* nextSibling = currentRoot;
  while (nextSibling) {
    count++;
    DFSsmallestTree(nextSibling->sibling, count);
    nextSibling = nextSibling->children;
  }
  return count;
}
