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
#include "binary_tree.h"

//
// This function determines if there is another leaf node after node having a particular subtree root.
// heigbt is tracked and either the next leaf node or NULL is returned, depending on if there exists
// another leaf as part of a particular subtree or not.
struct node_ * has_right_branch(struct node_ *node, int *height) 
{
	int new_height = *height;
	if (node->rchild) {
		node = node->rchild;
		++new_height;
		while (node) {
                        if (node->lchild) {
                                node = node->lchild;
                                ++new_height;
                        }
                        else if (node->rchild) {
                                node = node->rchild;
                                ++new_height;
                        }       
                        else {
                                break;
                        }
                }
		*height = new_height;
		return node;
	}
	else {  
		if (new_height == 1) {
			return NULL;
		}
		while ( ((node->parent)->lchild != node) || ((node->parent)->rchild == NULL)) {
			node = node->parent;
			--new_height;
			if ((node->parent) == NULL) {
				return NULL; // root node reached, no leaf node comes after the input leaf node	
			}
			if (new_height <= 1) {

				return NULL; // subtree has no more leaf nodes
			}
		}
		node = (node->parent)->rchild;
		while (node) {
			if (node->lchild) {
				node = node->lchild;
				++new_height;
			}
			else if (node->rchild) {
				node = node->rchild;
				++new_height;
			}
			else {
				break;
			}
		}
		*height = new_height;
		return node;
	}	
	
}

int calculate_node_height(struct node_ *node)
{
	if (node==NULL) {
		return 0;
	}
	int node_height = 1;
	while (node->lchild) {
		++node_height;
		node = node->lchild;
	}	
	int max_height = node_height;
	node = has_right_branch(node, &node_height); // progress to next leaf whilst tracking height
	while ( node != NULL  ) {
		if (node_height > max_height) {
			max_height = node_height;
		}
		node = has_right_branch(node, &node_height);
	}
	return max_height;
}

int calc_balance_factor(struct node_ *node)
{
	return calculate_node_height(node->rchild) - calculate_node_height(node->lchild);	
}

char *balance_factor(struct node_ *node)
{
	return &(node->bf);
}

// X = root of subtree to be rotated left
// Z = right child of X, Z is right heavy on insertion,
// and Z has height Height(LeftSubtree(X))+2
//
// Returns new root of rebalanced subtree
struct node_ *rotate_left(struct node_ *X, struct node_ *Z)
{
	struct node_ *t23 = Z->lchild;
	X->rchild = t23;
	if (t23 != NULL) {
		t23->parent = X;	
	}
	Z->lchild = X;
	X->parent = Z;
	
	// 1st case BF(Z)==0 only happens with deletion, not insertion
	if ((Z->bf) == 0) {
		X->bf = 1;
		Z->bf = -1;
	}
	else { // second case can happen with insertion or deletion
		X->bf = 0;
		Z->bf = 0;
	}
	return Z;
}

// X = root of subtree to be rotated right
// Z = left child of X, Z is left heavy (on insertion) and Z has height
// equal to Height(RightSubtree(X))+2
struct node_ *rotate_right(struct node_ *X, struct node_ *Z)
{
	struct node_ *t23 = Z->rchild;
	X->lchild = t23;
	if (t23 != NULL) {
		t23->parent = X;
	}
	Z->rchild = X;
	X->parent = Z;

	// 1st case, BF(Z) == 0, only happens with deletion, not insertion
	if ((Z->bf)==0) {
		Z->bf = 1;
	 	X->bf = -1;	
	}
	else { // second xase happens with insertion or deletion
		X->bf = 0;
		Z->bf = 0;
	}
	return Z;
}

