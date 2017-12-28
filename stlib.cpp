#include <iostream>
#include <string>
#include <sstream>
using namespace std;
#include "ltlib.cpp"
#include <math.h>

/* --------- STRUCTURAL TREE ---------*/
// initialize struct tree
//struct structTree_t* initStructTree(graph_t* graph){
//};

struct node_t* newNode(int name){
  struct node_t* node = (struct node_t*) malloc(sizeof(node_t));
  node->name = name;
  node->parent = NULL;
  node->root = node;
  node->level = 0;
  node->size = 0;
  node->n = 0;
  node->rank = 0;
  //CHANGE: try realloc when adding
  node->cluster = (struct adjTreeList_t*) malloc(10*sizeof(adjTreeList_t));
  return node;
};

// Build the structural tree
struct structTree_t* initStructTree(graph_t* graph){
  structTree_t* structTree = (struct structTree_t*) malloc(sizeof(struct structTree_t));
  structTree->list = (struct adjTreeList_t*) malloc(graph->size * sizeof(struct adjTreeList_t));
  int i;
  //point to each level 0 cluster
  for(i = 0; i< graph->size; i++){
    structTree->list[i].nodes = newNode(i);
    structTree->list[i].nodes->leaf = 1;
    structTree->list[i].nodes->root = structTree->list[i].nodes;

    //init local tree
    structTree->list[i].nodes->localTree = initLocalTree(structTree->list[i].nodes);
      }

  structTree->size = graph->size;
  return structTree;
};

//updates structural tree after edge insertion
void addTree(graph_t* graph, int u, int v){
  node_t* rootu = graph->tree->list[u].nodes->root;
  node_t* rootv = graph->tree->list[v].nodes->root;
  if(rootu != rootv){
    if(rootu->leaf == 1 && rootv->leaf == 1){
      printf("Both %d and %d are leaves\n", rootu->name, rootv->name);

      //make a new parent node for u and v
      node_t* node = newNode(graph->tree->size);

      node->sibling = NULL;

      //update cluster
      node->cluster[0].nodes = graph->tree->list[u].nodes;
      node->cluster[1].nodes = graph->tree->list[v].nodes;
      node->cluster->size = 2;


      //put into adj list
      graph->tree->list[graph->tree->size].nodes = node;
      //updates the subtree size
      node->n = node->n + 2;
      node->rank = ceil(log2(node->n));


      //update height
      node->height = graph->tree->list[u].nodes->root->height + 1;

      //update child
      node->children = graph->tree->list[u].nodes;
      node->size = 2;

      //update siblings
      graph->tree->list[u].nodes->sibling = graph->tree->list[v].nodes;
      graph->tree->list[u].nodes->last = graph->tree->list[v].nodes;
      graph->tree->list[v].nodes->sibling = NULL;

      //update levels
      recurseLevel(node, node, 0);

      //update parent
      graph->tree->list[u].nodes->root = graph->tree->list[graph->tree->size].nodes;
      graph->tree->list[v].nodes->root = graph->tree->list[graph->tree->size].nodes;
      graph->tree->list[u].nodes->parent = graph->tree->list[graph->tree->size].nodes;
      graph->tree->list[v].nodes->parent = graph->tree->list[graph->tree->size].nodes;

      //update tree size
      graph->tree->size++;

      graph->tree->list[u].nodes->root->localTree = makeLT(graph->tree->list[u].nodes->root);
    }
    else if(rootu->leaf == 1 && rootv->leaf == 0){
      printf("\nRoot of %d is a leaf\n", u);

      //update cluster
      int clusterSize = graph->tree->list[v].nodes->root->cluster->size;
      graph->tree->list[v].nodes->root->cluster[clusterSize].nodes = graph->tree->list[u].nodes;
      graph->tree->list[v].nodes->root->cluster->size++;

      mergeLT(graph->tree->list[v].nodes->root, graph->tree->list[u].nodes->root);
      //Update subtree size
      graph->tree->list[v].nodes->root->n = rootv->n + 1;
      graph->tree->list[v].nodes->root->rank = ceil(log2(graph->tree->list[v].nodes->root->n));

      //update siblings
      rootv->children->last->sibling = graph->tree->list[u].nodes;
      rootv->children->last = graph->tree->list[u].nodes;
      graph->tree->list[u].nodes->sibling = NULL;

      graph->tree->list[u].nodes->root = graph->tree->list[v].nodes->root;
      graph->tree->list[u].nodes->parent = graph->tree->list[v].nodes->root;

      recurseLevel(rootv, rootv, 0);

      //update children size for root
      (graph->tree->list[v].nodes->root->size) = rootv->size+1;

    }
    else if(rootu->leaf == 0 && rootv->leaf == 1){
      addTree(graph, v, u);
      return;

    }
    else if(rootu->leaf == 0 && rootv->leaf == 0){
      printf("None are leaves\n");

      mergeNodes(graph->tree->list[u].nodes->root, graph->tree->list[v].nodes->root);
      mergeLT(graph->tree->list[u].nodes->root, graph->tree->list[v].nodes->root);
      recurseLevel(graph->tree->list[u].nodes->root->parent, graph->tree->list[u].nodes->root->parent, 0);
      //updateLT(graph->tree->list[u].nodes->root->localTree, graph->tree->list[u].nodes->root);
      }
    }
    //Update tree(a) bitmap
  else{
    printf("\n%d and %d share the same root\n", u, v);
    //Update non-tree(a) bitmap
  }

  printf("\nValues in cluster\n");
  int k;
  for(k = 0; k<graph->tree->list[v].nodes->root->cluster->size;k++){
    printf("addTree: Cluster value %d\n", graph->tree->list[v].nodes->root->cluster[k].nodes->name);
  }

  //update tree bitmap at level i
  //updateLT(graph->tree->list[u].nodes->localTree, graph->tree->list[u].nodes->root);

} ;

