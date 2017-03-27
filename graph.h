/*===================================;
 *  
 * File: graph.h
 * Content: header file for containing
 * struct and funciton definitions for 
 * the graph data structure. Structure
 * can be eithr directed or undirected
 * , depending on user initialization
 * parameters.
 * Date: 26/03/2016
 *
 ************************************/

#ifndef __GRAPH_H_INCLUDED__
#define __GRAPH_H_INCLUDED__

#include "binary_tree.h"
#include "vector.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct graph
{
  int node_size; // size in bytes of the data type used for node values
  int edge_size; // size in bytes of the data type used for edge values
                 // can be zero if edges do not have values

  struct binary_tree *nodeSet;   // set of all distinct nodes in the graph
  struct binary_tree *nodePairs; // set of all the distinct (node,node,edge)
                                 // triplets within the tree. 
  // Each node.val pointer in the nodePairs binary_tree is a pointer to an
  // allocated memory block of node_size*2 + edge_size bytes that contains
  // in contiguous memory a (node1, node2, edge) triplet.
  // Symantics being that (node1, node2, edge) triplet immplies that node1
  // and node2 are connected via the edge given by edge (or by an edge with 
  // 0 value if edge_size is zero). 
  
  int NoOfNodes; // Number of nodes within the tree
  int NoOfEdges; // Number of edges (equivalent to the number of connected
                 //                  vertices (node,node,edge) in the
                 //                 graph, or the size of the nodePairs tree)  
  
  void *(*compare_node)(void *, void *); // takes the addresses of two
                                        // node objects/values and returns the 
                                        // address of the greater 
                                        // of the two node objects/values
                                        // . If the objects are equal then 
                                        // the null pointer is returned. 
                                        // MUST be implemented

  void *(*compare_triplet)(void *,void *); /* Given the adderesses of two memory
   blocks of equal size that each specify a (node1, node2, edge) triplet,
   this function returns the address of the greater of the two triplets,
   or null if the triplets are equal. Once compare_node has been implemented
   , the following function may be used, where the value
   NODE_SIZE is replaced with the size (in bytes) of each node value/object :

void *(*compare_triplet)(void *trip1, void *trip2) {
  void *cmp = compare_node(trip1, trip2);
  if (cmp==trip1)
    return trip1;
  else if (cmp==trip2)
    retrun trip2;
  else if (!cmp) {
    char *trip1_node2 = (char *)trip1;
    trip1_node2+= NODE_SIZE;         // NODE_SIZE MUST BE REPLACED
                                  with size in bytes of node values/objects
    char *trip2_node2 = (char *)trip2;
    trip2_node2+= NODE_SIZE;        // REPLACE NODE_SIZE HERE ALSO
    cmp = compare_node(trip1_node2, trip2_node2);
    if (cmp==trip1_node2)
      return trip1;
    if (cmp==trip2_node2)
      retrun trip2;
    else if (!cmp)
      return 0;
  }    
}   
  */

  int type; // defines the type of graph data structure:
            //   0 represents undirected,
            //   1 represents directed.
  
  // printing fucntion pointers, may be null during initialization
  // Input arguement is address of node or edge object/value 
  void (*print_node)(void *); 
  void (*print_edge)(void *); 
};


struct graph *init_graph(int node_size, int edge_size, 
                         void *(*compare_node)(void *, void *),
                         void *(*compare_triplet)(void *,void *),
                         void (*print_node)(void *),
                         void (*print_edge)(void *),
                          int type) 
{
  struct graph *newgraph = malloc(sizeof(struct graph));
  newgraph->node_size = node_size;
  newgraph->edge_size = edge_size;
  newgraph->type= type;
  newgraph->NoOfNodes = 0;
  newgraph->NoOfEdges = 0;
  newgraph->nodeSet = init_btree(node_size,compare_node, print_node);
  newgraph->nodePairs = init_btree((2*node_size)+edge_size, compare_triplet, 0);
  newgraph->compare_node = compare_node;
  newgraph->compare_triplet = compare_triplet;
  newgraph->print_node = print_node;
  newgraph->print_edge = print_edge;
  
  return newgraph;
}

// Inserts the node given at address void *node into the graph given ad address
// struct graph * graph. Function returns 1 if insertion is successful (ie if
// the node value given at address node has not already been inserted into the 
// graph), 0 otherwise.
int graph_insert_node(struct graph *graph, void *node)
{
  int init_nodeSet_size = (graph->nodeSet)->NoOfNodes;
  btree_insert(graph->nodeSet, node);
  if ((graph->nodeSet)->NoOfNodes>init_nodeSet_size) {
    ++(graph->NoOfNodes);
    return 1;
  }
  return 0;
}

