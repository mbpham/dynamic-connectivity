#include <iostream>
#include <string>
#include <sstream>
using namespace std;
#include <math.h>
#include "lib.hpp"

/* --------- LOCAL TREE INITIALIZING AND HELPER FUNCTIONS ---------*/

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

// takes a sorted list an pair by rank from greatest to smallest
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
      printf("pairNodes: the rank of the new parent, %d, is %d\n", newParent->name, newParent->rank);

      //TODO: update bitmaps using bitwise OR
      unsigned int bit_position;
      printf("\npairNodes: the tree edge bitmap: ");
      for(bit_position = 0; bit_position<(BITMAP_LEN*BYTE_LEN); bit_position++){
        unsigned char bit_Map_array_index = (bit_position-1) / 8;
        newParent->tree[bit_Map_array_index] = arr[i]->tree[bit_Map_array_index]|arr[i-1]->tree[bit_Map_array_index];
        printf("%d", newParent->tree[bit_Map_array_index]);
      }
      printf("\n");

      printf("pairNodes: the non-tree edge bitmap: ");
      for(bit_position = 0; bit_position<(BITMAP_LEN*BYTE_LEN); bit_position++){
        unsigned char bit_Map_array_index = (bit_position-1) / 8;
        newParent->nonTree[bit_Map_array_index] = arr[i]->nonTree[bit_Map_array_index]|arr[i-1]->nonTree[bit_Map_array_index];
        printf("%d", newParent->nonTree[bit_Map_array_index]);
      }
      printf("\n");

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

  printf("pairNodes: Filter out children. Rank path roots: %d\n", Tu->pNodes);
  //filter out children
  int j = 0;
  int pNodes = 0;

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

// updates a local tree: sort, pair and builds rank path
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
    printf("\nUpdateLT: Paired succesfully\n");
  }


  //the paring is finished arr
  //arr only contains nodes with unique ranks
  buildRankPath(Tu, arr);


}

/* --------- UPDATES AFTER INSERTION OR DELETION --------- */

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

//remove a child b from a
void delLT(){}

/* --------- BITMAP UPDATES --------- */

void tree(localNode_t* a, int level, int keep){
  //update tree bitmap
  unsigned int bit_position, setOrUnsetBit, ch;
  unsigned char bit_Map_array_index, shift_index;

  bit_position = level;
  setOrUnsetBit = keep;

  bit_Map_array_index = (bit_position) / 8;
  shift_index =  (bit_position) % 8;
  printf("shift_index: %d\n", shift_index);
  if(setOrUnsetBit){
    a->tree[bit_Map_array_index] |= 1<<(7-shift_index);
  }
  else{
    a->tree[bit_Map_array_index] &= ~(1<<(7-shift_index));
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
  printf("\nbuildRankPath: Building rank path\n");
  int i;

  if(Tu->pNodes == 1){
    printf("buildRankPath: there is only one rank path node %d with rank %d\n", arr[0]->name, arr[0]->rank);
    printf("buildRankPath: let it be the right child of the root: %d\n", Tu->list[0].node->name);
    arr[0]->parent = Tu->list[0].node;
    Tu->list[0].node->right = arr[0];
  }
  else{
    //the amount of rank path nodes will be pNodes-1
    //since the last rpn's children both are rank roots
    printf("buildRankPath: there are more than one rank path root\n");
    printf("buildRankPath: give %d and %d a new parent, that is a rank path node\n", arr[0]->name, arr[1]->name);
    localNode_t* RPnode = newLocalNode(-1);
    arr[0]->parent = RPnode;
    arr[1]->parent = RPnode;
    RPnode->right = arr[0];
    RPnode->left = arr[1];
    Tu->list[Tu->size].node = RPnode;
    //TODO: update bitmaps for RPnode

    if(Tu->pNodes == 2){
      RPnode->parent = Tu->list[0].node;
      Tu->list[0].node->right = RPnode;
    }
    else{
      localNode_t* prevRPN = arr[Tu->pNodes];
      for(i = 2; i<(Tu->pNodes)-1; i++){
        localNode_t* newRPN = newLocalNode(-1);
        newRPN->right = prevRPN;
        newRPN->left = arr[i];
        prevRPN->parent = newRPN;
        arr[i]->parent = newRPN;

        Tu->list[(Tu->size)+(i-1)].node = newRPN;

        //TODO: update bitmaps

        prevRPN = newRPN;
      }
      prevRPN->parent = Tu->list[0].node;
      Tu->list[0].node->right = prevRPN;
    }
  }

}

// search for connection

/* --------- PRINTS --------- */

void printBitmap(unsigned char bitmap[]){
  unsigned int bit_position;
  printf("\nprintBitmap: the bitmap: ");
  int i, j;
  for(i = 0; i<BITMAP_LEN ; i++){
    print_byte_as_bits(bitmap[i]);
  }
  printf("\n");

/*

  for(bit_position = 0; bit_position<(BITMAP_LEN*BYTE_LEN); bit_position++){
    unsigned char bit_Map_array_index = (bit_position) / 8;
    printf("%d", bitmap[bit_Map_array_index]);
  }
  printf("\n");*/
}


void print_byte_as_bits(char val) {
  for (int i = 7; 0 <= i; i--) {
    printf("%c", (val & (1 << i)) ? '1' : '0');
  }
}
