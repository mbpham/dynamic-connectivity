#include <iostream>
#include <string>
#include <sstream>
using namespace std;
#include "lib.hpp"
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
  if(rootu != rootv){
    if(rootu->leaf == 1 && rootv->leaf == 1){
      printf("\nBoth %d and %d are leaves\n", rootu->name, rootv->name);
      node_t* node = newNode(graph->tree->size);
      //put into adj list
      graph->tree->list[graph->tree->size].nodes = node;

      //update parent
      graph->tree->list[u].nodes->root->parent = node;
      graph->tree->list[v].nodes->root->parent = node;

      //update root
      graph->tree->list[u].nodes->root = node;
      graph->tree->list[v].nodes->root = node;

      //update levels
      graph->tree->list[u].nodes->level = 1;
      graph->tree->list[v].nodes->level = 1;
      node->level = 0;

      //update tree size
      graph->tree->size = graph->tree->size + 1;
    }
    else if(rootu->leaf == 1 && rootv->leaf == 0){
      printf("\nRoot of %d is a leaf\n", u);

      //update parent
      graph->tree->list[u].nodes->root->parent = rootv;

      //update root
      graph->tree->list[u].nodes->root = rootv;

      //update level
      graph->tree->list[u].nodes->level = (graph->tree->list[v].nodes->root->level)+1;

      //update local tree
    }
    else if(rootu->leaf == 0 && rootv->leaf == 1){
      printf("\nRoot of %d is a leaf\n", v);

      //update parent
      graph->tree->list[v].nodes->root->parent = rootu;

      //update root
      graph->tree->list[v].nodes->root = rootu;

      //update level
      graph->tree->list[v].nodes->level = (graph->tree->list[u].nodes->root->level)+1;

      //update local tree
    }
    else{
      printf("\nNone are leaves\n");
      node_t* node = newNode(graph->tree->size);
      graph->tree->list[graph->tree->size].nodes = node;
      //update parents
      graph->tree->list[u].nodes->root->parent = node;
      graph->tree->list[v].nodes->root->parent = node;

      //Update roots
      graph->tree->list[u].nodes->root = node;
      graph->tree->list[v].nodes->root = node;

      int i;
      //Update levels
      for(i = 0; i<graph->tree->size){
        
      }

      }

      //update tree size
      graph->tree->size = graph->tree->size + 1;

      //update local tree
    }

  }
