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
void updateTree(graph_t* graph, int u, int v){
  node_t* rootu = graph->tree->list[u].nodes->root;
  node_t* rootv = graph->tree->list[v].nodes->root;
  if(rootu != rootv){
    if(rootu->leaf == 1 && rootv->leaf == 1){
      printf("Both %d and %d are leaves\n", rootu->name, rootv->name);

      //make a new parent node for u and v
      node_t* node = newNode(graph->tree->size);
      node->sibling = NULL;
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
      graph->tree->list[u].nodes->root->parent = graph->tree->list[graph->tree->size].nodes;
      graph->tree->list[v].nodes->root->parent = graph->tree->list[graph->tree->size].nodes;


      //update tree size
      graph->tree->size++;

      //make local tree for new node
      //graph->tree->list[v].nodes->root->localTree = makeLT(graph->tree,   graph->tree->list[v].nodes->root);

      graph->tree->list[u].nodes->root->localTree = makeLT(graph->tree->list[u].nodes->root);
    }
    else if(rootu->leaf == 1 && rootv->leaf == 0){
      printf("\nRoot of %d is a leaf\n", u);
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

      //update local tree
    }
    else if(rootu->leaf == 0 && rootv->leaf == 1){
      updateTree(graph, v, u);
    }
    else if(rootu->leaf == 0 && rootv->leaf == 0){
      printf("None are leaves\n");

      mergeNodes(graph->tree->list[u].nodes->root, graph->tree->list[v].nodes->root);
      recurseLevel(graph->tree->list[u].nodes->root->parent, graph->tree->list[u].nodes->root->parent, 0);

      }
    }
  else{
    printf("\n%d and %d share the same root\n", u, v);
  }
  //update tree bitmap at level i
  //updateLT(graph->tree->list[u].nodes->localTree, graph->tree->list[u].nodes->root);

  //printf("n is now: %d\n",graph->tree->list[u].nodes->root->n);
  //printf("rank is now: %d\n",graph->tree->list[u].nodes->root->rank);

  //update LT here

} ;

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
  struct node_t* child = b->children;

  a->children->last->sibling = b->children;
  while(child){
    a->size = a->size +1;
    child->parent = a;
    child = child->sibling;
  }
  a->n = a->n+ b->n;
  a->rank = floor(log2(a->n));
  b = NULL;


}
