/*========================================;
 *
 * File: binary tree.c
 * Content: binary tree generice data type
 * container. Support only for one data type
 * to be stored throughout the tree.
 * self balancing insertion and deletion
 * ensures optimum search.
 * Date: 2/11/2016
 *
 ******************************************/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

struct node_ {

  // pointer to node value
  void *val;

  // pointers to associated nodes
  struct node_ *parent;
  struct node_ *rchild;
  struct node_ *lchild;

};
// 

struct binary_tree {

  // pointer to root nodei
  struct node_ *root_node;

  /* pointer to a function that takes the addresses of two instantiated objects
  of the data type that each node points to with its val member. The function
  returns the address of the object that is greater. If both objects are
  considered equal, the null pointer is returned. */
  void *(*compare) (void *v1, void *v2);

  // pointer to function that takes as a parameter a pointer to an instantiated object
  // of the data type pointed to by each nodes val member. Function prints the object
  // pointed to by its parameter to the terminal.
  void (*print) (void *);

  // size in bytes of the object stored at each node
  int data_size;

  // Number of total nodes in binary tree
  int NoOfNodes;
};

// returns 2^exponent, where exponent
// is a non-negative integer
static int pow_2(int exponent)
{
  if (!exponent)
    return 1;
  else {
    int val = 2;
    for (int i=0; i<exponent-1; ++i)
      val*=2;
    return val;
  }
}
// returns the logarithm (base 2) of x
static double log_2(double x)
{
  if (fabs(x-1.0)<1e-14)
    return 0;
  else {
    double numerator = log(x), denominator = log(2.0);
    return numerator/denominator;
  }
}
// convertes the nonnegative integer into its corresponding binary
// digitstring of '1's and '0's, least significant bit is at index 0.
// returned array is dynamically allocted.
static unsigned char *int_to_bin(int integer)
{
  int len;
  if (integer>0) 
    len = floor(log_2(integer))+2;
  else {
    unsigned char *zero = malloc(2);
    zero[0] = '0';
    zero[1] = 0;
    return zero;
  }
  unsigned char *bitstring = malloc(len);
  int quotient = integer, remainder=0, index=0;
  while (quotient!=0) {
    bitstring[index++] = 0x30 + (quotient % 2);
    quotient = quotient/2;
  }
  bitstring[index]=0;
  return bitstring;
}
// converts the nonnegative integer val into its correpsonding binary
// digiststring of '1's and '0's. The returned digitstring is null terminated
// , dynamically allocated, and null terminated, with its least significant
// bit occuring at index 0.
static unsigned char *int_to_bin_fixed_length(int val, int len)
{
  unsigned char *bitstring = malloc(len+1);
  bitstring[len]=0;
  for (int i=0; i<len; ++i)
    bitstring[i]=0x30;
  int index=0, quotient=val, remainder=0;
  while (quotient!=0) {
    bitstring[index++] = 0x30 + (quotient % 2);
    quotient = quotient/2;
  }
  return bitstring;
}
// string defines a null terminated binary digitstring (of '0's and '1's).
// this function decrements the digitstring by 1, whereby the digitstring
// is to have its most significant bit at index 0.
static void decrement_bitstring(unsigned char *string)
{ 
  int len=0;
  while (string[len]!=0) 
    ++len;
  --len;
  int index=0;
  while (len>=0) {
    if (string[len]=='1') {
      string[len]='0';
      return;
    }
    else string[len] = '1';
    --len;
  }
}

// Node struct used to hold values within a binary_tree struct
/*struct node_ {

  // pointer to node value
  void *val;

  // pointers to associated nodes
  struct node_ *parent;
  struct node_ *rchild;
  struct node_ *lchild;

}; */
// 

/*struct binary_tree {

  // pointer to root nodei
  struct node_ *root_node;

  // pointer to a function that takes the addresses of two instantiated objects
  //of the data type that each node points to with its val member. The function
  //returns the address of the object that is greater. If both objects are
  //considered equal, the null pointer is returned. 
  void *(*compare) (void *v1, void *v2);

  // pointer to function that takes as a parameter a pointer to an instantiated object
  // of the data type pointed to by each nodes val member. Function prints the object
  // pointed to by its parameter to the terminal.
  void (*print) (void *);

  // size in bytes of the object stored at each node
  int data_size;

  // Number of total nodes in binary tree
  int NoOfNodes;
};*/

