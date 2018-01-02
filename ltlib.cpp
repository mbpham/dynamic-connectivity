#include <iostream>
#include <string>
#include <sstream>
using namespace std;
#include <math.h>
#include "lib.hpp"

/* --------- LOCAL TREE ---------*/
// building local tree

int comp(const void *p, const void *q){
    struct localNode_t* l = ((struct localNode_t *)p);
    struct localNode_t* r = ((struct localNode_t *)q);
    //printf("Compare left: %d, right: %d\n", l->rank, r->rank);
    return (l->rank - r->rank);
}

struct localNode_t* newLocalNode(int name){
  struct localNode_t* newNode = (struct localNode_t*) malloc(sizeof(struct localNode_t));
  newNode->name = name;
  newNode->pNode = 1;
  return newNode;
} ;

struct localTree_t* initLocalTree(node_t* node){
  //allocate memory for local tree
  localTree_t* localTree = (struct localTree_t*) malloc(sizeof(struct localTree_t));
  localTree->list = (struct adjLTList_t*) malloc(10* sizeof(struct adjLTList_t));

  //make a local node that represents the structural forest node, a.
  localNode_t* newNode = newLocalNode(node->name);
  newNode->rank = node->rank;
  newNode->belongsTo = node;
  localTree->list[0].node = newNode;
  localTree->size = 1;
  localTree->root = localTree->list[0].node;
  localTree->pNodes = 0;


  //check if there are any children
  //initializing state: nodes have at most one child
  if(node->children != NULL){
    localNode_t* newChild = newLocalNode(node->name);
    newChild->belongsTo = node->children;
    newChild->rank = node->children->rank;
    newNode->right = newChild;
    newChild->pNode = 1;
    localTree->list[1].node = newChild;
    localTree->size++;
    localTree->pNodes++;
  }

  //printf("Size of local tree in %d is %d \n", node->name, localTree->size);


  return localTree;
} ;

//called when merging in structural forest
//making a new structural tree with new child as single node and merge
void addLT(localTree_t* a, node_t* b){
  printf("addLT: adding a structural child: %d. \nmaking a new structural tree with only that node\n", b->name);
  localTree_t* localTree = (struct localTree_t*) malloc(sizeof(struct localTree_t));
  localTree->list = (struct adjLTList_t*) malloc(sizeof(struct adjLTList_t));

  localNode_t* newChild = newLocalNode(b->name);
  newChild->belongsTo = b;
  newChild->rank = b->rank;
  newChild->pNode = 1;

  localTree->list[0].node = newChild;
  localTree->size = 1;
  localTree->pNodes = 1;

  mergeLT(a, localTree);
}

void updateLT(localTree_t* Tu){
  printf("\n");
  //prepare localnode* array to sort by taking pNodes only
  int i;
  int j = 0;
  struct localNode_t* arr[Tu->pNodes];
  //printf("\nupdateLT: pNodes of %d: %d\n", Tu->list[0].node->name, Tu->pNodes);
  for(i = 1; i < Tu->size; i++){
    if(Tu->list[i].node->pNode){
      arr[j] = Tu->list[i].node;
      printf("UpdateLT: Inserting %d with rank %d to array\n", arr[j]->name, arr[j]->rank);
      j++;
    }
  }

  //sort array by rank
  qsort(arr, Tu->pNodes, sizeof(struct localNode_t*), comp);

  //pair nodes by rank
  Tu->build = 0;
  while(!(Tu->build)){
    Tu->build = 1;
    pairNodes(Tu, arr);
  }

  //the paring is finished arr
  //arr only contains nodes with unique ranks
  //TODO: buildRankPath
  buildRankPath(Tu, arr);


}

