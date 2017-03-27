/*==================================;
 *
 * File: queue.h
 * Cotnents: Header file including definitions
 * for functions and structs to implement
 * the queue data structure. Queue is 
 * designed to cotnain one data type
 * of fixed length, but this data type
 * may be user defined/arbitrary.
 * Date: 27/03/2017
 * 
 *************************************/

#ifndef __QUEUE_H_INCLUDED__
#define __QUEUE_H_INCLUDED__

#include <string.h>
#include <stdlib.h>
#include "doubly linked list.h"
struct queue
{
  int data_size; // size in bytes of the contained data type 
  int size;      // number of objects the queue currently contains
  struct doubly_linked_list *elem; // storage and access of queued objects
};


struct queue *init_queue(int data_size)
{
  struct queue *queue = malloc(sizeof(struct queue));
  queue->data_size = data_size;
  queue->elem = init_dlist(data_size, 0);
  queue->size = 0;
}

// pushes the value given ata ddress val to the back of the queue 
int queue_push(struct queue *queue, void *val)
{
  dlist_push_front(queue->elem, val);
  ++(queue->size);
}

// pops the value that off of the end of the queue, retrieves a pointer
// to the value, if size of queue is zero, null pointer is returned
void *queue_pop(struct queue *queue)
{
  if (queue->size==0)
    return 0;
  struct link *currLink = dlist_end(queue->elem);
  currLink = currLink->prev;
  void *elem = malloc(queue->data_size);
  memcpy(elem, currLink->val, queue->data_size);
  dlist_pop_back(queue->elem);
  --(queue->size);
  return elem;
}

void free_queue(struct queue *queue)
{
  free_dlist(queue->elem);
  free(queue);
}

#endif