// pos is the first character of a char array of having length len.
// the array has values 0 and 1, or '0' and '1', corresponding to left and right branches.
// the array defines a position in the binary tree. with pos[0] giving the
// direction to branch from the root node. len must be greater than or equal to 1.
// returns 0 if node already exists there, 1 if no node exists and the position is 
// available for insertion.
static int is_available(struct binary_tree *tree, unsigned char *pos, int len)
{
  struct node_ *currNode = tree->root_node;
  int index=0;
  while ( index<len-1 ) {
    if ((pos[index]==0) || (pos[index]=='0'))
      currNode = currNode->lchild;
    else 
      currNode = currNode->rchild;
    ++index;
  }
  if ((pos[index]==0) || (pos[index]=='0')) {
    if (currNode->lchild)
      return 0; // position is already taken up
    else return 1; // no node in position pos
  } 
  else if ((pos[index]==1) || (pos[index]=='1')) {
    if (currNode->rchild)
      return 0; // no poition available
    else return 1; // position available
  }
}

// incremembt pos so that it defines the next position at the [zero starting] depth
// len. positions increment from left to right along the tree depth len.
// pos[0] indicates branch left (0) or right (1) from root node.
// len must be 1 or greater. 
static void increment_pos(unsigned char *pos, int len)
{
  unsigned char carry = 0;
  for (int i=len-1; i>=0; --i) {
    if (pos[i]==0) {
      pos[i]=1;
      return;
    }
    else {
      pos[i] = 0;
      carry = 1;
    }
  }
  if (carry==1) { // should not really rely on this behaviour.
    printf("\nincrement position overflow! setting pos to zero");
  }
}

// pos[0], pos[1],...,pos[index] specifies tree node. pos is unsigned char array of length len.
// this function returns the number of nodes in the tree to the right of the specified node that have
// a position (initial) substring of {pos[0],..,pos[index]}. That is this function returns 1 plus the number
// of nodes that decend from the specified node defined by pos that are also greater than (to the right of)
// the pos node. 
static int right_hand_nodecount_inclusive(struct binary_tree *tree, 
                                          unsigned char *pos, 
                                          int index, int depth)
{
  int NoOfNodes = 0; 
  struct node_ *currNode = tree->root_node;
  int i=0;
  while (i<=index) {
    if ( (pos[i]==0) || (pos[i]=='l') )
      currNode = currNode->lchild;
    else currNode = currNode->rchild;
    ++i;
  }
  if (currNode) {
  ++NoOfNodes;

  if ((currNode = (currNode->rchild))) {
    ++NoOfNodes;
    int len = depth-2-index;
    if (len>0) {
      for (int j=1; j<=len; ++j) {
        for (int i=0; i<pow_2(j); ++i) {
          struct node_ *tempNode_track = currNode;
          unsigned char *bitstring = int_to_bin_fixed_length(i,j);
          int index = 0;
          int existence = 1; // assume node existence
          while (bitstring[index] !=0 ) {
            if (bitstring[index++]==0x31) {
              if (tempNode_track->rchild) 
                tempNode_track = tempNode_track->rchild;
              else {
                existence = 0;
                break;
              }
            }
            else {
              if (tempNode_track->lchild) 
                tempNode_track = tempNode_track->lchild;
              else {
                existence = 0;
                break;
              }
            }
          }
          if (existence)
            ++NoOfNodes;
          free(bitstring);
        }
      }
    }
  }

  }
  
  return NoOfNodes;
}
// decend and less than (to the left of) pos, plus pos (inclusive).
static int left_hand_node_count_inclusive(struct binary_tree *tree,  
                                          unsigned char *pos, 
                                          int index, int depth)
{
  int NoOfNodes = 0; 
  struct node_ *currNode = tree->root_node;
  int i=0;
  while (i<=index) {
    if ((pos[i]==0) || (pos[i]=='l'))
      currNode = currNode->lchild;
    else currNode = currNode->rchild;
    ++i;
  }
  if (currNode) {
  ++NoOfNodes;
  if ((currNode = (currNode->lchild))) {
    ++NoOfNodes;
    int len = depth-2-index;
    if (len>0) {
      for (int j=1; j<=len; ++j) {
        for (int i=0; i<pow_2(j); ++i) {
          struct node_ *tempNode_track = currNode;
          unsigned char *bitstring = int_to_bin_fixed_length(i, j);
          int index = 0;
          int existence = 1; // assume node existence
          while (bitstring[index] !=0 ) {
            if (bitstring[index++]==0x31) {
              if (tempNode_track->rchild)
                tempNode_track = tempNode_track->rchild;
              else {
                existence = 0;
                break;
              }
            }
            else {
              if (tempNode_track->lchild)
                tempNode_track = tempNode_track->lchild;
              else {
                existence = 0;
                break;
              }
            }
          }
          if (existence)
            ++NoOfNodes;
          free(bitstring);
        }
      }
    }
  }
  }
  return NoOfNodes;
}

