/*========================================;
 *
 * File: binary tree.h
 * Content: binary tree generice data type
 * container. Support only for one data type
 * to be stored throughout the tree.
 * self balancing insertion and deletion
 * ensures optimum search.
 * Date: 2/11/2016
 *
 ******************************************/

#ifndef BINARY_TREE_H
#define BINARY_TREE_H


struct node_;
// 

struct binary_tree;


// allocates memory for a binary_tree data structure and 
// allocates memory the root node. root node has a value 
// that is null initialized. Each nodes value is of size 
// data_size bytes. compare and print are pointers to 
// funcitons that will be the compare and print members 
// of the binary_tree struct. 
struct binary_tree *init_null_btree(int data_size,  
                              void *(*compare)(void *, void *), 
                              void (*print)(void *) );

// allocates memory for new binary_tree structure and allocates memory 
// for the root node. Root node value is initialized to the value 
// given at value_ptr. data size specifies the size in bytes of each 
// node value in the binary tree. compare and print are pointers 
// to the functions that compare and print node values [ see binary_tree strut deifinion ]
struct binary_tree *init_btree(int data_size,  
                         void *(*compare)(void *, void *) , 
                         void (*print)(void *));

// ordered bnary tree isnertion. No insertion occurs if
// value_ptr is already contained within a node in the tree.
// balanced insertion.
void btree_insert(struct binary_tree *tree, void *value_ptr);

// delete a specific value from the binary_tree that is given
// by the data at address value_ptr (tree->data_size bytes of data).
// Function does nothing if value_ptr is not foudn within the tree.
void btree_rem(struct binary_tree *tree, void *value_ptr);

// Search the binary_tree struct given at the address tree
// for the data type/object given at the address value_ptr.
// Function returns 1 if value_ptr is contained within one of the nodes
// of the tree, 0 otherwise. 
int btree_search(struct binary_tree *tree, void *value_ptr);

// Exacttly the same as btree_search but returns the address of the node->val
// that compares equal to value_ptr. If value_ptr is not found in tree this
// function returns null
void *btree_find(struct binary_tree *tree, void *value_ptr);

// prints the values in the tree in ascending order.
// to command line.
void print_btree(struct binary_tree *tree);

// free the memory allocated by tree
void free_btree(struct binary_tree *tree);

// function that returns the number of nodes in the binary tree pointer to
// by tree
int btree_no_of_nodes(struct binary_tree *tree);

// This function returns the node->val pointer (address of a stored data 
// object) of the node with index index if tree was an array of sorted data 
// objects in asscending order.
void *btree_get(struct binary_tree *tree, int index);

// function that returns the root node of a binary tree
void *btree_get_addr_of_root_val(struct binary_tree *tree);

#endif