void pairNodes(localTree_t* Tu, localNode_t* arr[]){
  //start from the nodes with largest rank and pair (from right to left)
  printf("\npairNodes: Paring nodes by rank\n");
  int i;
  int size = Tu->pNodes-1;
  for(i = size; i>0; i--){
    //The two nodes have equal ranks, pair.
    if(arr[i]->rank == arr[i-1]->rank){
      printf("pairNodes: paring nodes %d and %d with rank %d\n", arr[i-1]->name, arr[i]->name, arr[i]->rank);
      //make a new node
      localNode_t* newParent = newLocalNode(arr[i]->name);
      newParent->rank = arr[i]->rank +1;
      newParent->left = arr[i];
      newParent->right = arr[i-1];
      newParent->pNode = 1;
      printf("pairNodes: the rank of the new parent is %d\n", newParent->rank);
      //TODO: update bitmaps using bitwise OR
      //newParent->tree = (arr[i]->tree)|(arr[i-1]->tree);
      //newParent->nonTree = (arr[i]->nonTree)|(arr[i-1]->nonTree);

      arr[i]->parent = newParent;
      arr[i]->pNode = 0;

      arr[i-1]->parent = newParent;
      arr[i-1]->pNode = 0;


      Tu->list[Tu->size].node = newParent;
      Tu->size++;
      arr[i-1] = newParent;

      Tu->build = 0;

      Tu->pNodes--;
    }
    else{
      printf("Node's ranks do not match\n");
    }
  }
  //filter out children
  int j, pNodes = 0;
  while(pNodes < Tu->pNodes){
    if(arr[j]->pNode == 1){
      arr[pNodes] = arr[j];
      pNodes++;
    }
    j++;
  }
}

// merging local roots by deleting rank paths and do the same procedure as in updateLT
void mergeLT(localTree_t* Tu, localTree_t* Tv){
  printf("MergeLT: Merging \n");

  //delete rank path
  printf("MergeLT: Deleting rank path nodes\n");
  delRankPath(Tu, Tv);

  //now we will only work with Tu
  //call updateLT to pair nodes and make rank path
  updateLT(Tu);
};

/* --------- BITMAP UPDATES --------- */

void tree(localNode_t* a, int level, int keep){
  //update tree bitmap
  unsigned int bit_position, setOrUnsetBit, ch;
  unsigned char bit_Map_array_index, shift_index;

  bit_position = level;
  setOrUnsetBit = keep;
  bit_Map_array_index = (bit_position-1) / 8;
  shift_index =  (bit_position-1) % 8;
  if(setOrUnsetBit){
    a->tree[bit_Map_array_index] |= 1<<shift_index;
  }
  else{
    a->tree[bit_Map_array_index] &= ~(1<<shift_index);
  }
} ;

void nonTree(localNode_t* a, int level, int keep){
  //update nontree bitmap
  unsigned int bit_position, setOrUnsetBit, ch;
  unsigned char bit_Map_array_index, shift_index;

  bit_position = level;
  setOrUnsetBit = keep;
  bit_Map_array_index = (bit_position-1) / 8;
  shift_index =  (bit_position-1) % 8;
  if(setOrUnsetBit){
    a->nonTree[bit_Map_array_index] |= 1<<shift_index;
  }
  else{
    a->nonTree[bit_Map_array_index] &= ~(1<<shift_index);
  }
} ;

/* --------- RANK PATH UPDATES --------- */

//takes two local trees and deletes the rank path nodes
//and but the local tree node pointer into one array
void delRankPath(localTree_t* Tu, localTree_t* Tv){
  int i;
  for(i = 0 ; i < Tv->size ; i++){
    if(Tv->list[i].node->name != -1){
      Tu->list[Tu->size].node = Tv->list[i].node;
      Tu->size++;
      if(Tv->list[i].node->pNode){
        Tu->pNodes++;
      }
    }
  }
}

void buildRankPath(localTree_t* Tu, localNode_t* arr[]){
  int i;
  //the amount of rank path nodes will be pNodes-1
  //since the last rpn's children both are rank roots
  for(i = 0; i<(Tu->pNodes); i++){
    
  }
}

// search for connection


/*//makes a new local tree for new parent nodes in structural tree
struct localTree_t* makeLT(node_t* localRoot){
  printf("Name of local root is %d\n", localRoot->name);
  localNode_t* newRoot = newLocalNode(localRoot->name);



  newRoot->rank = localRoot->rank;
  localTree_t* localTree = (struct localTree_t*) malloc(sizeof(struct localTree_t));
  localTree->list = (struct adjLTList_t*) malloc(11*sizeof(struct adjLTList_t));
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
    localTree->size = 3;
    localTree->pNodes = 2;
  }

  else{
    printf("Levels are not equal and something is wrong\n");
  }

  return localTree;
}*/

