#include <iostream>
#include <string>
#include <sstream>
using namespace std;
#include "ltlib.cpp"
#include <math.h>

/* --------- STRUCTURAL TREE INITIALIZING AND HELPER FUNCTIONS ---------*/
//Function for making a structural node
struct node_t* newNode(int name){
  struct node_t* node = (struct node_t*) malloc(sizeof(node_t));
  node->name = name;
  node->parent = NULL;
  node->root = node;
  node->sibling = NULL;
  node->last = node;
  node->leaf = 0;
  node->size = 0;
  node->rank = 0;
  //CHANGE: try realloc when adding
  node->cluster = (struct adjTreeList_t*) malloc(10*sizeof(adjTreeList_t));
  return node;
};

// Build the structural tree and local nodes for each node in C
struct structTree_t* initStructTree(graph_t* graph){
  int i, j;

  //allocate memory in the structural tree
  structTree_t* structTree = (struct structTree_t*) malloc(sizeof(struct structTree_t));
  structTree->list = (struct adjTreeList_t*) malloc(graph->size * sizeof(struct adjTreeList_t));

  //initialize structural forest. The leaves of C/vertices of G are represented in structTree->list
  for(i = 0; i< graph->size; i++){
    structTree->list[i].nodes = newNode(i);
    structTree->list[i].nodes->leaf = 1;



    //make parents for the node until the forest depth is reached
    int depth = ceil(log2(graph->size));
    node_t* node = structTree->list[i].nodes;
    for(j = 0; j<depth; j++){
      node->parent = newNode(i);
      node->level = depth - (j+1);
      node->parent->children = node;
      node->localTree = initLocalTree(node);
      if(j == depth-1){
        break;
      }
      node = node->parent;
    }
    //update root for all descending nodes
    node_t* root = node;
    for(j = 0; j<depth; j++){
      node->root = root;
      node->n = 1;
      node->cluster[0].nodes = structTree->list[i].nodes;
      node = node->children;
    }
  }

  structTree->size = graph->size;
  return structTree;
};

struct tv_t* initTv(){
  tv_t* tv = (struct tv_t*) malloc(sizeof(tv_t));
  tv->size = 0;
  tv->arr = (struct adjTreeList_t*) malloc(100* sizeof(adjTreeList_t));
  tv->visited1 = (int*) malloc(50*sizeof(int));
  tv->visited2 = (int*) malloc(50*sizeof(int));
  return tv;
}

/* --------- UPDATES AFTER INSERTION OR DELETION --------- */

//updates structural tree after edge insertion
void addTree(graph_t* graph, int u, int v){
  //updates local tree in mergeNodes
  mergeNodes(graph->tree->list[u].nodes->root, graph->tree->list[v].nodes->root);
  tree(graph->tree->list[u].nodes->localTree->list[0].node, 0, 1);
  tree(graph->tree->list[v].nodes->localTree->list[0].node, 0, 1);

  //Updates tree bitmaps for the path from u and v towards the root of the structural tree
  updateBitmaps(graph->tree, u, 0);
  updateBitmaps(graph->tree, v, 0);
  printf("Root of %d and %d are %d, level %d and %d, level %d\n", u,v,graph->tree->list[u].nodes->root->name, graph->tree->list[u].nodes->root->level, graph->tree->list[v].nodes->root->name, graph->tree->list[v].nodes->root->level);

} ;

