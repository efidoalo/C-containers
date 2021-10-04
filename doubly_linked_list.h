/*=================================================;
 *
 * File: doubly linked list.h
 * Contents: type generic doubly linked list container
 * implementation, using void pointers.
 * Date: 26/10/2016
 *
 **************************************************/

#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H


struct link;

struct doubly_linked_list;

/* returns a doubly_linked_list initialized to 0,0,elem_size. use this function
   for initialization of doubly_linked_lists, elem_size=sizeof(type) */
struct doubly_linked_list *init_dlist(int elem_size, void *(*compare)(void *, void *));

/* inserts a link into the list at the end, entry/element value
   equal to *value_ptr */
void dlist_push_back(struct doubly_linked_list *list, void *value_ptr);

/* inserts link at front of list having value identical to that at address
   value_ptr. This function adjusts list->beg. */
void dlist_push_front(struct doubly_linked_list *list, void *value_ptr);

/* removes the final link from the list */
void dlist_pop_back(struct doubly_linked_list *list);

/* removes the first link from the list */
void dlist_pop_front(struct doubly_linked_list *list);

/* inserts a link with the value pointed to by value_ptr at the position
   just before link_addr in the link.
   returns address of inserted link     */
struct link *dlist_insert(struct doubly_linked_list *list, struct link *link_addr, 
	    void* value_ptr);
 
/* erases the link from the list pointed to by link_addr.
   link_addr MUST NOT BE list->end, if someone passes this the fucntion does nothing  */
void dlist_erase(struct doubly_linked_list *list, struct link *link_addr);

// Erase all links from list whose link->val member compares equal to
// elem using list->compare
void dlist_erase_all(struct doubly_linked_list *list, void *elem);

// return the size of the doubly linked list
int dlist_size(struct doubly_linked_list *list);

// return the data size of the dlist
int dlist_data_size(struct doubly_linked_list *list);

// return the begginning link of the dlist
struct link *dlist_beg(struct doubly_linked_list *list);
 
//return the end link of the dlist which is one beyond the last link
struct link *dlist_end(struct doubly_linked_list *list);

// returns the value of the member val (which is a void *) of link l.
void *dlist_get_val(struct link *l);

// returns the address of l's successor
struct link *dlist_succ(struct link *l);

/* empties container  (size==0) */
void clear_dlist(struct doubly_linked_list *list);

// frees entire memory allocated for the list.
void free_dlist(struct doubly_linked_list *list);

void print_dlist(struct doubly_linked_list *list, void (*print)(void *)); 
#endif

