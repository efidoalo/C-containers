/*===============================================;
 * 
 * File: linked list.h
 * COntent: Generic unidirectional linked list
 * that is nonwrapping. Contians generic data type
 * and manages this via using void pointers.
 * DATE: 06/12/2016, ammended and tested 11/02/2017
 *
 ************************************************/

#ifndef __LINKED_LIST_H_INCLUDED__
#define __LINKED_LIST_H_INCLUDED__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
struct node 
{
  void *val;  
  struct node *succ;
}; 

struct linked_list 
{
  int data_size; // number iof bytes stored at each node val pointer
  struct node *first; // pointer to firt node in list
  int size;   // number of current entries in the list, ie number of
              // nodes in the list currently.
  void *(*compare)(void *val1, void* val2); // returns the greater of the two object
                                            // stored at val1 and val2.
                                            //  if they are both equa then the null pointer is returned.
};

// creates a singly linked list with zeto entries. but specifies the
// number of bytes of each elemen held within each node. No memory
// is allocated for the first node or any susequent nodes.
struct linked_list *null_init_list(int elem_size, void *(*compare)(void *v1, void *v2))
{
  struct linked_list *newlist = malloc(sizeof(struct linked_list));
  newlist->first = 0;
  newlist->size = 0;
  newlist->data_size = elem_size;
  newlist->compare = compare;
  return newlist;
}

// initiate first node in the list wiht the value piointer to by the val
// pointer. elem_size gives the number of bytes that is stored within each
// node.
struct linked_list *init_list(int data_size, void* value_ptr, void *(*compare)(void *v1, void *v2))
{
  struct linked_list *newlist = malloc(sizeof(struct linked_list));
  struct node *root_node = malloc(sizeof(struct node));
  root_node->val = malloc(data_size);
  root_node->succ = 0;
  memcpy(root_node->val, value_ptr, data_size);
  newlist->first = root_node;
  newlist->size = 1;
  newlist->data_size = data_size;
  newlist->compare = compare;  
  return newlist;
}

// push the object at value_pointer to the start 
// of the list, regardless of the lists previous contents
void list_push_front(struct linked_list *list, void *value_pointer)
{
  if ((list->size)==0) {
    list->first = malloc(sizeof(struct node));
    list->first->val = malloc(list->data_size);
    memcpy(list->first->val, value_pointer, list->data_size);
    list->first->succ = 0;
    ++(list->size);
    return;
  }
  if ((list->size)>0) {
    struct node *newlink = malloc(sizeof(struct node));
    newlink->val = malloc(list->data_size);
    memcpy(newlink->val, value_pointer, list->data_size);
    newlink->succ = list->first;
    list->first = newlink;
    ++(list->size);
    return;
  }
}

// performs ordered isnertion. the returned pointer points to the node int he list
// that has either been inserted (or id the value was already present in th elsit then the returned pintyer
// points to the node that is already in the lsit). insert the value given at value_poitner in to the linked list
// at list. 
struct node *list_insert(struct linked_list *list, void *value_pointer)
{ 
  if (list->size==0) {
    list_push_front(list, value_pointer);
    return list->first;
  }
  else if ((list->size)>0) {
    struct node *node_track = list->first;
    struct node *node_trackprev = 0;
    void *val_track;
    int EOFList=0;
    while (node_track) {
      if ( (val_track = list->compare(node_track->val, value_pointer))==node_track->val) {
        break;
      }
      else if (val_track==value_pointer) {
        node_trackprev = node_track;
        node_track = node_track->succ;
        if (!node_track) {
          EOFList=1;  
          break;
        }
      }
      else if (val_track==0) {
        return node_track;
      }
    }
    struct node *new_node = malloc(sizeof(struct node));
    new_node->val = malloc( list->data_size );
    memcpy(new_node->val, value_pointer, list->data_size); 
    if (node_trackprev==0) {
      new_node->succ = list->first;
      list->first = new_node;
    }
    else if (EOFList==0) {
      new_node->succ = node_track;
      node_trackprev->succ = new_node;
    }
    else if (EOFList==1) {
      new_node->succ = 0;
      node_trackprev->succ = new_node;
    }
    ++list->size;
    return new_node;
  }
  else if (list->size<0) {
    printf("\nInvalid input arguement list. The list has a negative node size.");
    return 0;
  }
}

// removed/delete the object given by vlaue_ptr from the lniked_list list.
// returns 1 to indicate that node has been deleted, 0 otherwise.
int list_rem(struct linked_list *list, void *value_ptr)
{
  struct node *currNode = list->first, *prevNode = 0;
  void *val=0;
  if (currNode) {
    while ( (val = list->compare(currNode->val, value_ptr))!=0) {
      prevNode = currNode;
      currNode = currNode->succ;
      if (!currNode)
        return 0;
    }
    if (!prevNode) {
      list->first = currNode->succ;
      --(list->size);
      free(currNode->val);
      free(currNode);
      if (!(list->first)) { // null initilize list
        list->first = malloc(sizeof(struct node));
        list->first->val = malloc(list->data_size);
        list->first->succ = 0;
        unsigned char NULLinit[list->data_size];
        for (int i=0; i<list->data_size; ++i)
          NULLinit[i]=0;
        memcpy(list->first->val, &(NULLinit[0]), list->data_size);
      }
      return 1;    
    }
    prevNode->succ = currNode->succ;
    free(currNode->val);
    free(currNode);
    --(list->size);
    return 1;
  }
  return 0;
}

// Search the linked list at list for the object at value_ptr.
// Returned address is either the node that stores value_ptr
// or the null pointer if the value_ptr is not cointained within
// the list.
struct node *search_list(struct linked_list *list, void *value_ptr)
{
  struct node *currNode = list->first;
  void *val =0;
  while ( (val = list->compare(currNode->val, value_ptr))!=0) {
    currNode = currNode->succ;
    if (!currNode)
      return 0; // vlaue_ptr node not found.
  }
  return currNode;
}

void free_list(struct linked_list *list)
{
  struct node *currNode = list->first;
  while (currNode) {
    struct node *temp = currNode;
    currNode = currNode->succ;
    free(temp->val);
    free(temp);
  }
  free(list);
}

#endif