//X = root of subtree to be rotated
//Z = its right child, left heavy with height Height(LeftSubtree(X))+2
//function returns the root of the rebalanced subtree
struct node_ *rotate_rightleft(struct node_ *X, struct node_ *Z)
{
	// Z is by 2 higher than its sibling
	struct node_ *Y = Z->lchild; // Inner child of Z
	// Y is by 1 higher than sibling
	struct node_ *t3 = Y->rchild;
	Z->lchild = t3;
	if (t3 != NULL) {
		t3->parent = Z;
	}
	Y->rchild = Z;
	Z->parent = Y;
	struct node_ *t2 = Y->lchild;
	X->rchild = t2;
	if (t2 != NULL) {
		t2->parent = X;
	}
	Y->lchild = X;
	X->parent = Y;

	// 1st case, BF(Y)==0 only happens with deletion, not insertion
	if ((Y->bf)==0) {
		X->bf = 0;
		Z->bf = 0;
	}
	else {
		// other cases happen with insertion or deletion
		if ((Y->bf)>0) {
			X->bf = -1;
			Z->bf = 0;
		}
		else {
			X->bf = 0;
			Z->bf = 1;
		}
	}
	Y->bf = 0;
	return Y;
}

// X = root of subtree to be balanced
// Z = left child, right heavy with height Height(RightSubtree(X)) + 2
// function returns the root node of the balanced subtree
struct node_ *rotate_leftright(struct node_ *X, struct node_ *Z)
{
	struct node_ *Y = Z->rchild; // Inner child of Z
	struct node_ *t3 = Y->lchild;
	Z->rchild = t3;
	if (t3!=NULL) {
		t3->parent = Z;
	}
	Y->lchild = Z;
	Z->parent = Y;
	struct node_ *t2 = Y->rchild;
	X->lchild = t2;
	if (t2!=NULL) {
		t2->parent = X;
	}
	Y->rchild = X;
	X->parent = Y;