// spaceloc defines an array of length len that defines the position in the tree
// at (0 starting) depth len where there is space for a new node. parloc is an 
// array of length len that defines the position of the parent node (currently) 
// in the tree, which is located in the tree at (zero starting) depth len. 
// This function returns the maximum number of nodes (including the new node to be inserted)
// that need to be relocated to fit the new node in whilst preserving tree depth.
static int calc_NoOfNodes(struct binary_tree *tree, 
                          unsigned char *spaceloc, 
                          unsigned char *parloc, 
                          int depth)
{
  int currDepth=0, NoOfNodes=0;
  while ( ((spaceloc[currDepth]==0) && (parloc[currDepth]=='l')) || 
          ((spaceloc[currDepth]==1) && (parloc[currDepth]=='r')) )  
    ++currDepth;
  // spaceloc diverged left
  if ((spaceloc[currDepth]==0) && (parloc[currDepth]=='r')) {
    NoOfNodes=0;
    int lowest_val=1, highest_val=1;
    while ( (currDepth+lowest_val) < depth ) {
      if (spaceloc[currDepth+lowest_val]==1)
      break;
      ++lowest_val;
    } 
    for (int i=lowest_val-1; i>=0; --i) 
      NoOfNodes += right_hand_nodecount_inclusive(tree, 
                                                  &(spaceloc[0]), 
                                                  currDepth+i, 
                                                  depth );
    
    while ( (currDepth+highest_val)<depth) { 
      if (parloc[currDepth+highest_val]=='l')
        break;
      ++highest_val;
    } 
    for (int i=highest_val-1; i>=0; --i) 
      NoOfNodes+=left_hand_node_count_inclusive(tree,  
                                                &(parloc[0]), 
                                                currDepth+i, 
                                                depth );
    
  } // parentloc diverged left
  else {
    NoOfNodes=0;
    int lowest_val=1, highest_val=1;
    while ( (currDepth+highest_val)<depth ) {
      if (spaceloc[currDepth+highest_val]==0)
        break;
      ++highest_val;
    }
    for (int i=highest_val-1; i>=0; --i) 
      NoOfNodes+= left_hand_node_count_inclusive(tree, 
                                                 &(spaceloc[0]), 
                                                 currDepth+i, 
                                                 depth );
    
    while ( (currDepth+lowest_val)<depth ) {
      if (parloc[currDepth+lowest_val]=='r')
        break;
      ++lowest_val;
    }
    for (int i=lowest_val-1; i>=0; --i) 
      NoOfNodes+= right_hand_nodecount_inclusive(tree,               // was a call left_hand_node_count_inclusive
                                                &(parloc[0]), 
                                                currDepth+i, 
                                                depth );
    
  }
  return NoOfNodes+2; // 2, 1 for joining top node, 1 for new node.
}

// function to check if the node object located at currNode matches the nodepath 
// given by max_nodepath (which is an array of '1' and '0' specifying right and left branches).
// returns 1 for a match, 0 otherwise.
static int is_max_node(struct binary_tree *tree, 
                       struct node_ *currNode, 
                       unsigned char *max_nodepath)
{
  struct node_ *track_node = tree->root_node;
  int index =0;
  while (max_nodepath[index]!=0) {
    if (max_nodepath[index]=='0')
      track_node = track_node->lchild;
    else
      track_node = track_node->rchild;
    ++index;
  }   
  if (track_node == currNode)
    return 1;
  else return 0;
}