/*void updateSTLT(localTree_t* tree, node_t* localRoot){

  tree->size = localRoot->size+1;
  tree->roots = tree->size;
  int size = tree->roots;

  //prepare local root for local tree array
  node_t* child = localRoot->children;
  int j = 1;

  //make new nodes for local tree
  while(child){
    localNode_t* elem = newLocalNode(child->name);
    elem->rank = child->rank;
    elem->rank = child->rank;
    printf("%d\n", child->rank);
    tree->list[j].node = elem;
    j++;
    child = child->sibling;
  }
  struct localNode_t* arr[size];
  int k;
  for(k = 0; k<size; k++){
    // printf("%d\n", tree->list[k+1].node->name);
      arr[k] = tree->list[k+1].node;
  }

  //sorting array by rank
  qsort(arr, size, sizeof(localNode_t*), comp);
  tree->build = 0;

  //paring nodes
  while (tree->build == 0) {
    tree->build = 1;
    pairNodes(tree, localRoot, arr);
    localNode_t* newArray[tree->roots];

    int i;
    int j = 0;

    for(i=0;i<size;i++){
      if(arr[i]->pNode == 1){
        arr[j] = arr[i];
        j++;
      }
    }
    size = tree->roots;
  }

  localNode_t* rankPath[size];

  //make rank path here using pointers from arr
  for(k = 0; k<size; k++){
    localNode_t* newPnode = newLocalNode(-1);
    newPnode->left = arr[k];
    rankPath[k] = newPnode;
    printf("New node is %d\n", rankPath[k]->name);
    if(k<0){
      rankPath[k-1]->right = rankPath[k];
      rankPath[k]->parent = rankPath[k-1];
    }
    if(k==0){
      rankPath[k]->parent = tree->list[0].node;
      tree->list[0].node->right = rankPath[k];
    }
    if(k==size-2){

    }
  }
  printf("%d\n", tree->list[0].node->right->left->name);

};*/

/*
//pairing nodes by rank and make rank path
void pairNodes(localTree_t* tree, node_t* localRoot, localNode_t* arr[]){
  printf("\nGetting started with the pairing\n");
  int i;
  int size = tree->roots;
  tree->roots = 0;
  //pair nodes from sorted list with the same rank r
  for(i = size-1; i>0;i--){
    printf("PairNodes: Trying to pair %d with rank %d and %d with rank %d\n", arr[i-1]->name, arr[i-1]->rank, arr[i]->name, arr[i]->rank);
    if(((arr[i]->parent->name == -1)&&(arr[i-1]->parent->name == -1))) {
      if(arr[i]->rank == arr[i-1]->rank){
        printf("PairNodes: Pairing nodes %d and %d with rank %d\n", arr[i-1]->name, arr[i]->name, arr[i]->rank);
        tree->roots++;
        tree->size++;

        //making a new parent node
        localNode_t* newParent = newLocalNode(tree->size);
        newParent->rank = arr[i]->rank +1;
        newParent->pNode = 1;
        newParent->parent = arr[i]->parent;
        arr[i]->pNode = 0;
        arr[i-1]->pNode = 0;


        //update parents and children
        newParent->left = arr[i-1];
        newParent->right = arr[i];
        arr[i]->parent = newParent;
        arr[i-1]->parent = newParent;

        //place the new parent in local tree list
        tree->list[tree->size-1].node = newParent;
        //newParent->nonTree = (tree->list[tree->size-1].node->nonTree)|(tree->list[tree->size].node->nonTree);
        //printf("Nontree bitmap %c\n", newParent->nonTree);
        //remove the children from array and keep parent
        arr[i] = newParent;
        printf("PairNodes: new node is %d and put into index %d\n", arr[i]->name, i);

        i--;
        if(size > 3){
          tree->build = 0;
          }
        }
        else{
          printf("PairNodes: Nodes %d and %d do not have the same ranks: %d and %d\n", arr[i-1]->name, arr[i]->name, arr[i-1]->rank, arr[i]->rank);
          arr[i-1]->pNode = 1;
          arr[i]->pNode = 1;
          tree->roots++;
        }

        if(i == 1){
          printf("PairNodes: The last node in arr is %d with rank %d and is changed to a pnode \n", arr[i-1]->name, arr[i-1]->rank);
          tree->roots++;
          arr[i-1]->pNode = 1;
        }
      }
      else{
        printf("PairNodes: %d and %d are not roots of their subtree\n", arr[i-1]->name, arr[i]->name);
      }
    }
} ;
*/


