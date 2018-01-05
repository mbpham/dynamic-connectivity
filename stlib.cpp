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

struct tv_t* initTv(){
  tv_t* tv = (struct tv_t*) malloc(sizeof(tv_t));
  tv->size = 0;
  tv->arr = (struct adjTreeList_t*) malloc(100* sizeof(adjTreeList_t));
  tv->visited1 = (int*) malloc(50*sizeof(int));
  tv->visited2 = (int*) malloc(50*sizeof(int));
  return tv;
}

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

  tv_t* tvp = initTv();
  tv_t* tvu = initTv();

  //Finding size of trees, but we do not run the procedures parallel to each other
  Size(tvp, graph->tree->list[u].nodes, graph->tree->list[v].nodes, level);
  Size(tvu, graph->tree->list[v].nodes, graph->tree->list[u].nodes, level);

  //defining tv
  tv_t* smaller;
  tv_t* larger;

  //Finding smallest tree
  int minSize = min(tvu->size, tvp->size);
  if(tvu->size == minSize){
    smaller = tvu;
    larger = tvp;
  }
  else{
    smaller = tvp;
    larger = tvu;
  }

  printf("delTree: Smaller tree is %d\n", smaller->node->name);
  node_t* tv = smaller->node;
  if(smaller->size == 1){
    //TODO: update tree bitmap such that it does not have any tree edges
    int updateIndex = smaller->arr[0].nodes->name;
    localNode_t* singleLeaf = graph->tree->list[updateIndex].nodes->localTree->root;
    tree(singleLeaf, level, 0);

  }
  else{
    //increase all level i edges in smaller tree by 1
    for(int i = 0 ; i < (smaller->size) ; i++){
      vertex_t* vertex = smaller->arr[i].nodes->to;

      while (vertex) {
        if(vertex->level == level){
          vertex->level++;
        }
        vertex = vertex->next;
      }
      //merging nodes in level i+1
      node_t* mergeNode = findLevelnode(smaller->arr[i].nodes, level+1);

      if(tv!= mergeNode){
        //merge tv and mergenode in tv
        mergeNodes(tv, mergeNode);
      }
      //update single tree bitmap
      tree(smaller->arr[i].nodes->localTree->root, level, 0);
      tree(smaller->arr[i].nodes->localTree->root, level+1, 1);

      //update all tree bitmaps in tv and to the root
      updateBitmaps(graph->tree, smaller->arr[i].nodes->name);

      //TODO: update siblings and cluster in tw
    }
  }
  printf("Verices in merged node\n");
  for(int i = 0; i<tv->n ; i++){
    printf("%d, ", tv->cluster[i].nodes->name);
  }



  //TODO: structural changes
    //TODO: search for a replacement for (u,v) on level i
  replacement_t* rep = (struct replacement_t*) malloc(sizeof(replacement_t));
  rep->replacement = 0;
  rep->numReplacements = 0;
  findReplacement(graph->tree, tv, level, rep);


}

/* --------- SEARCHES ---------*/
/* --------- FIND REPLACEMENT EDGE ---------*/

//Find a replacement non-tree edge in level i, for tvNode
void findReplacement(structTree_t* tree, node_t* tvNode, int level, replacement_t* rep){
  //Check if there is a replacement yet, if so, stop
  for(i = 1; i<tvNode->n; i++){
    
  }

  if(rep->replacement){
    return;
  }
  else{
    if(level == 0){
      printf("Reached the root, there is no replacement edge\n");
      node_t* newParent = newNode(tvNode->name);
      newParent->level = tvNode->level-1;
      newParent->children = tvNode;
      newParent->localTree = initLocalTree(newParent);

      tvNode->parent = newParent;
    }
    else{
      findReplacement(tree, tvNode->parent, level-1, rep);
    }
  }
}

void structFindReplacement(){

}

void localFindReplacement(){

}


/* --------- SIZE PRODEDURE ---------*/
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


/* --------- NODE SEARCHES ---------*/

struct node_t* findLevelnode(node_t* node, int level){
  while(node->level != level){
    node = node->parent;
  }
  return node;
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
