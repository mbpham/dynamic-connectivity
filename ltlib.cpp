#include <iostream>
#include <string>
#include <sstream>
using namespace std;
#include "lib.hpp"
#include <math.h>

/* --------- LOCAL TREE ---------*/
// building local tree

int comp(const void *p, const void *q)
{
    int l = ((struct node_t *)p)->rank;
    int r = ((struct node_t *)q)->rank;
    return (l - r);
}

struct localNode_t* newLocalNode(int name){
  struct localNode_t* newNode = (struct localNode_t*) malloc(sizeof(struct localNode_t));
  newNode->name = name;
  newNode->tree = 0;
  newNode->nonTree = 0;
  return newNode;
} ;

struct localTree_t* initLocalTree(graph_t* graph, node_t* node){
  localTree_t* localTree = (struct localTree_t*) malloc(sizeof(struct localTree_t));
  localTree->list = (struct adjLTList_t*) malloc(sizeof(struct adjLTList_t));
  localTree->list[0].node = newLocalNode(node->name);
  return localTree;
} ;


void tree(localNode_t* a){

} ;

void nonTree(localNode_t* a){

} ;

struct localTree_t* makeLT(structTree_t* structTree, node_t* localRoot){
  localTree_t* localTree = (struct localTree_t*) malloc(sizeof(struct localTree_t));
  localTree->list = (struct adjLTList_t*) malloc(2*localRoot->n * sizeof(struct adjLTList_t));
  //sort children in ranks
  struct adjTreeList_t* arr;
  localTree->size = localRoot->size+1;
  arr = localRoot->children;
  qsort(arr, sizeof(arr)/sizeof(*arr), sizeof(*arr), comp);
  int i;
  int size = structTree->size;
  //pair nodes with the same rank r
  for(i = 0; i<localRoot->size;(i+=2)){
    if(arr[i].nodes->rank == arr[i+1].nodes->rank){
      //CHANGE: the name of parent
      localNode_t* newParent = newLocalNode(size);
      localNode_t* leftNode = newLocalNode(arr[i].nodes->name);
      localNode_t* rightNode = newLocalNode(arr[i+1].nodes->name);

      //copy ranks
      newParent->rank = arr[i].nodes->rank + 1;
      leftNode->rank = arr[i].nodes->rank;
      rightNode->rank = arr[i].nodes->rank;

      //update parent and children
      newParent->left = leftNode;
      newParent->right = rightNode;
      rightNode->parent = newParent;
      leftNode->parent = newParent;

      //place in local tree list
      localTree->list[arr[i].nodes->name].node = leftNode;
      localTree->list[arr[i+1].nodes->name].node = rightNode;
      localTree->list[localTree->size].node = newParent;
      //printf("%d\n", newParent->name);
      //increase size of local tree by 1
      localTree->size++;
      size++;

      }
    else{
      i--;
    }
  }
    //make a parent node with rank r+1

  //make a path from local root
  return localTree;
};


// Insert
//void insert(int u, int v){
//  int connected = isConnected(u, v);
//  if (not connected){
  //   Insert the edge (u,v)
//  }
//};

// Delete

// replace
void replace(graph_t graph){

};

// merging nodes when running edge delete
void merge(){

};

// search for connection
void search(){

};