/*
void updateLT(localTree_t* tree, node_t* localRoot){
  struct localNode_t* arr[tree->pNodes];
  int k;
  int q = 0;

  for(k = 1; k<tree->size; k++){
    if(tree->list[k].node->parent->name == -1){
      arr[q] = tree->list[k].node;
      printf("UpdateLT: %d inserted to array in index %d\n", arr[q]->name, q);
      q++;
      tree->roots = q;
    }
  }
  int size = q;
  //sorting array by rank
  qsort(arr, size, sizeof(struct localNode_t*), comp);

  //paring nodes
  tree->build = 0;
  while (tree->build == 0) {
    //printf("size %d\n", tree->size);
    //printf("roots %d\n", tree->roots);
    tree->build = 1;
    pairNodes(tree, localRoot, arr);
    localNode_t* newArray[tree->roots];

    int i;
    int j = 0;
    //filter out the ones that are not roots

    printf("\nUpdateLT: Filtering out non root nodes. Size is %d\n", size);

    for(i=0;i<size;i++){
      if(arr[i]->pNode == 1){
        printf("UpdateLT: Put %d into array, i = %d, into j = %d, rank = %d\n", arr[i]->name, i, j, arr[i]->rank);
        arr[j] = arr[i];
        j++;
      }
    }
    size = tree->roots;
  }
  localNode_t* rankPath[tree->size];
  //make rank path here using pointers from arr

  for(k = 0; k<tree->roots; k++){
    localNode_t* newPnode = newLocalNode(-1);
    tree->pNodes++;
    newPnode->left = arr[k];
    rankPath[k] = newPnode;
    if(k==0){
      arr[k]->parent = rankPath[k];
      rankPath[k]->left = arr[k];
      //printf("child to path node %d\n", arr[k]->name);
    }
    else{
      rankPath[k-1]->parent = rankPath[k];
      rankPath[k]->right = rankPath[k-1];
      rankPath[k]->left = arr[k];
      arr[k]->parent = rankPath[k];
    }
    if(k==tree->roots-1){
      rankPath[k]->parent = tree->list[0].node;
      tree->list[0].node->right = rankPath[k];
      //printf("child to path node %d\n", arr[k]->name);
    }
  }
};
*/


/*TRASH from mergeLT*/

/*

  //Deleting rank path
  if(v->leaf == 1){
    v->localTree->list[0].node->parent = u->localTree->list[u->localTree->size-1].node->parent;
    u->localTree->list[u->localTree->size] = v->localTree->list[0];
    u->size++;
    u->localTree->size++;
    u->localTree->roots++;

  }
  else{
    for(i = 1; i<v->localTree->size; i++){
      if((v->localTree->list[i].node->name != -1) && (v->localTree->list[i].node->parent->name == -1)){
          printf("MergeLT: Putting %d into u list\n", v->localTree->list[i].node->name);
          u->localTree->list[u->localTree->size] = v->localTree->list[i];
          u->localTree->size++;
          u->localTree->roots++;
      }
    }
  }
  u->localTree->roots = u->localTree->size-1;
  u->localTree->pNodes = u->localTree->pNodes + v->localTree->pNodes;

  for(i = 1; i<u->localTree->size;i++){
    printf("MergeLT: Node: %d, Rank: %d Parent: %d\n", u->localTree->list[i].node->name, u->localTree->list[i].node->rank, u->localTree->list[i].node->parent->name);
  }
  updateLT(u->localTree, u);*/