// isnerts the (un)ordered node pair (node1,node2) into the graph with the 
// edge value given by the address void *edge. Function considers ordering only
// if graph->type = 1 to signify a directed graph. Funciton returns 1 if 
// the edge has been inserted, 0 otherwise. Edge can only be inserted if both
// node values given at the addersses node1 and node2 are already in the graph
int graph_insert_edge(struct graph *graph, void *node1, void *node2, void *edge)
{
  // Chekck to see if both nodes are contained in the graph
  if (!btree_search(graph->nodeSet, node1))
    return 0;
  if (!btree_search(graph->nodeSet, node2))
    return 0;
  // Determine if edge between nodes exists 
  void *triplet1 = malloc((2*(graph->node_size)) + (graph->edge_size));
  memcpy(triplet1, node1, graph->node_size);
  memcpy( (char *)triplet1 + graph->node_size, node2, graph->node_size);
  memcpy( (char *)triplet1 + 2*(graph->node_size), edge, graph->edge_size); 
  if (btree_search(graph->nodePairs, triplet1)) { //error here
    free(triplet1);  //     DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
    return 0;
  }
 
  // if directed, check for (node2, node1, edge) existence
  if ( (!graph->type) ) {
    void *triplet2 = malloc((2*graph->node_size)+graph->edge_size);
    memcpy(triplet2, node2, graph->node_size);
    memcpy( &(((char *)triplet2)[graph->node_size]), node1, graph->node_size);
    memcpy(&(((char *)triplet2)[2*(graph->node_size)]), edge, graph->edge_size);
    if (btree_search(graph->nodePairs, triplet2)) {
      free(triplet1);
      free(triplet2);
      return 0;   
    }
    free(triplet2);
  }
  
  btree_insert(graph->nodePairs, triplet1);
  ++(graph->NoOfEdges);
  free(triplet1);
  return 1; 
}

// test to see if there is an edge from node1 to node2, returns 1 if their
// is , 0 otherwise.
int graph_adjacent(struct graph *graph, void *node1, void *node2)
{
  void *trip1 = malloc( 2*(graph->node_size) + graph->edge_size);
  memcpy(trip1, node1, graph->node_size);
  memcpy(&(((char *)trip1)[graph->node_size]), node2, graph->node_size);
  memset(&(((char *)trip1)[2*(graph->node_size)]),0,graph->edge_size);
  if (btree_search(graph->nodePairs,trip1))
    return 1;
  if (graph->type)
    return 0;
  memcpy(trip1, node2, graph->node_size);
  memcpy(&(((char *)trip1)[graph->node_size]), node1, graph->node_size);
  if (btree_search(graph->nodePairs,trip1))
    return 1;
  return 0;
  free(trip1);
} 

// lists all of the node such that their is an edge from the node given 
// at adderss void *node to a node in the returned list.
// Null pointer is returned if there are no edges going from the node given
// at node or if the node given at address void *nide is not in the graph.
// if returned poitner is non null, the first data type in the array is an int
// specifying the number of nodes in the array
void *graph_neighbours(struct graph *graph, void *node)
{
  // check if the node is contained in the graph
  if (!btree_search(graph->nodeSet, node))
    return 0;
  
  struct node_ *currNode = (graph->nodePairs)->root_node;
  while (currNode->lchild) 
    currNode = currNode->lchild;

  int nodeCount = 0;
  struct vector *nodeContainer = init_vect(0,0,sizeof(void *)); 
  for (int i=0; i<(graph->nodePairs)->NoOfNodes; ++i) {
    void *currTrip = currNode->val; 
    if (graph->compare_node(currTrip,node)==0) {
      char *secondNode = (char *)currTrip;
      secondNode+=graph->node_size; 
      vect_push_back(nodeContainer, &secondNode);
    }
    if (!(graph->type)) {
      char *secondNode = (char *)currTrip;
      secondNode+= graph->node_size;
      if (graph->compare_node(secondNode, node)==0) 
        vect_push_back(nodeContainer, &currTrip);     
    }
    if (i<((graph->nodePairs)->NoOfNodes - 1)) 
      increment(&currNode);
  }
  int array_len = vect_size(nodeContainer);
  if (array_len==0)
    return 0;
  void *nodeArray = malloc(array_len*(graph->node_size) + sizeof(int));
  int *intptr = (int *)nodeArray;
  *intptr = array_len;
  for (int i=0; i<array_len; ++i) {
    void ** nodeAddr_addr = (void **)vect_at(nodeContainer,i);
    char *node_byteArray = (char *)nodeArray;
    memcpy(&(node_byteArray[i*(graph->node_size) + sizeof(int)]), *nodeAddr_addr, 
                                                   graph->node_size);
  }
  free_vect(nodeContainer);
  return nodeArray;
}