//Updates the structural forest after a level i deletion
void delTree(graph_t* graph, int u, int v, int level){
  printf("\ndelTree: Running size procedure on level %d\n", level);
  tv_t* tvp = initTv();
  tv_t* tvu = initTv();

  //Finding size of trees, but we do not run the procedures parallel to each other
  Size(tvp, graph->tree->list[u].nodes, graph->tree->list[v].nodes, level);
  Size(tvu, graph->tree->list[v].nodes, graph->tree->list[u].nodes, level);

  //defining tv
  tv_t* smaller;
  tv_t* larger;

  //Finding smallest tree
  int minSize = min(tvu->size, tvp->size);
  if(tvu->size == minSize){
    smaller = tvu;
    larger = tvp;
  }
  else{
    smaller = tvp;
    larger = tvu;
  }

  printf("delTree: Smaller tree is %d, rooted in level %d\n", smaller->node->name, smaller->node->level);
  node_t* tv = smaller->node;
  node_t* tw = larger->node;

  if(smaller->arrSize == 0){
    printf("Nothing to merge\n");
  }
  else if(smaller->size == 1){
    //TODO: update tree bitmap such that it does not have any tree edges
    int updateIndex = smaller->arr[0].nodes->name;
    localNode_t* singleLeaf = graph->tree->list[updateIndex].nodes->localTree->root;
    tree(singleLeaf, level, 0);
  }
  else{
    //increase all level i edges in smaller tree by 1
    for(int i = 0 ; i < (smaller->size) ; i++){

      vertex_t* vertex = smaller->arr[i].nodes->to;
      //Increasing level of edge in the graph list
      while (vertex) {
        if(vertex->level == level && vertex->nontreeEdge != 1){
          vertex->level++;
        }
        vertex = vertex->next;
      }
      //merging nodes in level i+1
      node_t* mergeNode = findLevelnode(smaller->arr[i].nodes, level+1);

      if(tv!= mergeNode){
        printf("delTree: the mergeNode cluster is");
        //merge tv and mergenode in tv
        mergeNodes(tv, mergeNode);

        //remove mergenode as sibling to tw
        node_t* twSiblings = tw->parent->children;
        if(twSiblings == mergeNode){
          twSiblings->parent->children = twSiblings->sibling;
          twSiblings->sibling = NULL;
        }
        else{
          node_t* prev = twSiblings;
          twSiblings = twSiblings->sibling;
          while(twSiblings){
            if(twSiblings == mergeNode){
              if(twSiblings->sibling){
                prev->sibling = twSiblings->sibling;
                twSiblings->sibling = NULL;
              }
              else{
                prev->sibling = NULL;
                prev->parent->children->last = prev;
                twSiblings->sibling = NULL;
              }
              break;
            }
            twSiblings = twSiblings->sibling;
          }
        }

        //update localtree for tw
        delLT(tw->parent->localTree, mergeNode);
      }
      //update single tree bitmap
      tree(smaller->arr[i].nodes->localTree->root, level, 0);
      tree(smaller->arr[i].nodes->localTree->root, level+1, 1);
      //update all tree bitmaps in tv and to the root

      updateBitmaps(graph->tree, smaller->arr[i].nodes->name, level);

    }
  }

  printSiblings(tv->parent);
  //structural changes
  //search for a replacement for (u,v) on level i
  replacement_t* rep = (struct replacement_t*) malloc(sizeof(replacement_t));
  rep->replacement = 0;
  rep->numReplacements = 0;
  printf("\ndelTree: Looking for a replacement at level %d\n", level);
  findReplacement(graph, u, v, tv, tw, level, rep);

}

/* --------- FIND REPLACEMENT EDGE ---------*/