// increments the currNode, when currNode is guaranteed to be incrementable.
// *currNode upon function return points to the node in the tree
// with the next highest node value.
static void increment(struct node_ **currNode)
{
  if ((*currNode)->rchild) {
    (*currNode) = (*currNode)->rchild;
    while ((*currNode)->lchild) 
      (*currNode) = (*currNode)->lchild;
    return;
  }
  else {
    while ( ((*currNode)->parent)->rchild == *currNode)
      (*currNode) = (*currNode)->parent;
    (*currNode) = (*currNode)->parent;
    return;
  }
}

// fill the array of *(node->val) objects with the objects that are contained within each node in the tree
// (including the node to be inserted) between spaceloc and parloc.
// min and max nodepath_addr are addresses to store pointers that point to arrays that
// define the maximum node and minimum node in the tree between spaceloc and parloc (inclusive)
static void fill_ordered_array(struct binary_tree *tree, // tree
                               unsigned char *value_array,  // array to be filled (pre allocated)
                               int total_nodes,             // length of value_array (in objects)
                               unsigned char *spaceloc,     // array (of 0 and 1s) specifying location of a node to be inserted
                               unsigned char *parloc,       // array of 'l' and 'r's specifying location of parent node.
                               int depth,                   // length of spaceloc and parloc
                               unsigned char *value_ptr,     // pointer to the object/value to be inserted
                               unsigned char **min_nodepath_addr, 
                               unsigned char **max_nodepath_addr) 
{
  struct node_ *currNode = tree->root_node;
  unsigned char *max_nodepath; 
  unsigned char *min_nodepath; 
  int index=0, diverge=0;  
  while ( ((spaceloc[index]==0) && (parloc[index]=='l')) ||
          ((spaceloc[index]==1) && (parloc[index]=='r')) ) 
    ++index;
  diverge=index;
    // spaceloc diverges to the left.
  if (spaceloc[index]==0) {
    while (++index<depth) {
      if (spaceloc[index]==1)  
        break;  
    }
    min_nodepath = malloc(index+1);
    min_nodepath[index]=0; 
    for (int i=0; i<index; ++i) { 
      if (spaceloc[i]==0) 
        min_nodepath[i] = '0';
      else min_nodepath[i] = '1';
    }
    index = diverge;
    while (++index<depth) {
     if (parloc[index]=='l')
       break;
    }
    max_nodepath = malloc(index+1);
    max_nodepath[index] =0;
    for (int i=0; i<index; ++i) {
      if (parloc[i]=='l')
        max_nodepath[i] = '0';
      else max_nodepath[i] = '1';
    }
  }// spaceloc diverges to the right
  else {
    while (++index<depth) {
      if (parloc[index]=='r') 
        break;
    }
    min_nodepath =  malloc(index+1);
    min_nodepath[index]=0;
    for (int i=0; i<index; ++i) {
      if (parloc[i]=='l')
        min_nodepath[i] = '0';
      else min_nodepath[i] = '1';
    }
    index = diverge;
    while  (++index<depth) {
      if (spaceloc[index]==0)
        break;
    }
    max_nodepath = malloc(index+1);
    max_nodepath[index]=0;
    for (int i=0; i<index; ++i)  {
      if (spaceloc[i]==0)
        max_nodepath[i] = '0';
      else max_nodepath[i] = '1';
    }    
  }
  int inserted_newval = 0;
  currNode = tree->root_node;
  index=0;
  while (min_nodepath[index]) {
    if (min_nodepath[index]=='0') {
      if (currNode->lchild)
      currNode = currNode->lchild;
    }
    else {
      if (currNode->rchild)
      currNode = currNode->rchild;
    }
    ++index;
  }// fill array     
  index=0;
  while (1) {
    void *val;
    if ( ( ((val=tree->compare(currNode->val, value_ptr))==currNode->val) && (inserted_newval==0) ) ||
         ( (inserted_newval==0) && (index==(total_nodes-1))) ) {
      memcpy( ((char *)value_array)+ (index*(tree->data_size)), value_ptr, tree->data_size );
      inserted_newval = 1;
      ++index;
    }
    memcpy( ((char *)value_array)+ (index*(tree->data_size)), currNode->val, tree->data_size );
    ++index;
    if ((index==total_nodes-1) && (inserted_newval==0)) {
      memcpy( ((char *)value_array)+ (index*(tree->data_size)), value_ptr, tree->data_size );
      ++index;
    }
    if (index==total_nodes)
      break;
    if ( !is_max_node(tree, currNode, max_nodepath) )
      increment(&currNode);
  }
  *min_nodepath_addr = min_nodepath;
  *max_nodepath_addr = max_nodepath;
}

