#include <iostream>
#include <string>
#include <sstream>
using namespace std;
#include "ltlib.cpp"
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
  node->size = 0;
  node->n = 0;
  node->rank = 0;
  //CHANGE: try realloc when adding
  node->cluster = (struct adjTreeList_t*) malloc(10*sizeof(adjTreeList_t));
  node->cluster[0].nodes = node;
  node->cluster->size = 1;
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

    //init local tree
    structTree->list[i].nodes->localTree = initLocalTree(structTree->list[i].nodes);
      }

  structTree->size = graph->size;
  return structTree;
};

//updates structural tree after edge insertion
void addTree(graph_t* graph, int u, int v){
  node_t* rootu = graph->tree->list[u].nodes->root;
  node_t* rootv = graph->tree->list[v].nodes->root;
  if(rootu != rootv){
    if(rootu->leaf == 1 && rootv->leaf == 1){
      printf("Both %d and %d are leaves\n", rootu->name, rootv->name);

      //make a new parent node for u and v
      node_t* node = newNode(graph->tree->size);

      node->sibling = NULL;

      //update cluster
      node->cluster[1].nodes = graph->tree->list[u].nodes;
      node->cluster[2].nodes = graph->tree->list[v].nodes;
      node->cluster->size = 3;

      //put into adj list
      graph->tree->list[graph->tree->size].nodes = node;
      //updates the subtree size
      node->n = node->n + 2;
      node->rank = ceil(log2(node->n));


      //update height
      node->height = graph->tree->list[u].nodes->root->height + 1;

      //update child
      node->children = graph->tree->list[u].nodes;
      node->size = 2;

      //update siblings
      graph->tree->list[u].nodes->sibling = graph->tree->list[v].nodes;
      graph->tree->list[u].nodes->last = graph->tree->list[v].nodes;
      graph->tree->list[v].nodes->sibling = NULL;

      //update levels
      recurseLevel(node, node, 0);

      //update parent
      graph->tree->list[u].nodes->root = graph->tree->list[graph->tree->size].nodes;
      graph->tree->list[v].nodes->root = graph->tree->list[graph->tree->size].nodes;
      graph->tree->list[u].nodes->parent = graph->tree->list[graph->tree->size].nodes;
      graph->tree->list[v].nodes->parent = graph->tree->list[graph->tree->size].nodes;

      //update tree size
      graph->tree->size++;

      graph->tree->list[u].nodes->root->localTree = makeLT(graph->tree->list[u].nodes->root);
    }
    else if(rootu->leaf == 1 && rootv->leaf == 0){
      printf("\nRoot of %d is a leaf\n", u);

      //update cluster
      int clusterSize = graph->tree->list[v].nodes->root->cluster->size;
      graph->tree->list[v].nodes->root->cluster[clusterSize].nodes = graph->tree->list[u].nodes;
      graph->tree->list[v].nodes->root->cluster->size++;

      mergeLT(graph->tree->list[v].nodes->root, graph->tree->list[u].nodes->root);
      //Update subtree size
      graph->tree->list[v].nodes->root->n = rootv->n + 1;
      graph->tree->list[v].nodes->root->rank = ceil(log2(graph->tree->list[v].nodes->root->n));

      //update siblings
      rootv->children->last->sibling = graph->tree->list[u].nodes;
      rootv->children->last = graph->tree->list[u].nodes;
      graph->tree->list[u].nodes->sibling = NULL;

      graph->tree->list[u].nodes->root = graph->tree->list[v].nodes->root;
      graph->tree->list[u].nodes->parent = graph->tree->list[v].nodes->root;

      recurseLevel(rootv, rootv, 0);

      //update children size for root
      (graph->tree->list[v].nodes->root->size) = rootv->size+1;

    }
    else if(rootu->leaf == 0 && rootv->leaf == 1){
      addTree(graph, v, u);
      return;

    }
    else if(rootu->leaf == 0 && rootv->leaf == 0){
      printf("None are leaves\n");

      mergeNodes(graph->tree->list[u].nodes->root, graph->tree->list[v].nodes->root);
      mergeLT(graph->tree->list[u].nodes->root, graph->tree->list[v].nodes->root);
      recurseLevel(graph->tree->list[u].nodes->root->parent, graph->tree->list[u].nodes->root->parent, 0);

      }
    }
    //Update tree(a) bitmap
  else{
    printf("\n%d and %d share the same root\n", u, v);
    //Insert non-tree edge


    //Update non-tree(a) bitmap
  }

  printf("\nValues in cluster\n");
  int k;
  for(k = 0; k<graph->tree->list[v].nodes->root->cluster->size;k++){
    printf("addTree: Cluster value %d\n", graph->tree->list[v].nodes->root->cluster[k].nodes->name);
  }

  //update tree bitmap at level i
  //updateLT(graph->tree->list[u].nodes->localTree, graph->tree->list[u].nodes->root);

} ;

