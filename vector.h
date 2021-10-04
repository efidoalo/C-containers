/*=====================================;
 *  
 *  File: vector.h
 *  Content: header file to be included
 *  when using the vector data structure
 *  Date: 30/09/2018
 *
 **************************************/

#ifndef __vector_h_included__
#define __vector_h_included__

struct vector;

// creates a vector with capacity 10, size 0. Each elem being of size data_size bytes
struct vector *vector_null_init(int data_size);

// Returns the current size (how many elements are stored in the vector)
int vector_get_size(struct vector *vect);

// appends to the end of the vector the value at address data
// if size==capacity then the capacity is extended (doubled) to hold
// the appended data
void vector_push_back(struct vector *vect, void *data);

// function that removes the last element in the vector and resets
// the vectors size accordingly
void vector_pop_back(struct vector *vect);

// inserts a data element at the front of the vector
void vector_push_front(struct vector *vect, void *data);

//remove the first data element of the vector
void vector_pop_front(struct vector *vect);

// write the data element at address data to the vector vect at 0 starting index index.
// vect[index] = *data
// index must to be within [0, vect->size-1]
void vector_write(struct vector *vect, int index, void *data);

// returns the address of the element inside vect having 0 starting index index.
// index must to be within [0, vect->size-1]
void *vector_read(struct vector *vect, int index);

// Returns the zero starting index of the data element that is identical in its
// contents to the data at address data. If no element is identical to bytes
// at data then -1 is returned. If data is a null pointer -1 is returned.
int vector_search(struct vector *vect, void *data);

// Remove from vect the element having index index (0 starting) where index is
// in the inclusive range [0, vect->size-1]
void vector_remove(struct vector *vect, int index);

// sorts the vector vector where compare is a function that takes the
// addresses of two data types of size vect->data_size bytes and returns
// the address of the data value having 'greater' valued. If both data 
// instances are considered equal then compare returns the null pointer.
void vector_sort(struct vector *vect, void *(*compare)(void *, void *));

// Function that returns a dynamically allocated array of length
// (v->data_size)*(v->size) containing all of the contents of v. This array
// must be eventually freed. If v->size ==0, 0 is returned (null pointer)
void *vector_get_elems(struct vector *v);

// frees all memory associated with vect
void vector_free(struct vector *vect);
#endif
