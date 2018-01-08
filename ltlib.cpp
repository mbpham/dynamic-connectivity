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
  localTree->list = (struct adjLTList_t*) malloc(100* sizeof(struct adjLTList_t));
  localTree->rankRoots = (struct adjLTList_t*) malloc(100 * sizeof(struct adjLTList_t));

  //make a local node that represents the structural forest node, a.
  localNode_t* newNode = newLocalNode(node->name);
  newNode->rank = node->rank;
  newNode->belongsTo = node;
  localTree->list[0].node = newNode;
  localTree->size = 1;
  localTree->root = newNode;
  localTree->pNodes = 0;

  //check if there are any children
  //initializing state: nodes have at most one child
  if(node->children != NULL){
    localNode_t* newChild = newLocalNode(node->name);
    node->children->parentLeaf = newChild;
    newChild->root = localTree->root;
    newChild->belongsTo = node->children;
    newChild->rank = node->children->rank;
    newNode->right = newChild;
    newChild->pNode = 1;
    localTree->rankRoots[0].node = newChild;
    newChild->parent = newNode;
    localTree->list[1].node = newChild;
    localTree->size++;
    localTree->pNodes++;
    node->children->parentLeaf = newChild;
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
    printf("%d rank %d, %d rank %d\n", arr[i]->name, arr[i]->rank, arr[i-1]->name, arr[i-1]->rank);
    if(arr[i]->rank == arr[i-1]->rank){
      printf("pairNodes: paring nodes %d and %d with rank %d\n", arr[i-1]->name, arr[i]->name, arr[i]->rank);
      //make a new node
      localNode_t* newParent = newLocalNode(arr[i]->name);
      newParent->rank = arr[i]->rank + 1;
      newParent->left = arr[i];
      newParent->root = arr[i]->root;
      newParent->right = arr[i-1];
      newParent->pNode = 1;
      printf("pairNodes: the rank of the new parent, %d, is %d\n", newParent->name, newParent->rank);

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

      newParent->left->pNode = 0;
      newParent->right->pNode = 0;

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
  //delete rank path
  printf("MergeLT: Deleting rank path nodes\n");
  delRankPath(Tu, Tv);

  //now we will only work with Tu
  //call updateLT to pair nodes and make rank path
  updateLT(Tu);
};

// updates a local tree: sort, pair and builds rank path
void updateLT(localTree_t* Tu){

  //prepare localnode* array to sort by taking pNodes only
  struct localNode_t* arr[Tu->pNodes];


  for(int i = 0; i < Tu->pNodes; i++){
    arr[i] = Tu->rankRoots[i].node;
    printf("UpdateLT: Inserting %d with rank %d to array\n", arr[i]->name, arr[i]->rank);

  }

  //sort array by rank
  qsort(arr, Tu->pNodes, sizeof(struct localNode_t*), comp);

  //pair nodes by rank
  Tu->build = 0;
  while(!(Tu->build)){
    Tu->build = 1;
    pairNodes(Tu, arr);
    printf("\nUpdateLT: Paired succesfully, rank roots: %d\n", Tu->pNodes);
  }

  //the paring is finished arr
  //arr only contains nodes with unique ranks
  //printf("%d\n", Tu->pNodes);
  buildRankPath(Tu, arr);
  //printf("%d\n\n", Tu->pNodes);
  //
  for(int i = 0 ; i<Tu->pNodes ; i++){
    //printf("%d\n", Tu->rankRoots[i].node->name);
    Tu->rankRoots[i].node = arr[i];
    Tu->rankRoots[i].node->root = Tu->root;
  }
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
  b->parentLeaf = newChild;

  newChild->rank = b->rank;
  newChild->pNode = 1;

  localTree->root = a->root;
  localTree->list[0].node = newChild;
  localTree->size = 1;
  localTree->pNodes = 1;


  mergeLT(a, localTree);
}

//remove a child b from a
void delLT(localTree_t* a, node_t* b){
  printf("\ndelLT: delete %d from %d local tree\n", b->name, a->root->name);
  //remove b and its path to the rank root
  //printf("delTree: there are %d rank roots\n", a->pNodes);
  adjLTList_t* rankRoots = a->rankRoots;
  localNode_t* Rb = b->parentLeaf;

  if(a->pNodes == 1){
    Rb = rankRoots[0].node;
  }
  else{
    while(Rb->pNode == 0){
      Rb = Rb->parent;
    }
  }
  //find R_b
  //printf("delLT: found rank root for %d: %d, rank %d\n", b->parentLeaf->name, Rb->name, Rb->rank);

  //remove rank root for b from the rank roots array
  int i = 0;
  while(1){
    if(rankRoots[i].node == Rb){
      for(int j = i ; j<a->pNodes ; j++){
        rankRoots[j].node = rankRoots[j+1].node;
      }
      a->pNodes--;
      break;
    }
    i++;
  }

  //printf("delLT: rank roots %d\n", a->pNodes);
  /*for(int j = 0 ; j < a->pNodes ; j++){
    printf("%d rank %d\n", rankRoots[j].node->name, rankRoots[j].node->rank);
  }*/

  //printf("delTree: Calling updateRankRoots with Rb = %d rank %d\n", Rb->name, Rb->rank);

  updateRankRoots(a, Rb, b->parentLeaf);
  //printf("delLT: ranksroots %d\n", a->pNodes);
  /*for(int i = 0 ; i < a->pNodes ; i++){
    printf("%d \n", rankRoots[i].node->name);
  }*/
  updateLT(a);

}

void updateRankRoots(localTree_t* tree, localNode_t* node, localNode_t* rem){
  printf("Calling updateRankRoots with Rb = %d rank %d\n", node->name, node->rank);

  if(!(node->left)&&!(node->right)){
    printf("updateRankRoots: Leaf reached, return\n");
    return;
  }
  if(node->left==rem) {
    printf("updateRankRoots: the left child is %d. Make %d a rank root\n", node->left->name, node->right->name);
    tree->rankRoots[tree->pNodes].node = node->right;
    tree->rankRoots[tree->pNodes].node->pNode = 1;
    tree->pNodes++;
    return;
  }
  else if (node->right==rem){
    printf("updateRankRoots: the right child is %d. Make %d a rank root\n", node->right->name, node->left->name);
    tree->rankRoots[tree->pNodes].node = node->left;
    tree->rankRoots[tree->pNodes].node->pNode = 1;
    tree->pNodes++;
    return;
  }
  else{
    printf("%d %d\n", node->right->name, node->right->pNode);
    printf("%d\n", node->right->parent->right->pNode);
    updateRankRoots(tree, node->left, rem);
    updateRankRoots(tree, node->right, rem);
  }
}


/* --------- BITMAP UPDATES --------- */

void tree(localNode_t* a, int level, int keep){
  //update tree bitmap
  unsigned int bit_position, setOrUnsetBit, ch;
  unsigned char bit_Map_array_index, shift_index;

  bit_position = level;
  setOrUnsetBit = keep;

  bit_Map_array_index = (bit_position) / 8;
  shift_index =  (bit_position) % 8;
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

  bit_Map_array_index = (bit_position) / 8;
  shift_index =  (bit_position) % 8;
  if(setOrUnsetBit){
    a->nonTree[bit_Map_array_index] |= 1<<(7-shift_index);
  }
  else{
    a->nonTree[bit_Map_array_index] &= ~(1<<(7-shift_index));
  }
} ;

//TODO: implementation of bitwise OR for tree and non tree bitmaps
void bitWiseOR(localNode_t* a, localNode_t* b, localNode_t* newNode){
  for(int i = 0; i < BITMAP_LEN; i++){
    newNode->tree[i] = (a->tree[i])|(b->tree[i]);
  }
}
void bitWiseAND(localNode_t* a, localNode_t* b, localNode_t* newNode){
  for(int i = 0; i < BITMAP_LEN; i++){
    newNode->tree[i] = (a->tree[i])&(b->tree[i]);
  }
}

void nonbitWiseOR(localNode_t* a, localNode_t* b, localNode_t* newNode){
  for(int i = 0; i < BITMAP_LEN; i++){
    newNode->nonTree[i] = (a->nonTree[i])|(b->nonTree[i]);
  }
}
void nonbitWiseAND(localNode_t* a, localNode_t* b, localNode_t* newNode){
  for(int i = 0; i < BITMAP_LEN; i++){
    newNode->nonTree[i] = (a->nonTree[i])&(b->nonTree[i]);
  }
}

//update anscestors for the tree or non tree bitmap
//"node" is the index of the leaf in the structural tree
void updateBitmaps(structTree_t* structTree, int node, int level){
  //printf("\nupdateBitmaps: Updating the bitmaps from leaf to root for %d\n", node);

  node_t* currentRoot = structTree->list[node].nodes;

  //printf("Copy tree bitmap from root of %d to its parent's (level %d) leaf\n", node, currentRoot->parent->level);
  localNode_t* prevLocalNode = currentRoot->localTree->root;
  localNode_t* updateNode = currentRoot->parentLeaf;

  updateNode->tree[0] = prevLocalNode->tree[0];
  updateNode->tree[1] = prevLocalNode->tree[1];

  currentRoot = currentRoot->parent;
  //run through the structural forest starting from parent of leaf in structural tree
  while(currentRoot->level != level || currentRoot->level == level){
    printf("\nupdateBitmaps: Run through local node for %d level %d \n", currentRoot->name, currentRoot->level);


    //run through the local tree

    while(updateNode != updateNode->root){
      //printf("test %d, root: %d\n", updateNode->parent->name, updateNode->root->name);
      //printBitmap(updateNode->tree);
      localNode_t* b;
      if(updateNode->parent != updateNode->root){

        //printf("updateBitmaps: The parent of current node has two children\n");
        if(updateNode->parent->left == updateNode){
          //printf("updateBitmaps: the current local node is a left child, %d with rank %d\n", updateNode->name, updateNode->rank);
          b = updateNode->parent->right;
        }
        else{
          //printf("updateBitmaps: the current local node is a right child, %d with rank %d\n", updateNode->name, updateNode->rank);
          b = updateNode->parent->left;
        }
        bitWiseOR(updateNode, b, updateNode->parent);
        //printf("updateBitmaps: The tree bitmap for %d with rank %d is\n", updateNode->parent->name, updateNode->parent->rank);
        //printBitmap(updateNode->parent->tree);

      }
      else{
        //printf("reached the root\n");
        //printf("updateBitmaps: The parent is the root and only has one child\n");
        updateNode->parent->tree[0] = updateNode->tree[0];

        updateNode->parent->tree[1] = updateNode->tree[1];

        //printBitmap(updateNode->tree);
        if(currentRoot->level == level){
          return;
        }
        break;
      }
      updateNode = updateNode->parent;
    }


    if(currentRoot->level == 0){
      break;
    }
    else if(currentRoot->level == level){
      break;
    }


    prevLocalNode = currentRoot->localTree->root;
    //printf("%d\n", currentRoot->level);
    updateNode = currentRoot->parentLeaf;
    //printf("%d\n", updateNode->name);

    updateNode->tree[0] = prevLocalNode->tree[0];
    updateNode->tree[1] = prevLocalNode->tree[1];

    currentRoot = currentRoot->parent;
  }
  //update the roots tree bitmap

  //printf("updateBitmaps: bitmaps successfully updated\n");
}

void updateNonBitmaps(structTree_t* structTree, int node, int level){
  //printf("\nupdateBitmaps: Updating the bitmaps from leaf to root for %d\n", node);

  node_t* currentRoot = structTree->list[node].nodes;

  //printf("Copy tree bitmap from root of %d to its parent's (level %d) leaf\n", node, currentRoot->parent->level);
  localNode_t* prevLocalNode = currentRoot->localTree->root;
  localNode_t* updateNode = currentRoot->parentLeaf;

  updateNode->nonTree[0] = prevLocalNode->nonTree[0];
  updateNode->nonTree[1] = prevLocalNode->nonTree[1];

  currentRoot = currentRoot->parent;
  //run through the structural forest starting from parent of leaf in structural tree
  while(currentRoot->level != level || currentRoot->level == level){
    printf("\nupdatenonBitmaps: Run through local node for %d level %d \n", currentRoot->name, currentRoot->level);
    printf("%d\n", currentRoot->parent->level);

    //run through the local tree

    while(updateNode != updateNode->root){
      printf("test\n");
      //printBitmap(updateNode->tree);
      localNode_t* b;
      if(updateNode->parent != updateNode->root){

        //printf("updateBitmaps: The parent of current node has two children\n");
        if(updateNode->parent->left == updateNode){
          //printf("updateBitmaps: the current local node is a left child, %d with rank %d\n", updateNode->name, updateNode->rank);
          b = updateNode->parent->right;
        }
        else{
          //printf("updateBitmaps: the current local node is a right child, %d with rank %d\n", updateNode->name, updateNode->rank);
          b = updateNode->parent->left;
        }
        bitWiseOR(updateNode, b, updateNode->parent);
        //printf("updateBitmaps: The tree bitmap for %d with rank %d is\n", updateNode->parent->name, updateNode->parent->rank);
        //printBitmap(updateNode->parent->tree);
      }
      else{

        //printf("reached the root\n");
        //printf("updateBitmaps: The parent is the root and only has one child\n");
        updateNode->parent->nonTree[0] = updateNode->nonTree[0];

        updateNode->parent->nonTree[1] = updateNode->nonTree[1];

        if(currentRoot->level == level){

          return;
        }

        break;
      }

      updateNode = updateNode->parent;
    }


    if(currentRoot->level == 0){
      break;
    }
    else if(currentRoot->level == level){
      break;
    }


    prevLocalNode = currentRoot->localTree->root;

    updateNode = currentRoot->parentLeaf;
    printf("%d\n", updateNode->name);
    printf("test1\n");
    updateNode->nonTree[0] = prevLocalNode->nonTree[0];
    updateNode->nonTree[1] = prevLocalNode->nonTree[1];

    currentRoot = currentRoot->parent;
}
}

//Return 1 if there is an edge on level i
int isEdge(unsigned char bitmap[], int level){
  unsigned char bit_Map_array_index, shift_index;
  bit_Map_array_index = (level) / 8;

  shift_index =  (level) % 8;

  if(bitmap[bit_Map_array_index] & (1 << (7-shift_index))) {
    return 1;
  }

  return 0;
}


/* --------- RANK PATH UPDATES --------- */

//takes two local trees and deletes the rank path nodes
//and but the local tree node pointer into one array
void delRankPath(localTree_t* Tu, localTree_t* Tv){
  int i;
  printf("MergeLT: Deleting rank path nodes\n");
  //printf("Size of tv %d\n", Tv->size);
  for(i = 0 ; i < Tv->size ; i++){
    if(Tv->list[i].node->name != -1){
      Tu->list[Tu->size].node = Tv->list[i].node;
      Tu->list[Tu->size].node->root = Tu->root;
      Tu->size++;
      if(Tv->list[i].node->pNode==1){
        Tu->rankRoots[Tu->pNodes].node = Tv->list[i].node;
        Tu->pNodes++;
      }
    }

  }
  //printf("MergeLT: Deleting rank path nodes\n");
}

//TODO: check if building correctly
void buildRankPath(localTree_t* Tu, localNode_t* arr[]){
  printf("\nbuildRankPath: Building rank path\n");
  int i;

  if(Tu->pNodes == 1){
    //printf("buildRankPath: there is only one rank path node %d with rank %d\n", arr[0]->name, arr[0]->rank);
    //printf("buildRankPath: let it be the right child of the root: %d\n", Tu->list[0].node->name);
    arr[0]->parent = Tu->root;
    Tu->root->right = arr[0];
  }
  else{
    //printf("%d\n", Tu->pNodes);
    //the amount of rank path nodes will be pNodes-1
    //since the last rpn's children both are rank roots
    //printf("buildRankPath: there are more than one rank path root\n");
    //printf("buildRankPath: give %d and %d a new parent, that is a rank path node\n", arr[0]->name, arr[1]->name);
    //printf("test\n");
    localNode_t* RPnode = newLocalNode(-1);
    RPnode->root = Tu->root;

    arr[0]->parent = RPnode;
    arr[1]->parent = RPnode;

    RPnode->right = arr[0];
    RPnode->left = arr[1];
    //printf("%d %d %d\n", RPnode->right->name, RPnode->left->name, RPnode->name);
    RPnode->parent = Tu->root;
    Tu->root->right = RPnode;

    //TODO: update bitmaps for RPnode

    if(Tu->pNodes == 2){
      RPnode->parent = Tu->root;
      Tu->root->right = RPnode;
    }
    else{
      localNode_t* prevRPN = RPnode;

      for(int i = 2; i<(Tu->pNodes); i++){
        //printf("buildRankPath: Inserting %d\n", arr[i]->name);
        localNode_t* newRPN = newLocalNode(-1);
        newRPN->right = prevRPN;
        newRPN->left = arr[i];
        prevRPN->parent = newRPN;
        arr[i]->parent = newRPN;
        //Tu->list[(Tu->size)+(i-1)].node = newRPN;
        //TODO: update bitmaps

        prevRPN = newRPN;
      }
      prevRPN->parent = Tu->root;
      prevRPN->root = Tu->root;
      Tu->root->right = prevRPN;
    }
  }

}

// search for connection

/* --------- PRINTS --------- */


void printSiblings(node_t* parent){
  node_t* siblings = parent->children;
  printf("printSiblings: siblings for %d are: ", siblings->name);
  while(siblings){
    printf("%d ", siblings->name);
    siblings = siblings->sibling;
  }
  printf("\n");
}

void printCluster(node_t* node){
  printf("\nThe cluster for %d level %d: ", node->name, node->level);
  for(int i = 0 ; i < node->n ; i++){
    printf("%d ", node->cluster[i].nodes->name);
  }
  printf("\n");
}


void printBitmap(unsigned char bitmap[]){
  unsigned int bit_position;
  printf("\nprintBitmap: the bitmap: ");
  for(int i = 0; i<BITMAP_LEN ; i++){
    byte_as_bits(bitmap[i]);
  }
  printf("\n");
}

void byte_as_bits(char bitmap) {
  for (int i = 7; 0 <= i; i--) {
    printf("%c", (bitmap & (1 << i)) ? '1' : '0');
  }
}