static void input_new_node(struct binary_tree *tree, 
                           unsigned char *spaceloc, 
                           int len, struct node_ *new_node)
{
  struct node_ *currNode = tree->root_node;
  int index=0;
  while (index<len-1) {
    if (spaceloc[index++]==0)
      currNode = currNode->lchild;
    else currNode = currNode->rchild;
  }
  if (spaceloc[index]==0)
    currNode->lchild = new_node;
  else currNode->rchild = new_node;
  new_node->parent = currNode;
}

// input the data given by value_array into the tree between minnodepath and maxnodepath
static void input_reshaped_data(struct binary_tree *tree, 
                                unsigned char *min_nodepath, 
                                unsigned char *max_nodepath, 
                                unsigned char *value_array)
{
  struct node_ *currNode = tree->root_node;
  int index=0;
  while (min_nodepath[index]!=0) {
    if (min_nodepath[index++]=='0')
      currNode = currNode->lchild;
    else currNode = currNode->rchild;
  }
  index=0;
  while (!is_max_node(tree, currNode, max_nodepath)) { 
    memcpy(currNode->val, value_array, tree->data_size);
    value_array+=(tree->data_size);
    increment(&currNode);
  }
  memcpy(currNode->val, value_array, tree->data_size);
}

// allocates memory for a binary_tree data structure and 
// allocates memory the root node. root node has a value 
// that is null initialized. Each nodes value is of size 
// data_size bytes. compare and print are pointers to 
// funcitons that will be the compare and print members 
// of the binary_tree struct. 
struct binary_tree *init_null_btree(int data_size,  
                              void *(*compare)(void *, void *), 
                              void (*print)(void *) )
{

  struct node_ *root_node = malloc(sizeof(struct node_));
  // null initialize root_node value
  root_node->val = malloc(data_size);
  char *null_initialize = root_node->val;
  for (int i=0; i<data_size; ++i)
    *(null_initialize++) = 0;

  root_node->parent = 0;
  root_node->rchild = 0;
  root_node->lchild = 0;
    
  struct binary_tree *new_btree = malloc(sizeof(struct binary_tree));
  new_btree->root_node = root_node;
  new_btree->compare = compare;
  new_btree->data_size = data_size;
  new_btree->NoOfNodes = 0;
  new_btree->print = print;
  
  return new_btree;
}

// allocates memory for new binary_tree structure and allocates memory 
// for the root node. Root node value is initialized to the value 
// given at value_ptr. data size specifies the size in bytes of each 
// node value in the binary tree. compare and print are pointers 
// to the functions that compare and print node values [ see binary_tree strut deifinion ]
struct binary_tree *init_btree(int data_size,  
                         void *(*compare)(void *, void *) , 
                         void (*print)(void *))
{
  struct node_ *root_node = malloc(sizeof(struct node_));
  // null initialize root_node value
  root_node->val = malloc(data_size);
  memset(root_node->val, 0, data_size);  
  root_node->parent = 0;
  root_node->rchild = 0;
  root_node->lchild = 0;

  struct binary_tree *new_btree = malloc(sizeof(struct binary_tree));
  new_btree->root_node = root_node;
  new_btree->compare = compare;
  new_btree->data_size = data_size;
  new_btree->NoOfNodes = 0;
  new_btree->print = print;
  return new_btree;
}