	//1st case BF(Y) == 0 only happens with deletion, not insertion
	if ((Y->bf)==0) {
		X->bf = 9;
		Z->bf = 0;
	}
	else {
		// other cases happen with insertion or deletion
		if ((Y->bf)>0) {
			X->bf = 0;
			Z->bf = -1;
		}
		else {
			Z->bf = 0;
			X->bf = 1;

		}
	}
	Y->bf = 0;
	return Y;
}

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
  root_node->bf = 0;
    
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
  if ((root_node->val) == NULL) {
	printf("Error allocating memory using malloc.\n");
	exit(EXIT_FAILURE);
  }
  memset(root_node->val, 0, data_size);  
  root_node->parent = 0;
  root_node->rchild = 0;
  root_node->lchild = 0;
  root_node->bf = 0;	

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
	struct node_ *new_node = malloc(sizeof(struct node_));
      	if (new_node == NULL) {
		printf("Error occurred allocating memory with malloc.\n");
		exit(EXIT_FAILURE);
	}		
	struct node_ *curr_node = tree->root_node;  
	while (curr_node) {
		void *val = tree->compare(curr_node->val, value_ptr);
		if (val == 0) {
			return; // no insertion occurs  since value is already contained in tree
		}
		else if (val == value_ptr) {
			if (!(curr_node->rchild)) {
				new_node->val = malloc(tree->data_size);
				if (new_node->val == NULL) {
					printf("Error allocating memory with malloc.\n");
					exit(EXIT_FAILURE);
				}	
				memcpy(new_node->val, value_ptr, tree->data_size);
				new_node->bf = 0;
				new_node->parent = curr_node;
				new_node->lchild = NULL;
				new_node->rchild = NULL;
				curr_node->rchild = new_node;
				break;
			}
			else {
				curr_node = curr_node->rchild;
			}
		}
		else {
			if (!(curr_node->lchild)) {
				new_node->val = malloc(tree->data_size);
				if (new_node->val == NULL) {
                                        printf("Error allocating memory with malloc.\n");
                                        exit(EXIT_FAILURE);
                                }
                                memcpy(new_node->val, value_ptr, tree->data_size);
                                new_node->bf = 0;
                                new_node->parent = curr_node;
				new_node->lchild = NULL;
				new_node->rchild = NULL;
                                curr_node->lchild = new_node;
                                break;
			}
			else {
				curr_node = curr_node->lchild;
			}
		}
	}
	// retrace
		
	while (curr_node) {
		//printf("here1 ");
		int curr_node_balance_factor = calc_balance_factor(curr_node);
		curr_node->bf = curr_node_balance_factor;
		if ( ((curr_node->bf) <-1) || ((curr_node->bf)>1) ) {
			// self balancing rotation needs to occur
			struct node_ *Z = NULL;
			struct node_ *subtree_parent = curr_node->parent;
			if ((curr_node->bf) < -1) {
				Z = curr_node->lchild;
				if ((Z->bf) <= 0) {
					if (subtree_parent != NULL) {
						if ((subtree_parent->lchild)==curr_node) {	
							struct node_ *subtree_root = rotate_right(curr_node, Z);
							subtree_root->parent = subtree_parent;
							subtree_parent->lchild = subtree_root;
						}		
						else {
                                                        struct node_ *subtree_root = rotate_right(curr_node, Z);
                                                        subtree_root->parent = subtree_parent;
							subtree_parent->rchild = subtree_root;
						}	
					}	
					else {
						tree->root_node = rotate_right(curr_node, Z);
						(tree->root_node)->parent = NULL;						
					}		
				}
				else {
					if (subtree_parent != NULL) {
						if ((subtree_parent->lchild) == curr_node) {
							struct node_ *subtree_root =  rotate_leftright(curr_node, Z);
							subtree_root->parent = subtree_parent;
							subtree_parent->lchild = subtree_root;
						}
						else {
							struct node_ *subtree_root = rotate_leftright(curr_node, Z);
							subtree_root->parent = subtree_parent;
							subtree_parent->rchild = subtree_root;
						}
					}
					else {
						tree->root_node = rotate_leftright(curr_node, Z);
						(tree->root_node)->parent = NULL;
					}
				}
			}
			else {
				Z = curr_node->rchild;
				if ((Z->bf) >= 0) {
					//printf("HERE0\n");
					if (subtree_parent != NULL) {
						if ((subtree_parent->lchild)==curr_node) {
							//printf("HERE2\n");
							struct node_ *subtree_root = rotate_left(curr_node, Z);
							subtree_root->parent = subtree_parent;
							subtree_parent->lchild = subtree_root;
						}
						else {
							//printf("HERE1\n");
							struct node_ *subtree_root = rotate_left(curr_node, Z);
							subtree_parent->rchild = subtree_root; 
							subtree_root->parent = subtree_parent;
						}
					}
					else {
						//printf("HERE3\n");
						tree->root_node = rotate_left(curr_node, Z);
						(tree->root_node)->parent = NULL;
					}
				}
				else {
					if (subtree_parent != NULL) {
                                                if ((subtree_parent->lchild)==curr_node) {
							struct node_ *subtree_root = rotate_rightleft(curr_node, Z);
                                                        subtree_root->parent = subtree_parent;
                                                        subtree_parent->lchild = subtree_root;
                                                }
                                                else {
							struct node_ *subtree_root = rotate_rightleft(curr_node, Z);
                                                        subtree_root->parent = subtree_parent;
                                                        subtree_parent->rchild = subtree_root;
                                                }
                                        }
                                        else {
                                                tree->root_node = rotate_rightleft(curr_node, Z);
						(tree->root_node)->parent = NULL;
                                        }
				}
			}
			++(tree->NoOfNodes);
			return;
		}
		curr_node = curr_node->parent;		
	}
	++(tree->NoOfNodes);
	return;

  }
}

