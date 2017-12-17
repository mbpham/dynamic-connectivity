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
  node->cluster = NULL;
  node->size = 0;
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
  }
  structTree->size = graph->size;
  return structTree;
};

//updates structural tree after edge insertion
void updateTree(graph_t* graph, int u, int v){

  node_t* rootu = graph->tree->list[u].nodes->root;
  node_t* rootv = graph->tree->list[v].nodes->root;
  //compare roots
  if(rootu->name != rootv->name){
    //printf("nodes do not have the samme root\n");
    if(rootu->leaf == 1 && rootv->leaf == 1){
      printf("\nBoth %d and %d are leaves\n", rootu->name, rootv->name);
      node_t* node = newNode(graph->tree->size);
      //put into adj list
      graph->tree->list[graph->tree->size].nodes = node;

      //update children
      updateChildren(graph, node, u, v, 0);

      //update parent
      graph->tree->list[u].nodes->root->parent = node;
      graph->tree->list[v].nodes->root->parent = node;

      //update height
      node->height = graph->tree->list[u].nodes->root->height + 1;

      //update root
      graph->tree->list[u].nodes->root = node;
      graph->tree->list[v].nodes->root = node;

      //update levels
      updateLevel(graph, node, u, v, 0, 0, 0, node->name);

      //update tree size
      graph->tree->size = graph->tree->size + 1;

      //printf("Children of %d are %d and %d\n", node->name,
      //node->children[0].nodes->name, node->children[1].nodes->name);
    }
    else if(rootu->leaf == 1 && rootv->leaf == 0){
      //printf("\nRoot of %d is a leaf\n", u);
      //update children
      updateChildren(graph, graph->tree->list[v].nodes->root, u, v, 1);

      //update parent
      graph->tree->list[u].nodes->root->parent = rootv;

      //update root
      graph->tree->list[u].nodes->root = rootv;

      //update children size for root
      (graph->tree->list[v].nodes->root->size) = rootv->size + 1;

      //update level
      updateLevel(graph, (graph->tree->list[v].nodes->root), u, v, 1,
                  graph->tree->list[v].nodes->root->height, 0, rootv->name);

      //update local tree

    }
    else if(rootu->leaf == 0 && rootv->leaf == 1){
      printf("\nRoot of %d is a leaf\n", v);

      //updates children
      updateChildren(graph, graph->tree->list[u].nodes->root, v, u, 1);

      //update parent
      graph->tree->list[v].nodes->root->parent = rootu;

      //update root
      graph->tree->list[v].nodes->root = rootu;

      //update children size for root
      (graph->tree->list[v].nodes->root->size) = rootu->size + 1;

      //update level
      updateLevel(graph, (graph->tree->list[u].nodes->root), u, v, 1,
                  graph->tree->list[v].nodes->root->height, 0, rootu->name);

      //update local tree
    }
    else if(rootu->leaf == 0 && rootv->leaf == 0){
      printf("\nNone are leaves\n");
      node_t* node = newNode(graph->tree->size);

      //update children
      updateChildren(graph, node , u, v, 0);
      graph->tree->list[graph->tree->size].nodes = node;

      //update parents
      graph->tree->list[u].nodes->root->parent = node;
      graph->tree->list[v].nodes->root->parent = node;


      graph->tree->list[v].nodes->root->height = max(rootu->height, rootv->height) + 1;
      updateLevel(graph, node, u, v, 1,
                  graph->tree->list[v].nodes->root->height, 0, node->name);


      //update tree size
      graph->tree->size = graph->tree->size + 1;
      //printf("\nNEW Root of %d: %d\nNEW Root of %d: %d\n", u, graph->tree->list[u].nodes->root->name, v, graph->tree->list[u].nodes->root->name );

      }

    }
  }

/* --------- UPDATES ---------*/

void updateLevel(graph_t* graph, node_t* currentRoot, int u, int v, int Case,
                  int height, int level, int rootnum){
  if(Case == 0){
    graph->tree->list[u].nodes->level = 1;
    graph->tree->list[v].nodes->level = 1;
    currentRoot->level = 0;
}
  else if(Case == 1 && height > -1){
      int i;
      int size = currentRoot->size;
      currentRoot->root = graph->tree->list[rootnum].nodes;
      //runs over num of children
      currentRoot->level = level;
      if(height>0) {
      for(i = 0; i<size; i++){
        updateLevel(graph, currentRoot->children[i].nodes, u, v, 1, height-1,
                    level+1, rootnum);
      }
    }
  }

} ;

void updateChildren(graph_t* graph, node_t* currentRoot, int u, int v, int Case){
  //Both are node u and v are leaves
  if(Case == 0) {
    currentRoot->children = (struct adjTreeList_t*) malloc(2* sizeof(adjTreeList_t));
    currentRoot->children[0].nodes = graph->tree->list[u].nodes->root;
    currentRoot->children[1].nodes = graph->tree->list[v].nodes->root;
    currentRoot->size = 2;
  }
  else if(Case == 1){
    int childSize;
    childSize = (graph->tree->list[v].nodes->root->size);

    //reallocate memory to children and update children
    struct adjTreeList_t* ptr = graph->tree->list[v].nodes->root->children;

    //reallocate memory
    graph->tree->list[v].nodes->root->children = (struct adjTreeList_t*)
    realloc(ptr, (childSize+1 * sizeof(adjTreeList_t)));
    graph->tree->list[v].nodes->root->children->nodes->size = childSize;

    //update children
    graph->tree->list[v].nodes->root->children[childSize].nodes =
    graph->tree->list[u].nodes->root;
  }
} ;