// ordered binary tree insertion. No insertion occurs if
// value_ptr is already contained within a node in the tree.
// balanced insertion.
void btree_insert(struct binary_tree *tree, void *value_ptr)
{
  // if binary tree has no Nodes (assumed root node
  // has been allocated and null initialized)
  if (!(tree->NoOfNodes)) {
    memcpy((tree->root_node)->val, value_ptr, tree->data_size);
    ++(tree->NoOfNodes);
  }
  else {
    if (tree->NoOfNodes==1) { 
      void *val;
      if ((val = tree->compare((tree->root_node)->val, value_ptr))!=0) {
        struct node_* new_node = malloc(sizeof(struct node_));
        new_node->val = malloc(tree->data_size);
        memcpy(new_node->val, value_ptr, tree->data_size);
        new_node->parent = tree->root_node;
        new_node->lchild = 0;
        new_node->rchild = 0;
        if (val==value_ptr) {
          (tree->root_node)->rchild = new_node;
          ++(tree->NoOfNodes);
        }
        else {
          (tree->root_node)->lchild = new_node;
          ++(tree->NoOfNodes);
        }
      }
      return;
    } // if tree depth (0 starting) is greater than 0.    
    else {
      int depth = floor(log_2(tree->NoOfNodes)), index=0;
      unsigned char parloc[depth];
      for (int i=0; i<depth; ++i)
        parloc[i] = 0; // loc of parent node
      struct node_ *currNode = tree->root_node;
      void *compRes;
      while ((compRes = tree->compare(currNode->val, value_ptr))!=0) {
        if (compRes==(currNode->val)) {
          if (currNode->lchild) {
            currNode = currNode->lchild;
            parloc[index++]='l';
          }
          else { 
            break;
          }
        }
        else if (compRes==value_ptr) {
          if (currNode->rchild) {
            currNode = currNode->rchild;
            parloc[index++]='r';
          }
          else 
            break;
        } 
      }// return due to *value_ptr already being in the tree
      if (!compRes) {
	return;
      }
      // create node to be inserted
      struct node_ *new_node = malloc(sizeof(struct node_));
      new_node->val = malloc(tree->data_size);
      memcpy(new_node->val, value_ptr, tree->data_size);
      new_node->lchild = 0;
      new_node->rchild = 0;
      if (((tree->NoOfNodes)==1) || ((tree->NoOfNodes)==(pow_2(depth+1)-1))) {
        new_node->parent = currNode;
        if (compRes==currNode->val) 
          currNode->lchild = new_node;
        else
          currNode->rchild = new_node;
        ++(tree->NoOfNodes);
        return;
      }
      else {
        int parent_depth = 0;
        while ( parent_depth<depth)  {
          if (parloc[parent_depth]==0)
            break;
          ++parent_depth; 
        } 
        if (parent_depth<depth) {  // if parent node is not on maximum tree depth 
          new_node->parent = currNode;
          if (compRes==currNode->val) 
            currNode->lchild = new_node;
          else
            currNode->rchild = new_node;
          ++(tree->NoOfNodes);
	  //printf("new_node->parent: %p\n", new_node->parent);
	  //printf("root_node addr: %p\n", tree->root_node);
          return;
        }
        // need to move nodes around to make space for new node.
        unsigned char spaceloc[depth];// 0 = left, 1 = right; 
        for (int i=0; i<depth; ++i)
          spaceloc[i] = 0; 
        while ( is_available(tree, &(spaceloc[0]), depth)==0 ) 
          increment_pos( &(spaceloc[0]), depth );
        int total_nodes=calc_NoOfNodes(tree, &(spaceloc[0]), &(parloc[0]), depth);  // fixed a bug in calc_NoOfNodes
        void *value_array = malloc(total_nodes*(tree->data_size)); // TODO: check from here
        unsigned char **min_nodepath_addr = malloc(sizeof(unsigned char *)),
                      **max_nodepath_addr = malloc(sizeof(unsigned char *));
        fill_ordered_array(tree, value_array, total_nodes, 
                           &(spaceloc[0]), &(parloc[0]), 
                           depth, value_ptr, 
                           min_nodepath_addr, max_nodepath_addr);
        input_new_node(tree, &(spaceloc[0]), depth, new_node);
        input_reshaped_data(tree, *min_nodepath_addr, *max_nodepath_addr, value_array);
        free(value_array);
        free(*min_nodepath_addr);
        free(*max_nodepath_addr);
        free(min_nodepath_addr);
        free(max_nodepath_addr);
        ++tree->NoOfNodes;
//	printf("tree->NoOfNodes after insert: %d\n", tree->NoOfNodes);
        return;         
      }
    }
  }
}