//Find a replacement non-tree edge in level i, for tvNode
void findReplacement(graph_t* graph, int u, int v, node_t* tvNode, node_t* twNode, int level, replacement_t* rep){
  printf("findReplacement: looking for a replacement edge\n");
  //Check if there is a replacement edge, if so, stop
  printf("findReplacement: the number of descending leaves from %d level %d: %d\n", tvNode->name, tvNode->level, tvNode->n);
  for(int i = 0; i<tvNode->n; i++){
    printf("findReplacement: running checkNonTreeEdges on %d\n", tvNode->cluster[i].nodes->name);
    //Check in the tv cluster whether or not there is a nontree edge
    checkNonTreeEdges(tvNode->cluster[i].nodes, rep, level);
  }


  //If a replacement edge is found, we may or may not have to update the bitmaps
  if(rep->replacement){
    //update tree bitmaps
    printf("\nfindReplacement: replacement (%d, %d) found on level %d, update tree bitmap\n", rep->replacementNodeu, rep->replacementNodev, level);
    tree(graph->tree->list[rep->replacementNodeu].nodes->localTree->root, level, 1);
    updateBitmaps(graph->tree, rep->replacementNodeu, level+1);
    tree(graph->tree->list[rep->replacementNodev].nodes->localTree->root, level, 1);
    updateBitmaps(graph->tree, rep->replacementNodev, level+1);
    tvNode->parent = twNode->parent;
    addLT(twNode->parent->localTree, tvNode);
    //TODO: update local tree

    vertex_t* Gupdatev = graph->tree->list[rep->replacementNodeu].nodes->to;
    vertex_t* Gupdateu = graph->tree->list[rep->replacementNodev].nodes->to;
    while(1){
      if(Gupdateu->name != rep->replacementNodeu){
        Gupdateu = Gupdateu->next;
      }
      else{
        Gupdateu->nontreeEdge = 0;
      }
      if(Gupdatev->name != rep->replacementNodev){
        Gupdatev = Gupdatev->next;
      }
      else{
        Gupdatev->nontreeEdge = 0;
      }
      if(Gupdateu->name == rep->replacementNodeu && Gupdatev->name == rep->replacementNodev){
        break;
      }
    }

    if(rep->numReplacements == 1){
      //update nontree bitmaps
      printf("findReplacement: there was only one replacement. Update nontree bitmap\n");
      printf("%d\n", graph->tree->list[rep->replacementNodev].nodes->localTree->root->name);
      nonTree(graph->tree->list[rep->replacementNodeu].nodes->localTree->root, level, 0);
      nonTree(graph->tree->list[rep->replacementNodev].nodes->localTree->root, level, 0);

    }

    return;
  }

  if(level == 0){
    printf("findReplacement: There are no replacement edges and we have reached level 0\n");
    node_t* newParent = newNode(tvNode->name);
    newParent->cluster = tvNode->cluster;
    newParent->n = tvNode->n;
    newParent->level = tvNode->level-1;
    newParent->children = tvNode;
    newParent->localTree = initLocalTree(newParent);
    tvNode->parent = newParent;
    printSiblings(twNode->parent);

    splitNodes(tvNode, twNode->parent);
    printCluster(tvNode);

    //update roots of leaves in tv
    for(int i = 0 ; i<(tvNode->n) ; i++){
      newParent->cluster[i].nodes->root = newParent;
    }
    return;
  }
  else{
    printf("findReplacement: No replacement found at level %d\nMoving to level %d\n", level, level-1);
    printf("findReplacement: Making a new parent for %d: %d level %d\n", tvNode->name, tvNode->name, tvNode->level-1);

    //make a new parent for tv and make v^i a child of w^i-1
    node_t* newParent = newNode(tvNode->name);
    newParent->level = tvNode->level-1;
    newParent->children = tvNode;
    tvNode->parent = newParent;
    newParent->cluster = tvNode->cluster;
    newParent->n = tvNode->n;
    newParent->parent = twNode->parent;
    twNode->parent->children->last->sibling = newParent;
    twNode->parent->children->last = newParent;

    printCluster(tvNode);
    printCluster(newParent);
    splitNodes(tvNode, twNode->parent);
    for(int j = 0 ; j<newParent->n ; j++){
      newParent->cluster[j].nodes->root = twNode->cluster[0].nodes->root;
    }

    newParent->localTree = initLocalTree(newParent);
    addLT(twNode->parent->parent->localTree, newParent);

    //Update the bitmaps since we have splittet the nodes
    updateBitmaps(graph->tree, u, newParent->level);
    updateBitmaps(graph->tree, v, newParent->level);
    updateNonBitmaps(graph->tree, u, newParent->level);
    updateNonBitmaps(graph->tree, v, newParent->level);
    printf("findReplacement: calling delTree on level %d\n", level-1);
    delTree(graph, u, v, level-1);

  }

}

//Only change vertices nontree value and save values
void checkNonTreeEdges(node_t* tvNode, replacement_t* rep, int level){
  printBitmap(tvNode->localTree->root->nonTree);
  if(isEdge(tvNode->localTree->root->nonTree, level)) {
    printf("\ncheckNonTreeEdges: non tree edge found at level %d from vertex %d\n", level, tvNode->name);
    //if there is a non tree edge, check for its connections
    vertex_t* vertex = tvNode->to;
    while(vertex){
      if(vertex->level == level && vertex->nontreeEdge == 1){
        node_t* xi1 = findLevelnode(vertex->structNode, level+1);
        node_t* yi1 = findLevelnode(tvNode, level+1);
        printf("xi1: %d, level %d\n", xi1->name, xi1->level);
        printf("yi1: %d, level %d\n", yi1->name, yi1->level);

        printf("Roots are %d and %d level %d and %d\n", xi1->name, yi1->name, xi1->level, xi1->level);

        //Check if x^(i+1) != y^(i+1). If so, we have found a replacement
        if(xi1 != yi1){
          printf("checkNonTreeEdges: replacement edge on level %d found\n", level);
          //saving the replacement edge for later updates

          if(!rep->replacement){
            rep->replacementNodeu = tvNode->name;
            rep->replacementNodev = vertex->name;
          }
          rep->replacement = 1;
          rep->numReplacements++;
          printf("Number of replacements %d\n", rep->numReplacements);
          if(rep->numReplacements>1){
            return;
          }
          //if we have found more than one replacement edge,
          //we dont need more information. break out of while loop

        }

        else{
          printf("checkNonTreeEdges: The vertices are already connected and is not a replacement\n");
        }

      }
      vertex = vertex->next;
    }
  }
  else{
    printf("checkNonTreeEdges: There are no non tree edges connected to this vertex: %d\n", tvNode->name);
  }
}

