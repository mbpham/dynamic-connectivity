#include <iostream>
#include <string>
#include <sstream>
using namespace std;
#include "lib.hpp"
#include <math.h>

/* --------- LOCAL TREE ---------*/
// building local tree

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
  //localTree->list[0] = newLocalNode(node->name);
  return localTree;
} ;


void tree(localNode_t* a){

} ;

void nonTree(localNode_t* a){

} ;

void updateLT(structTree_t* structTree, node_t* localRoot){
  //sort children in ranks

  //pair nodes with the same rank r

    //make a parent node with rank r+1

  //make a path from local root
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