void delTree(graph_t* graph, int u, int v){
  //find the first common local root
  node_t* minLevelNode;
  int high = min(graph->tree->list[u].nodes->level, graph->tree->list[v].nodes->level);
  if (graph->tree->list[u].nodes->level == high){
    //printf("Level of %d is %d\n", graph->tree->list[u].nodes->name, high);
    minLevelNode = graph->tree->list[u].nodes;
  }
  else if (graph->tree->list[v].nodes->level == high){
    //printf("Level of %d is %d\n", graph->tree->list[v].nodes->name, high);
    minLevelNode = graph->tree->list[v].nodes;
  }

  minLevelNode = minLevelNode->parent;
  while(1){
    printf("Parent is %d\n", minLevelNode->name);
    if(search(minLevelNode->cluster, u) == 1 && search(minLevelNode->cluster, v) == 1){
      printf("delTree: Matching node found: %d at level %d\n", minLevelNode->name, minLevelNode->level);
      break;
    }
    minLevelNode = minLevelNode->parent;
  }

  //increase level by 1
  //look at children of minLevelNode


  //Look for a replacement edge


}

int search(adjTreeList_t* cluster, int elem){
  printf("2\n");
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

/* --------- UPDATES ---------*/

void recurseLevel(node_t* root, node_t* currentRoot, int level){
  node_t* nextSibling = currentRoot;
  while (nextSibling) {
    recurseLevel(root, nextSibling->sibling, level);
    nextSibling->level = level;
    nextSibling->root = root;
    //printf("Root for %d is: %d\n", nextSibling->name, nextSibling->root->name);
    //printf("%d's level is: %d\n", nextSibling->name, nextSibling->level);
    nextSibling = nextSibling->children;
    level++;
  }
} ;

void Clusters(struct node_t* node){
  int i;
  struct node_t* child = node->children;
  while(child){
    printf("%d\n", child->name);
    child = child->sibling;
  }
}

void mergeNodes(node_t* a, node_t* b){
  printf("mergeNodes: Merging %d and %d in %d\n", a->name, b->name, a->name);
  struct node_t* child = b->children;
  //Updates children
  a->children->last->sibling = b->children;
  while(child){
    a->size = a->size +1;
    child->parent = a;
    printf("Parent to %d is %d\n", child->name, a->name);
    child = child->sibling;
  }
  a->n = a->n+ b->n;
  a->rank = floor(log2(a->n));

  //Updates cluster
  int i;
  for(i = 0; i<b->cluster->size;i++){
    a->cluster[a->cluster->size].nodes = b->cluster[i].nodes;
    a->cluster->size++;
  }

  b = NULL;

}