void splitNodes(node_t* tv, node_t* parent){
  printf("\nsplitNodes: there is no replacement at level %d\n", tv->level);

  //updates the cluster for tw->parent since tv is not a child to it
  splitCluster(tv, parent);
  removeSibling(tv, parent);

  if(tv->parent->level > 0){
    tv->parent->parent = parent->parent;
    parent->children->last->sibling = tv->parent;
    parent->children->last = tv->parent;

  }


}

void splitCluster(node_t* tv, node_t* parent){
  printf("\nsplitCluster: splitting the cluster, size of %d level %d cluster is %d\n", tv->name, tv->level, tv->n);
  int count = tv->n;
  int clusterIndex = 0;
  while(count>0){
    //printCluster(tv);
    if(search(tv->cluster, parent->cluster[clusterIndex].nodes->name, tv->n)){
      printf("Found a duplicate. Remove %d from %d, level %d\n", parent->cluster[clusterIndex].nodes->name, parent->name, parent->level);
      //found a value that is in tv. Remove it from parent
      int i = clusterIndex;
      while(parent->cluster[i].nodes != NULL){
        parent->cluster[i].nodes = parent->cluster[i+1].nodes;
        i++;
      }
      count--;
      parent->n--;
      printf("splitCluster: the number of descending leaves in %d, level %d: %d\n", parent->name, parent->level, parent->n);
      clusterIndex--;
    }
    //move to the next index
    clusterIndex++;
  }
}

void removeSibling(node_t* tv, node_t* parent){
  printf("\nremoveSibling: remove %d as sibling to tw\n", tv->name);
  node_t* siblings = parent->children;
  node_t* prev;
  if(siblings == tv){
    printf("removeSibling: removing %d\n", tv->name);
    parent->children = siblings->sibling;
    tv->sibling = NULL;
    return;
  }
  prev = siblings;
  siblings = siblings->sibling;
  while(siblings){
    if(siblings == tv){
      printf("removeSibling: found %d\n", tv->name);
      delLT(parent->localTree, tv);
      if(siblings->sibling){
        prev->sibling = siblings->sibling;
      }
      else{
        prev->sibling = NULL;
      }
      tv->sibling = NULL;

    }
    prev = siblings;
    siblings = siblings->sibling;
  }

}


/* --------- SIZE PRODEDURE ---------*/
//Finds the number of nodes connected to y in F_i \ (x,y)
void Size(tv_t* tv, node_t* x, node_t* y, int level){
  //Seeking for y^(i+1)
  tv->visited1[tv->arrSize] = x->name;
  tv->visited2[tv->arrSize] = y->name;

  node_t* yi1 = y;
  while(yi1->level != level+1){
    yi1 = yi1->parent;
  }
  printf("\nSize: y(i+1) found: %d, level %d\n", yi1->name, yi1->level);

  if(tv->visited == NULL){
    tv->visited = yi1;
    tv->size = tv->size + yi1->n;
  }
  else if(tv->visited != yi1){
    //Update node if they do not share the same root at level+1
    tv->size = tv->size + yi1->n;
  }

  //search down from y^i+1 in local and structural tree using the bitmaps
  structSearchDown(tv, yi1, level);
  tv->node = yi1;

}

//searchDown the structural nodes
void structSearchDown(tv_t* tv, node_t* yi1, int level){
    //printf("structSearchDown: searching in structural nodes, %d\n", yi1->level);
    localNode_t* currentLocal = yi1->localTree->root;

    //Check if the local node rooted in yi1 has an incident tree edge in level i
    if(isEdge(currentLocal->tree, level)){

      //Check if we have reached the leaf of the structural tree
      if(yi1->leaf == 1){
        printf("structSearchDown: found a leaf with level %d tree edge\n", level);
        printf("structSearchDown: add node %d, level %d to arr\n", yi1->name, yi1->level);
        tv->arr[tv->arrSize].nodes = yi1;
        tv->arrSize++;

        //printf("structSearchDown: search for connections\n");
        vertex_t* connections = yi1->to;
        while(connections){
          //printf("structSearchDown: found a connection (%d,%d) \n", yi1->name, connections->name);
          //recursive call to Size for each level i edge !(x,y)
          if(!(visited(tv->visited1, tv->visited2, yi1->name, connections->name, tv->arrSize))) {
            if(connections->level == level && connections->nontreeEdge == 0){
              printf("structSearchDown: edge (%d,%d) has not been visited, call size.\n", yi1->name, connections->name);
              Size(tv, yi1, connections->structNode, level);
            }
            else{
              printf("structSearchDown: edge (%d,%d) is a non-tree edge not a level %d edge\n", yi1->name, connections->name, level);
            }
          }
          else{
            printf("structSearchDown: (%d,%d) already visited\n", yi1->name, connections->name);
          }
          connections = connections->next;
        }
        return;
      }
      //move to its child
      currentLocal = currentLocal->right;

      //While there are still local nodes we search down breadth first
      localSearchDown(tv, currentLocal, level);
    }
    else{
      return ;
    }
}