void delTree(graph_t* graph, int u, int v){
  //find the first common local root
  node_t* minLevelNode;
  node_t* firstCommonNode;
  node_t* tv;
  node_t* tw;
  node_t* levelpp1;
  node_t* levelpp2;
  int currentLevel;
  int replacement = 0;
  int i;

  /* ------- ALL THE PREPERATIONS ------- */
  //Find the node with lowest rank
  minLevelNode = findMinLevelNode(graph, u, v);

  //First the first common node, in this case the root
  minLevelNode = minLevelNode->parent;
  while(minLevelNode){
    if(search(minLevelNode->cluster, u) == 1 && search(minLevelNode->cluster, v) == 1){
      printf("\ndelTree: Matching node found: %d at level %d\n", minLevelNode->name, minLevelNode->level);
      break;
    }
    minLevelNode = minLevelNode->parent;
  }
  firstCommonNode = minLevelNode;

  //increase level by 1
  //look at children of minLevelNode
  //find i+1 localroot for u and v
  minLevelNode = minLevelNode->children;
  while(minLevelNode){
    if(search(minLevelNode->cluster, u) == 1){
      levelpp1 = minLevelNode;
    }
    else if(search(minLevelNode->cluster, v) == 1){
      levelpp2 = minLevelNode;
    }
    minLevelNode = minLevelNode->sibling;
  }

  //find the smallest tree
  printf("delTree: Finding smallest tree\n");

  /* ------- CASES: init case or late  ------- */
  //if both clusters are leaves, we know that they are equal,
  //hence we will choose T_v by the one with fewest connections
  if(levelpp1->leaf == 1 && levelpp2->leaf == 1){
    //FIND SMALLEST TREE T_V
    currentLevel = firstCommonNode->level;
    printf("delTree: Both are leaves\n");
    //Choose smallest tree as the one with fewest connections
    if(graph->tree->list[u].size < graph->tree->list[v].size){
      //Smallest "tree" is tv
      tv = levelpp1;
      tw = levelpp2;

    }
    else{
      //Smallest "tree" is tv
      tv = levelpp2;
      tw = levelpp1;
    }

    printf("delTree: Smallest node is %d\n", tv->name);

    //UPDATE TREE EDGES (TV AND TW ARE LEAVES)
    //find replacement edge at level i
    replacement = findReplacement(graph, tv, tw);

    //No replacement is edge is found
    if(replacement == 0){
      noReplacementUpdate(tv, firstCommonNode, graph, currentLevel);
    }
  }

  else{
    //CHANGE: count parallel
    tv = smallest(levelpp1, levelpp2);
  }
}

/* --------- CHECKS AND SEARCHES ---------*/

int findReplacement(graph_t* graph, node_t* tv, node_t* tw){
  //looking at the linked list for tw
  //and see if there is anything from the list that is connected to tv
  vertex_t* twConnections = graph->graphArr[tw->name].vertex;
  while (twConnections) {
    //Look at the connections in the tw connections
    vertex_t* posRepl = graph->graphArr[twConnections->name].vertex;
    while(posRepl){
      //If we find the same node in any of the connections
      //we have found a replacement
      if(posRepl->name == tv->name){
        printf("findReplacement: Replacement found at %d\n", twConnections->name);

        updateNonTree(posRepl->name, twConnections->name, graph);
        return 1;
      }
      posRepl = posRepl->next;
    }
    twConnections = twConnections->next;
  }
  return 0;

}

void noReplacementUpdate(node_t* tv, node_t* firstCommonNode, graph_t* graph, int currentLevel){
  printf("delTree: No replacement found\n");

  //increase level of tv and give it a new parent
  node_t* newParent = newNode(graph->tree->size);
  newParent->children = tv;
  tv->parent = newParent;
  tv->root = newParent;
  //tv->sibling = NULL;
  tv->last = tv;

  //update siblings
  updateSiblings(graph, firstCommonNode, tv, currentLevel);

  //update levels
  recurseLevel(newParent, newParent, 0);

  //update structTree cluster for firstCommonNode

}

struct node_t* findMinLevelNode(graph_t* graph, int u, int v){
  int high = min(graph->tree->list[u].nodes->level, graph->tree->list[v].nodes->level);
  if (graph->tree->list[u].nodes->level == high){
    //printf("Level of %d is %d\n", graph->tree->list[u].nodes->name, high);
    return graph->tree->list[u].nodes;
  }
  else{
    //printf("Level of %d is %d\n", graph->tree->list[v].nodes->name, high);
    return graph->tree->list[v].nodes;
  }
};


