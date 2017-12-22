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
    printf("compare\n");
    struct node_t* l = ((struct node_t *)p);
    struct node_t* r = ((struct node_t *)q);
    return (l->rank - r->rank);
    }



struct localNode_t* newLocalNode(int name){
  struct localNode_t* newNode = (struct localNode_t*) malloc(sizeof(struct localNode_t));
  newNode->name = name;
  newNode->tree = 0;
  newNode->nonTree = 0;
  return newNode;
} ;

struct localTree_t* initLocalTree(node_t* node){
  localTree_t* localTree = (struct localTree_t*) malloc(sizeof(struct localTree_t));
  localTree->list = (struct adjLTList_t*) malloc(sizeof(struct adjLTList_t));
  localTree->list[0].node = newLocalNode(node->name);
  return localTree;
} ;


void tree(localNode_t* a){

} ;

void nonTree(localNode_t* a){

} ;

void updateLT(node_t* localRoot){
  localTree_t* localTree = (struct localTree_t*) malloc(sizeof(struct localTree_t));
  localTree->list = (struct adjLTList_t*) malloc(2*localRoot->n * sizeof(struct adjLTList_t));

  localTree->size = localRoot->size+1;

  //prepare local root for local tree array
  node_t* child = localRoot->children;
  localNode_t* root = newLocalNode(localRoot->name);
  root->rank = localRoot->rank;
  root->parent = NULL;
  localTree->list[0].node = root;
  int j = 1;


  //make new nodes for local tree
  while(child){
    localNode_t* elem = newLocalNode(child->name);
    //elem->rank = child->rank;
    elem->rank = j;
    //printf("%d\n", child->rank);
    localTree->list[j].node = elem;
    j++;
    child = child->sibling;
  }


  adjLTList_t* sortedList = localTree->list;
  printf("hhh %d\n", sortedList[1].node->name);
  //arr = localRoot->children;
  qsort(sortedList, sizeof(sortedList)/sizeof(*sortedList), sizeof(*sortedList), comp);
  int i;
  int size = localRoot->size;
  //printf("Size %d\n", localRoot->size);

  //pair nodes from sorted list with the same rank r
  for(i = 0; i<localRoot->size+1;i++){
    printf("%d have rank %d\n", sortedList[i].node->name, sortedList[i].node->rank);
    //if i and i+1 are not equal, then set i-1

    //otherwise pair the nodes by making a new parent node with rank r+1
  }


  //make a path from local root

};



// replace
void replace(graph_t graph){

};

// merging nodes when running edge delete
void merge(){

};

// search for connection
void search(){

};