//search down the local trees
void localSearchDown(tv_t* tv, localNode_t* currentLocal, int level){
  //we are only interested in going further down if there is a tree edge on level i
  if(isEdge(currentLocal->tree, level)){
    //return local nodes corresponding structural node if it is a leaf in the local tree
    if(!(currentLocal->right&&currentLocal->left)){
      structSearchDown(tv, currentLocal->belongsTo, level) ;
    }
    else{
      if(isEdge(currentLocal->right->tree, level)){
        localSearchDown(tv, currentLocal->right, level);
      }
      if(isEdge(currentLocal->left->tree, level)){
        localSearchDown(tv, currentLocal->left, level);
      }
    }
  }
  else{
    return;
  }
}

int visited(int vis1[], int vis2[], int seek1, int seek2, int size){
  for(int i = 0 ; i<size ; i++){
    if(vis1[i] == seek1 && vis2[i] == seek2){
      return 1;
    }
    if(vis1[i] == seek2 && vis2[i] == seek1){
      return 1;
    }
  }
  return 0;
}

/* --------- NODE SEARCHES ---------*/

struct node_t* findLevelnode(node_t* node, int level){
  while(node->level != level){
    node = node->parent;
  }
  return node;
}

//Return 1 if elem is in the cluster, return 0 otherwise
int search(adjTreeList_t* cluster, int elem, int n){
  int size = n;
  int i = 0;
  while (i < size && elem != cluster[i].nodes->name) {
      i++;
  }
  if(i < size){
    return 1;
  }
  else{
    return 0;
  }
}

//find first common node for node u and v
int findUpdateLevel(structTree_t* structTree, int u, int v){
  node_t* node = structTree->list[u].nodes;
  while(node){
    if(search(node->cluster, v, node->n) == 1 && search(node->cluster, u, node->n)){
      return node->level;
    }
    node = node->parent;
  }

  return 0;
}

/* --------- UPDATES ---------*/

//removes nontree edges and add them as tree edges
void updateNonTree(int u, int v, graph_t* graph){
  vertex_t* nextu = graph->graphArr[u].vertex;
  vertex_t* nextv = graph->graphArr[v].vertex;
  //delete edge from nontree edge set in the other direction
  while(nextu){
    if(nextu->name == v){
      printf("delTree: Change (%d, %d) to a tree edge\n", nextu->name, v);
      nextu->nontreeEdge = 0;
      break;
    }
    nextu = nextu->next;
  }
  while(nextv){
    if(nextv->name == u){
      printf("delTree: Change (%d, %d) to a tree edge\n", nextv->name, u);
      nextv->nontreeEdge = 0;
      break;
    }
    nextv = nextv->next;
  }
}

//merge nodes a and b, when adding an edge
void mergeNodes(node_t* a, node_t* b){
  printf("mergeNodes: Merging %d, level %d and %d level %d in %d\n", a->name, a->level, b->name, b->level, a->name);
  struct node_t* child = b->children;

  //make the last child of a be a sibling to first child of b
  a->children->last->sibling = b->children;
  a->children->last = b->children->last;

  //make each child of b be a child of a
  while(child){
    //printf("child of %d, %d\n", b->name, child->name);
    a->size = a->size +1;
    child->parent = a;
    addLT(a->localTree, child);
    child = child->sibling;
  }

  //Updates cluster and cluster size and the root
  for(int i = 0; i<b->n;i++){
    printf("mergeNodes: adding %d to %d's cluster\n", b->cluster[i].nodes->name, a->name);
    a->cluster[a->n].nodes = b->cluster[i].nodes;
    b->cluster[i].nodes->root = a->cluster[a->n-1].nodes->root;
    a->n++;
  }

  //update rank
  a->rank = floor(log2(a->n));
  b = NULL;
  free(b);
}