int search(adjTreeList_t* cluster, int elem){
  int size = cluster->size;
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

int graphConnected(graph_t* graph, int u, int v){
  vertex_t* nextu = graph->graphArr[u].vertex;
  vertex_t* nextv = graph->graphArr[v].vertex;
  //delete edge from nontree edge set in the other direction
  while(nextu){
    if(nextu->name == v){
      return 1;
    }
    nextu = nextu->next;
  }
  return 0;
}

void updateClusters(adjTreeList_t* pCluster, adjTreeList_t* tvCluster){
  //remove from cluster in p
  int j;
  int c = 0;
  while(1){
    //try to clear the space in p cluster
    if(pCluster[c].nodes->name == tvCluster[j].nodes->name){

      tvCluster[tvCluster->size].nodes = pCluster[c].nodes;
      printf("MATCH %d\n", tvCluster[tvCluster->size].nodes->name);
      //tvCluster->size++;
      int k=c;
      while(k<pCluster->size-1){
        pCluster[k] = pCluster[k+1];
        printf("updateClusters: %d's new descending leaf is %d\n", k, pCluster[k].nodes->name);
        k++;
      }
      pCluster->size--;
      break;
    }
    c++;
  }
}

void updateSiblings(graph_t* graph, node_t* p, node_t* tv, int level){
  node_t* newParent = tv->parent;
  int i;
  int j;
  int isConnectedTV;
  node_t* temp;

  //update siblings in structural forest
  adjTreeList_t* tvCluster = tv->cluster;
  node_t* siblings = p->children;
  node_t* prev;
  //if p is pointing to tv at the first child
  //then simply let p point to its sibling instead
  if(siblings == tv){
    p->children = tv->sibling;
    siblings = siblings->sibling;
  }
  //check for all siblings clusters if anything is connected to tv,
  //that is not connected to anything else
  while(siblings){
    printf("updateSiblings: Current sibling is %d\n", siblings->name);
    adjTreeList_t* clusterVal = siblings->cluster;
    //The siblings cluster
    for(i = 0; i<clusterVal->size;i++){
      printf("updateSiblings: Current clusterVal is %d\n", clusterVal[i].nodes->name);
      //the cluster values in tv
      for(j = 0; j<tvCluster->size;j++){
        isConnectedTV = graphConnected(graph, clusterVal[i].nodes->name, tvCluster[j].nodes->name);
        if(isConnectedTV||(tvCluster[j].nodes->name  == clusterVal[i].nodes->name)){
          printf("updateSiblings: connected, remove as sibling\n");
          temp = siblings;
          prev->sibling = siblings->sibling;
          printf("updateSiblings: %d's new sibling is: %d\n", prev->name, siblings->sibling->name);

          if(!(tvCluster[j].nodes->name  == clusterVal[i].nodes->name)){
            newParent->children->last->sibling = temp;
            newParent->children->last = temp;
            newParent->children->last->sibling = NULL;
          }
          else{
            temp->sibling = NULL;
          }

          updateClusters(p->cluster, tvCluster);

          siblings = prev;
        }
      }
    }

    prev = siblings;
    siblings = siblings->sibling;
  }

  Clusters(newParent);
}

//finds the smallest tree at level
node_t* smallest(node_t* u, node_t* v){
  int uSize = DFSsmallestTree(u, 0);
  int vSize = DFSsmallestTree(v, 0);

  if(uSize < vSize){
    return u;
  }
  else if (vSize < uSize){
    return v;
  }
  else{
    return u;
  }
}

int DFSsmallestTree(node_t* currentRoot, int count){
  node_t* nextSibling = currentRoot;
  while (nextSibling) {
    count++;
    DFSsmallestTree(nextSibling->sibling, count);
    nextSibling = nextSibling->children;
  }
  return count;
}

void recurseLevel(node_t* root, node_t* currentRoot, int level){
  node_t* nextSibling = currentRoot;
  while (nextSibling) {
    recurseLevel(root, nextSibling->sibling, level);
    nextSibling->level = level;
    nextSibling->root = root;
    printf("Root for %d is: %d\n", nextSibling->name, nextSibling->root->name);
    printf("%d's level is: %d\n", nextSibling->name, nextSibling->level);
    nextSibling = nextSibling->children;
    level++;
  }
} ;

void Clusters(struct node_t* node){
  printf("ss\n");
  int i;
  /*struct node_t* child = node->children;
  while(child){
    printf("Node that belongs to %d: %d\n", node->name, child->name);
    child = child->sibling;
  }*/
  printf("%d\n", node->cluster->size);
  for(i = 0; i<node->cluster->size;i++){
    printf("Node that belongs to %d: %d\n", node->name, node->cluster[i].nodes->name);
  }
}

void mergeNodes(node_t* a, node_t* b){
  printf("mergeNodes: Merging %d and %d in %d\n", a->name, b->name, a->name);
  struct node_t* child = b->children;
  //Updates children
  a->children->last->sibling = b->children;
  while(child){
    a->size = a->size +1;
    child->parent = a;
    printf("Parent to %d is %d\n", child->name, a->name);
    child = child->sibling;
  }
  a->n = a->n+ b->n;
  a->rank = floor(log2(a->n));

  //Updates cluster
  int i;
  for(i = 0; i<b->cluster->size;i++){
    a->cluster[a->cluster->size].nodes = b->cluster[i].nodes;
    a->cluster->size++;
  }

  b = NULL;

}