// delete a specific value from the binary_tree that is given
// by the data at address value_ptr (tree->data_size bytes of data).
// Function does nothing if value_ptr is not foudn within the tree.
void btree_rem(struct binary_tree *tree, void *value_ptr)
{
  if ((tree->NoOfNodes)==0)
    return;
  struct node_ *currNode = tree->root_node;
  void *val;
  while ( (val = tree->compare(currNode->val, value_ptr))!=0) {
    if (val==currNode->val) {
      if (currNode->lchild)
        currNode = currNode->lchild;
      else return;
    }
    else {
      if (currNode->rchild)
        currNode = currNode->rchild;
      else return;
    }
  }
  if (tree->NoOfNodes==1) {
    tree->NoOfNodes= 0;
   // free((tree->root_node)->val);
    return;
  } 
  unsigned char *value_array = malloc((tree->data_size)*(tree->NoOfNodes-1));
  int depth = floor(log_2(tree->NoOfNodes)), index=0;
  struct node_ *maxNode = currNode = tree->root_node;
  while (currNode->lchild) 
    currNode = currNode->lchild; 
  while (maxNode->rchild) 
    maxNode = maxNode->rchild;
  while (index<(tree->NoOfNodes-1)) {
    void *val;
    if ((val = tree->compare(currNode->val, value_ptr))) {
      memcpy(value_array+(index*(tree->data_size)), currNode->val, tree->data_size);
      ++index; 
    }
    if (currNode!=maxNode)
    increment(&currNode);
  }
  unsigned char *bitstring = int_to_bin(pow_2(depth)-1);
  while ( is_available(tree, bitstring, depth)==1) 
    decrement_bitstring(bitstring);   
  currNode = tree->root_node;
  index = 0;
  while (index<depth-1) {
    if (bitstring[index]=='0')
      currNode = currNode->lchild;
    else currNode = currNode->rchild;
    ++index;
  }  
  if (bitstring[index]=='0') {
    free((currNode->lchild)->val);
    free(currNode->lchild);
    currNode->lchild = 0;
  }
  else {
    free(currNode->rchild->val);
    free(currNode->rchild);
    currNode->rchild = 0;
  }
  free(bitstring);
  maxNode =  currNode = tree->root_node;
  while (currNode->lchild)
    currNode = currNode->lchild;
  while (maxNode->rchild)
    maxNode = maxNode->rchild;
  index=0; 
  while (index<((tree->NoOfNodes)-1)) {
    memcpy(currNode->val, value_array+(index*(tree->data_size)), tree->data_size);
    if (currNode!=maxNode)
    increment(&currNode);
    ++index;
  }
  free(value_array); 
  --(tree->NoOfNodes);  
  return;
}

// Search the binary_tree struct given at the address tree
// for the data type/object given at the address value_ptr.
// Function returns 1 if value_ptr is contained within one of the nodes
// of the tree, 0 otherwise. 
int btree_search(struct binary_tree *tree, void *value_ptr)
{
  if ((tree->NoOfNodes)==0)
    return 0;
  struct node_ *currNode = tree->root_node;
  void *value=0;
  while ( (value = tree->compare(currNode->val, value_ptr))!=0 ) {
    if (value==(currNode->val)) {
      if (currNode->lchild) 
        currNode = currNode->lchild;  
      else {
	      return 0; // value_ptr not contained within tree
      }
    }
    else if (value==value_ptr) {
      if (currNode->rchild) 
        currNode = currNode->rchild;
      else {
	      return 0; 
      }
    }
  } 
  return 1;
}

void *btree_find(struct binary_tree *tree, void *value_ptr)
{
  if ((tree->NoOfNodes)==0)
    return 0;
  struct node_ *currNode = tree->root_node;
  void *val=0;
  while ( (val = tree->compare(currNode->val, value_ptr))!=0 ) {
    if (val==currNode->val) {
      if (currNode->lchild)
        currNode = currNode->lchild;
      else return 0; // value_ptr not contained within tree
    }
    else if (val==value_ptr) {
      if (currNode->rchild)
        currNode = currNode->rchild;
      else return 0;
    }
  } 
  return currNode->val;
}

