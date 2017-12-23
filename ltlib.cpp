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
  node->localTree = localTree;
  localTree->root = localTree->list[0].node;
  localTree->size = 1;
  return localTree;
} ;

void tree(localNode_t* a){

} ;

void nonTree(localNode_t* a){

} ;

//makes a new local tree for new parent nodes in structural tree
struct localTree_t* makeLT(node_t* localRoot){
  localNode_t* newRoot = newLocalNode(0);
  localTree_t* localTree = (struct localTree_t*) malloc(sizeof(struct localTree_t));
  localTree->list = (struct adjLTList_t*) malloc(sizeof(struct adjLTList_t));
  localTree->root = newRoot;
  localTree->list[0].node = localTree->root;

  int i = 1;

  node_t* child = localRoot->children;
  while(child){
    localNode_t* elem = newLocalNode(child->name);
    elem->rank = child->rank;
    localTree->list[i].node = elem;
    printf("%d\n", localTree->list[i].node->rank);
    i++;
    child = child->sibling;
  }


  return localTree;
}

//updates local tree when mergin two non-leaves
void updateLT(localTree_t* tree, node_t* localRoot){
  tree->list = (struct adjLTList_t*) malloc(localRoot->n * sizeof(struct adjLTList_t));

  tree->size = localRoot->size+1;

  //prepare local root for local tree array
  node_t* child = localRoot->children;
  int j = 1;

  //make new nodes for local tree
  while(child){
    localNode_t* elem = newLocalNode(child->name);
    //elem->rank = child->rank;
    elem->rank = child->rank;
    //printf("%d\n", child->rank);
    tree->list[j].node = elem;
    j++;
    child = child->sibling;
  }


  qsort(tree, sizeof(tree->list)/sizeof(*tree->list), sizeof(*tree->list), comp);

  int i;
  int size = (localRoot->size)+1;
  printf("Size %d\n", localRoot->size);

  //pair nodes from sorted list with the same rank r
  for(i = 1; i<size;i++){
    printf("s\n");
    //printf("%d have rank %d %d\n", tree->list[i].node->name, tree->list[i].node->rank,tree->list[i-1].node->rank);

    //pairing
    if(tree->list[i].node->rank == tree->list[i-1].node->rank){
      printf("\nPairing nodes %d and %d\n", tree->list[i].node->name, tree->list[i-1].node->name);

      //making a parent node for matching ranks
      localNode_t* newNode = newLocalNode(localRoot->size+1);
      newNode->rank = tree->list[i].node->rank + 1;

      (localRoot->size)++;
      i++;
      tree->build = 0;
      printf("Rank of new node %d\n", newNode->rank);
    }

    //otherwise pair the nodes by making a new parent node with rank r+1
  }


  //make a path from local root

};


// merging local roots by deleting rank paths and do the same procedure as in updateLT
void merge(localNode_t* u, localNode_t* v){
  //remove rank path and v

  //sort the nodes by rank

  //pair by ranks

};

// search for connection
void search(){

};
