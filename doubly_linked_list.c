/*=================================================;
 *
 * File: doubly linked list.c
 * Contents: type generic doubly linked list container
 * implementation, using void pointers.
 * Date: 26/10/2016
 *
 **************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct link
{
  void *val;
  struct link *prev;
  struct link *succ;
};

struct doubly_linked_list
{
  struct link *beg;  /* points to first link */
  struct link *end;  /* points to one beyond the last link */
  int elem_size;     /* specifies the constant size (in bytes) of each entry */
  int size;          /* number if elements/entries in the list */
  void *(*compare)(void *, void *);//compares link->val data types,
                                   // returns the greater data type or null if
                                   // equal
};

/* returns a doubly_linked_list initialized to 0,0,elem_size. use this function
   for initialization of doubly_linked_lists, elem_size=sizeof(type) */
struct doubly_linked_list *init_dlist(int elem_size, void *(*compare)(void *, void *))
{
  struct doubly_linked_list* list = malloc(sizeof(struct doubly_linked_list));
  list->beg = malloc(sizeof(struct link));
  list->beg->prev = 0;
  list->beg->val = malloc(elem_size);
  // null initialize beg->val
  unsigned char NULLinit[elem_size];
  for (int i=0; i<elem_size; ++i) 
    NULLinit[i] = 0;
  memcpy(list->beg->val, &(NULLinit[0]), elem_size);
  list->end = malloc(sizeof(struct link));
  list->end->val=0;
  list->end->succ=0;
  list->end->prev = list->beg; // end link has been allocated but its val member 
                               // is a null pointer.
  list->beg->succ = list->end;
  list->elem_size = elem_size;
  list->size = 0; 
  list->compare = compare;
  return list;
}

/* inserts a link into the list at the end, entry/element value
   equal to *value_ptr */
void dlist_push_back(struct doubly_linked_list *list, void *value_ptr)
{
  if ( list->size==0 ) {
    memcpy(list->beg->val, value_ptr, list->elem_size);
  }
  else {
    struct link *newlink_addr = (struct link *)malloc(sizeof(struct link));
    newlink_addr->val = malloc(list->elem_size);
    memcpy(newlink_addr->val, value_ptr, list->elem_size);
    newlink_addr->succ = list->end;
    newlink_addr->prev = list->end->prev;
    list->end->prev->succ = newlink_addr;
    list->end->prev = newlink_addr;
  }
  ++list->size;
} 

/* inserts link at front of list having value identical to that at address
   value_ptr. This function adjusts list->beg. */
void dlist_push_front(struct doubly_linked_list *list, void *value_ptr)
{
  if (list->size==0) 
    dlist_push_back(list,value_ptr);
  else {
    struct link *newlink_addr = (struct link *)malloc(sizeof(struct link));
    newlink_addr->val = malloc(list->elem_size);
    memcpy(newlink_addr->val, value_ptr, list->elem_size);
    newlink_addr->prev = 0;
    newlink_addr->succ = list->beg;
    list->beg->prev = newlink_addr;
    list->beg = newlink_addr;
    ++list->size;
  }
}

/* removes the final link from the list */
void dlist_pop_back(struct doubly_linked_list *list)
{
  if (list->size==0)
    return;
  else if (list->size==1) {
    unsigned char NULLinit[list->elem_size];
    for (int i=0; i<list->elem_size; ++i)
    NULLinit[i]=0;
    memcpy(list->beg->val, &(NULLinit[0]), list->elem_size);   
    --(list->size);
  }
  else {
    struct link *temp = list->end->prev;
    list->end->prev->prev->succ = list->end;
    list->end->prev = list->end->prev->prev;
    free( temp->val);
    free( temp);
    --list->size;
  }
}

/* removes the first link from the list */
void dlist_pop_front(struct doubly_linked_list *list)
{
  if (list->size==0) 
    return;
  else if (list->size==1) {
    dlist_pop_back(list);	  
  } 
  else {
    struct link *temp = list->beg;
    list->beg = list->beg->succ;
    list->beg->prev = 0;
    free(temp->val);
    free(temp);
    --list->size;
  }
}

/* inserts a link with the value pointed to by value_ptr at the position
   just before link_addr in the link.
   returns address of inserted link     */
struct link *dlist_insert(struct doubly_linked_list *list, struct link *link_addr, 
	    void* value_ptr)
{
  if (link_addr==list->beg) {
    dlist_push_front(list, value_ptr);
    return list->beg;
  }
  else if (link_addr==list->end) {
    dlist_push_back(list, value_ptr);
    return list->end->prev;
  }
  else {
    struct link *newlink_addr = (struct link *)malloc(sizeof(struct link));
    newlink_addr->val = malloc(list->elem_size);
    memcpy(newlink_addr->val, value_ptr, list->elem_size);
    newlink_addr->succ = link_addr;
    newlink_addr->prev = link_addr->prev;
    link_addr->prev->succ = newlink_addr;
    link_addr->prev = newlink_addr;
    ++list->size;
    return newlink_addr;
  }
}

 
/* erases the link from the list pointed to by link_addr.
   link_addr MUST NOT BE list->end, if someone passes this the fucntion does nothing  */
void dlist_erase(struct doubly_linked_list *list, struct link *link_addr)
{
  /* dummy safe */
  if (link_addr==list->end)
    return;
  if ( (link_addr==0) ||   (list->size==0)  )
    return;
  else if (link_addr==list->beg) {
    dlist_pop_front(list);
  }
  else {
    link_addr->prev->succ = link_addr->succ;
    link_addr->succ->prev = link_addr->prev;
    free(link_addr->val);
    free(link_addr);
    --list->size;
  }
}

void dlist_erase_all(struct doubly_linked_list *list, void *elem)
{
	while (1) {
                struct link *curr_link = list->beg;
                unsigned char deleted_link = 0;
                while (curr_link != (list->end)) {
                        if (list->compare(curr_link->val, elem) == 0) {
                                dlist_erase(list, curr_link);
                                deleted_link = 1;
                                break;
                        }
                        curr_link = curr_link->succ;
                }
                if (deleted_link == 1) {
                        continue;
                }
                else {
                        break;
                }
        }
}

int dlist_size(struct doubly_linked_list *list)
{
  return list->size;
}

int dlist_data_size(struct doubly_linked_list *list)
{
  return list->elem_size;
}

struct link *dlist_beg(struct doubly_linked_list *list)
{
  return list->beg;
}

struct link *dlist_end(struct doubly_linked_list *list)
{
  return list->end;
}

void *dlist_get_val(struct link *l)
{
	return l->val;
}

// returns the address of l's successor
struct link *dlist_succ(struct link *l)
{
	return l->succ;
}


/* empties container  (size==0) */
void clear_dlist(struct doubly_linked_list *list)
{
  while (dlist_size(list))
    dlist_erase(list, dlist_beg(list));
}

// frees entire memory allocated for the list.
void free_dlist(struct doubly_linked_list *list)
{
  struct link *currLink = list->beg;
  while (currLink!=list->end) {
    struct link *temp = currLink;
    currLink = currLink->succ;
    free(temp->val);
    free(temp);
  }
  free(currLink);
}

void print_dlist(struct doubly_linked_list *list, void (*print)(void *))
{
  if ( (list->size) > 0) {
    struct link *curr_link = list->beg;
    while (curr_link!=list->end) {
      print(curr_link->val);
      printf(" ");
      curr_link = curr_link->succ;
    }
  }
}

// return the value pointer of the link with 0 starting position
// index. where index is less than the size of list.
// index==0 returns beg->val
void *dlist_at(struct doubly_linked_list *list, int index)
{

}