// prints the values in the tree in ascending order.
// to command line.
void print_btree(struct binary_tree *tree)
{
  printf("\nprinting tree\n");
  struct node_ *currNode = tree->root_node;
  while (currNode->lchild)
    currNode = currNode->lchild;
  int index = 0;
  while (index++<(tree->NoOfNodes)) {
    tree->print(currNode->val);
    printf(" ");
    if (index<(tree->NoOfNodes))
    increment(&currNode);
  }
}

void free_btree(struct binary_tree *tree)
{
  	if ((tree->NoOfNodes) == 0) {
		free((tree->root_node)->val);
		free(tree->root_node);	
	}
	else {
		int tree_depth = floor(log_2(tree->NoOfNodes));
		for (int i=tree_depth; i>=0; --i) {
			unsigned int NoOfNodes = pow_2(i); // The maximum number of Nodes on the current ith level of the tree
			unsigned int bitmask_length = i/8;
			if ((i % 8) != 0) {
				++bitmask_length;
			}
			if (bitmask_length == 0) {
				free((tree->root_node)->val);
                		free(tree->root_node);
				continue;
			}
			for (unsigned int j=0; j<NoOfNodes; ++j) {
				struct node_ *curr_node = tree->root_node;
				unsigned int directional_bitmask = htonl(j);
				unsigned char *directional_indicator = (unsigned char *)(&directional_bitmask);
				directional_indicator += (4-bitmask_length);
				unsigned int skip_initial_bit_count = 8 - (i%8);
				if ((i%8)==0) {
					skip_initial_bit_count = 0;
				}
                                unsigned char first_bitmask = (128 >> skip_initial_bit_count);
				unsigned char free_required = 1;
				for (int k=0; k<bitmask_length; ++k) {
					if (k==0) {
						for (int l=0; l<(8-skip_initial_bit_count); ++l) {
							if (((*directional_indicator) & first_bitmask) == 0) {
								if (curr_node->lchild) {
									curr_node = curr_node->lchild;
								}
								else {
									free_required = 0;
									break;
								}
							}
							else {
								if (curr_node->rchild) {
									curr_node = curr_node->rchild;
								}
								else {
									free_required = 0;
									break;
								}
							}
							first_bitmask >>= 1;
						}

					}
					else {
						
						for (int l=0; l<8; ++l) {
							unsigned char curr_bitmask = 128 >> l;
							if (((*directional_indicator) & curr_bitmask)==0) {
								if (curr_node->lchild) {
									curr_node = curr_node->lchild;
								}	
								else {
									free_required = 0;
									break;
								}
							}
							else {
								if (curr_node->rchild) {
									curr_node = curr_node->rchild;
								}
								else {
									free_required = 0;
									break;
								}
							}
						}
							
					}
					if (!free_required) {
						break;
					}
					++directional_indicator;
				}
				if (!free_required) {
					continue;
				}	
				free(curr_node->val);
				free(curr_node);
			}
		}
	}	
	free(tree);
}


int btree_no_of_nodes(struct binary_tree *tree)
{
  return tree->NoOfNodes;
}

// function that returns the address of the value held within
// the root node
void *btree_get_addr_of_root_val(struct binary_tree *tree)
{
  return (tree->root_node)->val;
}

// If all nodes were in an array in ascending order this function returns
// the value of the node at position index in the array (index being 0 starting)
// If the tree is empty or index refers to an element beyond the maximum ordered
// node in the tree then this function returns 0.
void *btree_get(struct binary_tree *tree, int index)
{
	if (((tree->NoOfNodes) == 0) || ((tree->NoOfNodes) <= index)) {
		return 0;
	}

	struct node_ *curr_node = tree->root_node;
	int curr_node_index = 0;
	while (curr_node->lchild) {
		curr_node = curr_node->lchild;
	}
	for (int i=1; i<=index; ++i) {
		increment(&curr_node);
	}	
	return curr_node->val;
}
