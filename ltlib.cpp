#include <iostream>
#include <string>
#include <sstream>
using namespace std;
#include "lib.hpp"
#include <math.h>

/* --------- LOCAL TREE ---------*/
// building local tree

int comp(const void *p, const void *q){
    struct localNode_t* l = ((struct localNode_t *)p);
    struct localNode_t* r = ((struct localNode_t *)q);
    return (l->rank - r->rank);


}

struct localNode_t* newLocalNode(int name){
  struct localNode_t* newNode = (struct localNode_t*) malloc(sizeof(struct localNode_t));
  newNode->name = name;
  newNode->tree = 0;
  newNode->nonTree = 0;
  newNode->pNode = 0;
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


//makes a new local tree for new parent nodes in structural tree
struct localTree_t* makeLT(node_t* localRoot){
  printf("Name of local root is %d\n", localRoot->name);
  localNode_t* newRoot = newLocalNode(localRoot->name);
  newRoot->rank = localRoot->rank;
  localTree_t* localTree = (struct localTree_t*) malloc(sizeof(struct localTree_t));
  localTree->list = (struct adjLTList_t*) malloc(sizeof(struct adjLTList_t));
  localTree->root = newRoot;
  localTree->list[0].node = localTree->root;
  localTree->roots = localRoot->size;

  int i = 1;

  node_t* child = localRoot->children;
  while(child){
    localNode_t* elem = newLocalNode(child->name);
    elem->rank = child->rank;
    localTree->list[i].node = elem;
    i++;
    child = child->sibling;
  }

  if(localTree->list[1].node->rank == localTree->list[2].node->rank){
    localNode_t* newParent = newLocalNode(-1);
    newParent->right = localTree->list[1].node;
    newParent->left = localTree->list[2].node;
    newParent->parent = newRoot;
    newParent->root = newRoot;
    localTree->list[1].node->parent = newParent;
    localTree->list[2].node->parent = newParent;
    localTree->list[1].node->root = newRoot;
    localTree->list[2].node->root = newRoot;
    newRoot->right = newParent;
    localTree->list[localRoot->size+1].node = newParent;
  }

  else{
    printf("Levels are not equal and something is wrong\n");
  }

  return localTree;
}

//updates local tree when adding nodes
void updateLT(localTree_t* tree, node_t* localRoot){
  //tree->list = (struct adjLTList_t*) realloc(2*localRoot->n * sizeof(struct adjLTList_t));

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
  int size = (localRoot->size)+1;
  struct localNode_t* arr[size-1];

  int k;
  for(k = 0; k<size-1; k++){
      arr[k] = tree->list[k+1].node;
  }

  //sorting array by rank
  qsort(arr, (size-1), sizeof(localNode_t*), comp);

  tree->build = 0;

  while (tree->build == 0) {

    tree->build = 1;
    pairNodes(tree, localRoot, arr);
    localNode_t* newArray[tree->roots];

    int i;
    int j = 0;

    for(i=0;i<size-1;i++){
      if(arr[i]->pNode == 1){
        arr[j] = arr[i];
        printf("%d\n", arr[i]->name);
        j++;
      }
    }
    size = tree->roots;
  }


};

void pairNodes(localTree_t* tree, node_t* localRoot, localNode_t* arr[]){
  int i;
  int size = (localRoot->size)+1;
  tree->roots = 0;
  //pair nodes from sorted list with the same rank r
  for(i = 1; i<size;i++){
    printf("current node is %d with rank %d\n", arr[i]->name, arr[i]->rank);
    if(arr[i]->rank == arr[i-1]->rank){
      printf("\nPairing nodes %d and %d with rank %d\n", arr[i-1]->name, arr[i]->name, arr[i]->rank);
      tree->roots++;
      //making a new parent node
      localNode_t* newParent = newLocalNode(localRoot->size+1);
      newParent->rank = arr[i]->rank +1;
      newParent->pNode = 1;

      //update parents and children
      newParent->left = arr[i];
      newParent->right = arr[i-1];
      arr[i]->parent = newParent;
      arr[i-1]->parent = newParent;

      //place the new parent in local tree list
      tree->list[localRoot->size+1].node = newParent;

      //remove the children from array and keep parent
      arr[i] = newParent;
      (localRoot->size)++;
      i++;
      if(size > 4){
        tree->build = 0;
      }
    }
  }
} ;

void tree(localNode_t* a){

} ;

void nonTree(localNode_t* a){

} ;



// merging local roots by deleting rank paths and do the same procedure as in updateLT
void merge(localNode_t* u, localNode_t* v){
  //remove rank path and v

  //sort the nodes by rank

  //pair by ranks

};

// search for connection
void search(){

};