// delete a specific value from the binary_tree that is given
// by the data at address value_ptr (tree->data_size bytes of data).
// Function does nothing if value_ptr is not foudn within the tree. AVL invariants maintained
void btree_rem(struct binary_tree *tree, void *value_ptr)
{
  //printf("here0\n");
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
  //printf("here1\n");
  	struct node_ *subtree_root = currNode;
	if ((currNode->lchild == NULL) && (currNode->rchild ==NULL)) {
		//printf("HERE0\n");
		if ((currNode->parent)->lchild == currNode) {
			(currNode->parent)->lchild = NULL;
		}
		else {
			//printf("HERE0.5\n");
			(currNode->parent)->rchild = NULL;
			
		}
		subtree_root = currNode->parent;
		free(currNode->val);
		free(currNode);
		tree->NoOfNodes = (tree->NoOfNodes)-1;
	}
	else if ( ((currNode->lchild) == NULL) || ((currNode->rchild) == NULL) ) {
		//printf("HERE1\n");
	        if ((currNode->lchild) != NULL) {
			if ((tree->root_node) == currNode) {
				tree->root_node = currNode->lchild;
				tree->NoOfNodes = (tree->NoOfNodes)-1;
				free(currNode->val);
				free(currNode);
				subtree_root = tree->root_node;
				(tree->root_node)->parent = NULL;
			}				
			else {
				if ((currNode->parent)->lchild == currNode) {
					(currNode->parent)->lchild = currNode->lchild;
					(currNode->lchild)->parent = currNode->parent;
					subtree_root = currNode->lchild;
					free(currNode->val);
					free(currNode);
					tree->NoOfNodes = (tree->NoOfNodes)-1;										
				}
				else {
					// (currNode->parent)->rchild = currNode
					(currNode->parent)->rchild = currNode->lchild;
					(currNode->lchild)->parent = currNode->parent;
					subtree_root = currNode->lchild;
					free(currNode->val);
					free(currNode);
					tree->NoOfNodes = (tree->NoOfNodes)-1;
				}
			}
	        }
		else {
			if ((tree->root_node)==currNode) {
				tree->root_node = currNode->rchild;
				(tree->root_node)->parent = NULL;
				tree->NoOfNodes = (tree->NoOfNodes)-1;
				free(currNode->val);
				free(currNode);
				subtree_root = tree->root_node;
			}
			else {
				if ((currNode->parent)->lchild == currNode) {
					(currNode->parent)->lchild = currNode->rchild;
					(currNode->rchild)->parent = currNode->parent;
					subtree_root = currNode->rchild;
					free(currNode->val);
					free(currNode);
					tree->NoOfNodes = (tree->NoOfNodes)-1;
				}
				else {	
					// (currNode->parent)->rchild == currNode
					(currNode->parent)->rchild = currNode->rchild;
					(currNode->rchild)->parent = currNode->parent;
					subtree_root = currNode->rchild;
					free(currNode->val);
					free(currNode);
					tree->NoOfNodes = (tree->NoOfNodes)-1;
				}	
			}
		}
	
	}
	else {
		//printf("HERE2\n");
		// currNode has two children
		if ((currNode->rchild)->lchild == NULL) {
			// currNodes successor is currNode->rchild
			(currNode->rchild)->lchild = currNode->lchild;
			(currNode->lchild)->parent = currNode->rchild;
			(currNode->rchild)->parent = currNode->parent;
			if (currNode->parent) {
				if ((currNode->parent)->rchild == currNode) {
					(currNode->parent)->rchild = currNode->rchild;
				}
				else {
					(currNode->parent)->lchild = currNode->rchild;
				}
			}
			else {
				tree->root_node = currNode->rchild;
				(tree->root_node)->parent = NULL;
			}
			subtree_root = currNode->rchild;
			free(currNode->val);
			free(currNode);
			tree->NoOfNodes = (tree->NoOfNodes)-1;
		}
		else {
				
			// succ is the node that comes numerically after currNode in the tree, succ for successor
			struct node_ *succ = (currNode->rchild)->lchild;
			while (succ->lchild) {
				succ = succ->lchild;
			}
			(succ->parent)->lchild = succ->rchild;
			if (succ->rchild) {
				(succ->rchild)->parent = succ->parent;
				subtree_root = succ->rchild;
			}
			else {
				subtree_root = succ->parent;
			}
			succ->lchild = currNode->lchild;
			succ->rchild = currNode->rchild;
			currNode->lchild->parent = succ;
			currNode->rchild->parent = succ;
			if (tree->root_node == currNode) {
				tree->root_node = succ;
				succ->parent = NULL;
			}
			else {
				succ->parent = currNode->parent;
				if ((currNode->parent)->lchild == currNode) {
					(currNode->parent)->lchild = succ;
				}
				else {
					(currNode->parent)->rchild = succ;
				}
			}
			//printf("succ->val: %d\n", *((int *)(succ->val)));
			free(currNode->val);
                        free(currNode);
                        tree->NoOfNodes = (tree->NoOfNodes)-1;

		}
	}
	struct node_ *curr_node = subtree_root;
	int curr_node_bf = calc_balance_factor(curr_node);
	curr_node->bf = curr_node_bf;
	while ((curr_node_bf <2) && (curr_node_bf>-2)) {
		curr_node = curr_node->parent;
		if (curr_node == NULL) {
			break;
		}
		curr_node_bf = calc_balance_factor(curr_node);
		curr_node->bf = curr_node_bf;
	}
	if (curr_node == NULL) {
		return; // retraced with AVL invariant maintained
	}
	//curr_node->bf == +2 or -2  
	struct node_ *subtree_parent = NULL;
	struct node_ *X = curr_node;
	
	if ((X->bf)>1) {
		struct node_ *Z = X->rchild;
		if ((Z->bf)>=0) {
			if ((X->parent) != NULL) {
				if ((X->parent)->lchild == X) {
					subtree_parent = X->parent;
					struct node_ *subtree_root = rotate_left(X,Z);
					subtree_root->parent = subtree_parent;
					subtree_parent->lchild =subtree_root;
				}
				else {
					subtree_parent = X->parent;
                                        struct node_ *subtree_root = rotate_left(X,Z);
                                        subtree_root->parent = subtree_parent;
                                        subtree_parent->rchild =subtree_root;					
				}
			}
			else {
				tree->root_node = rotate_left(X,Z);
				(tree->root_node)->parent = NULL;
			}
			
		}
		else {
			if ((X->parent) != NULL) {
                                if ((X->parent)->lchild == X) {
                                        subtree_parent = X->parent;
                                        struct node_ *subtree_root = rotate_rightleft(X,Z);
                                        subtree_root->parent = subtree_parent;
                                        subtree_parent->lchild =subtree_root;
                                }
                                else {
                                        subtree_parent = X->parent;
                                        struct node_ *subtree_root = rotate_rightleft(X,Z);
                                        subtree_root->parent = subtree_parent;
                                        subtree_parent->rchild =subtree_root;
                                }
                        }
                        else {
                                tree->root_node = rotate_rightleft(X,Z);
				(tree->root_node)->parent = NULL;
                        }
		}
	}
	else if ((X->bf)<-1) {
		//printf("getting there\n");
		struct node_ *Z = X->lchild;
		if ((Z->bf)<=0) {
                        if ((X->parent) != NULL) {
                                if ((X->parent)->lchild == X) {
                                        subtree_parent = X->parent;
                                        struct node_ *subtree_root = rotate_right(X,Z);
                                        subtree_root->parent = subtree_parent;
                                        subtree_parent->lchild =subtree_root;
                                }
                                else {
                                        subtree_parent = X->parent;
                                        struct node_ *subtree_root = rotate_right(X,Z);
                                        subtree_root->parent = subtree_parent;
                                        subtree_parent->rchild =subtree_root;
                                }
                        }
                        else {  
                                tree->root_node = rotate_right(X,Z);
				(tree->root_node)->parent = NULL;
                        }
                }
                else {
                        if ((X->parent) != NULL) {
                                if ((X->parent)->lchild == X) {
                                        subtree_parent = X->parent;
                                        struct node_ *subtree_root = rotate_leftright(X,Z);
                                        subtree_root->parent = subtree_parent;
                                        subtree_parent->lchild =subtree_root;
                                }
                                else {
                                        subtree_parent = X->parent;
                                        struct node_ *subtree_root = rotate_leftright(X,Z);
                                        subtree_root->parent = subtree_parent;
                                        subtree_parent->rchild =subtree_root;
                                }
                        }
                        else {
                                tree->root_node = rotate_leftright(X,Z);
				(tree->root_node)->parent = NULL;
                        }
                }
	}
	if (subtree_parent != NULL) {
		while (subtree_parent != NULL) {
			struct node_ *curr_node = subtree_parent;
			int curr_node_bf = calc_balance_factor(curr_node);
			curr_node->bf = curr_node_bf;
			struct node_ *X = curr_node;
			int X_bf = calc_balance_factor(X);
			X->bf = X_bf;
			if ( ((X->bf) <-1) || ((X->bf)>1) ) {
				struct node_ *Z = NULL;
				if ((X->bf) < -1) {
					Z = X->lchild;
					if ((Z->bf) <= 0) {
						if (X->parent) {
							if ((X->parent)->lchild == X) {
								subtree_parent = X->parent;
								struct node_ *subtree_root = rotate_right(X,Z);
								subtree_parent->lchild = subtree_root;
								subtree_root->parent = subtree_parent;	
							}
							else {
								subtree_parent = X->parent;
								struct node_ *subtree_root = rotate_right(X,Z);
								subtree_parent->rchild = subtree_root;
								subtree_root->parent = subtree_parent; 
							}
						}
						else {
							tree->root_node = rotate_right(X,Z);
							(tree->root_node)->parent = NULL;
							subtree_parent = NULL;
						}
					}
					else {
						if (X->parent) {
							if ((X->parent)->lchild == X) {
								subtree_parent = X->parent;
								struct node_ *subtree_root = rotate_leftright(X,Z);
								subtree_parent->lchild = subtree_root;
								subtree_root->parent = subtree_parent; 
							}
							else {
								subtree_parent = X->parent;
								struct node_ *subtree_root = rotate_leftright(X,Z);
								subtree_parent->rchild = subtree_root;
								subtree_root->parent = subtree_parent;
							}
						}
						else {
							tree->root_node = rotate_leftright(X,Z);
							(tree->root_node)->parent = NULL;
							subtree_parent = NULL;
						}

					}
				}
				else  {
					Z = X->rchild;
					if ((Z->bf) >= 0) {
						if (X->parent) {
							if ((X->parent)->lchild == X) {
								subtree_parent = X->parent;
								struct node_ *subtree_root = rotate_left(X,Z);
								subtree_parent->lchild = subtree_root;
								subtree_root->parent = subtree_parent; 
							}
							else {
								subtree_parent = X->parent;
								struct node_ *subtree_root = rotate_left(X,Z);
								subtree_parent->rchild = subtree_root;
								subtree_root->parent = subtree_parent;
							}
						}
						else {
							tree->root_node = rotate_left(X,Z);
							(tree->root_node)->parent = NULL;
							subtree_parent = NULL;
						}
					}
					else {
						if (X->parent) {
							if ((X->parent)->lchild == X) {
								subtree_parent = X->parent;
								struct node_ *subtree_root = rotate_rightleft(X,Z);
								subtree_parent->lchild = subtree_root;
								subtree_root->parent = subtree_parent;
							}
							else {
								subtree_parent = X->parent;
								struct node_ *subtree_root = rotate_rightleft(X,Z);
								subtree_parent->rchild = subtree_root;
								subtree_root->parent = subtree_parent;
							}
						}
						else {
							tree->root_node = rotate_rightleft(X,Z);
							(tree->root_node)->parent = NULL;
							subtree_parent = NULL;
						}

					}
				}
			}
			subtree_parent = X->parent;
		}
	}


	
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
  int counter = 0;
  while (currNode->lchild) {
    currNode = currNode->lchild;
    ++counter;
  }
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
	if ((tree->NoOfNodes)<=1) {
		free((tree->root_node)->val);
		free(tree->root_node);
		return;
	}

	int left_subtree_height = calculate_node_height((tree->root_node)->lchild);
	int right_subtree_height = calculate_node_height((tree->root_node)->rchild);
	int tree_depth = -1;
	if (left_subtree_height > right_subtree_height) {
		tree_depth = left_subtree_height;
	}
	else {
		tree_depth = right_subtree_height;
	}

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