// Removes the node given at address void *node. Returns 1 if the niode is 
// removed, 0 of there was no such node in the graph to remove.
// When removign nodes, all corresponding edges are removed also.
int graph_rem_node(struct graph *graph, void *node)
{
  if (!btree_search(graph->nodeSet,node))
    return 0;
  btree_rem(graph->nodeSet, node);
  while (1) {
    struct node_ *currNode = (graph->nodePairs)->root_node;
    while (currNode->lchild)
      currNode = currNode->lchild;
    int pair_removed = 0;
    for (int i=0; i<(graph->nodePairs)->NoOfNodes; ++i) {
      void *node1 = currNode->val;
      char *node_traverse = (char *)(currNode->val);
      node_traverse+=(graph->node_size);
      void *node2 = (void *)( node_traverse );
      if ( (graph->compare_node(node1, node)==0) || 
           (graph->compare_node(node2, node)==0) ) { 
        btree_rem(graph->nodePairs, currNode->val);
        pair_removed = 1;
      }
      if (i<((graph->nodePairs)->NoOfNodes - 1))
        increment(&currNode);
    }
    if (!pair_removed)
      break;
  }
  --(graph->NoOfNodes);
  return 1;
}


// removes the edge between node1 and node2 (node1,node2, edge).
// returns 1 if edge is remoed, returns 0 otherwise (ie if there is no
// such edge to remove)
int graph_rem_edge(struct graph *graph, void *node1, void *node2)
{
  if ( (!btree_search(graph->nodeSet,node1)) ||
       (!btree_search(graph->nodeSet,node2)) )
    return 0;
  struct node_ *currNode = (graph->nodePairs)->root_node;
  while (currNode->lchild)
    currNode = currNode->lchild;
  for (int i=0; i<(graph->nodePairs)->NoOfNodes; ++i) {
    char *currnode1 = currNode->val;
    char *currnode2 = ((char *)currnode1) + graph->node_size;
    if ( (graph->compare_node(currnode1, node1)==0) && 
       (graph->compare_node(currnode2, node2)==0) ) {
      btree_rem(graph->nodePairs, currNode->val);  
      --(graph->NoOfEdges);
      return 1;
    }
    if (!(graph->type)) {
      if ( (graph->compare_node(currnode1,node2)==0) &&
         (graph->compare_node(currnode2,node1)==0) ) {
        btree_rem(graph->nodePairs, currNode->val);
        --(graph->NoOfEdges);
        return 1;
      }
    }
    if (i< ( (graph->nodePairs)->NoOfNodes - 1 ))
      increment(&currNode);
  }
  return 0; 
}

// print_edge and print_node must both be implemented,even if print_edge
// deosnt print anything because edge values are not implemented.
void print_graph(struct graph *graph)
{
  printf("\nList of Nodes contained in the graph"
         " (not necessarily connected)");
  struct node_ *currNode = (graph->nodeSet)->root_node;
  while (currNode->lchild)
    currNode = currNode->lchild; 
  printf("\n");
  for (int i=0; i<(graph->nodeSet)->NoOfNodes; ++i) {
    graph->print_node(currNode->val);
    printf("\n");
    if (i< ((graph->nodeSet)->NoOfNodes - 1) )
      increment(&currNode);
  } 
  printf("\nList of edges: (node1, node2) edge: ");
  currNode = (graph->nodePairs)->root_node;
  while (currNode->lchild)
    currNode = currNode->lchild;
  for (int i=0; i<(graph->nodePairs)->NoOfNodes; ++i) {
    char *node1 = currNode->val;
    char *node2 = node1 + (graph->node_size);
    char *edge = node2 + (graph->node_size);
    printf("\n( ");
    graph->print_node(node1);
    printf(" , ");
    graph->print_node(node2);
    printf(" )    edge: ");
    graph->print_edge(edge);
    if (i< ((graph->nodePairs)->NoOfNodes - 1) )
      increment(&currNode);
  }
}

// release all memory for the graph
void free_graph(struct graph *graph) {
  free_btree(graph->nodeSet);

  free_btree(graph->nodePairs);
  free(graph);
}
#endif
