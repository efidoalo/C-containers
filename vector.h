/*================================================;
 *
 * File: vector.h
 * Content: implementation of the vector container
 * for a generic data type. [no range checking performed]
 * Date: 25/10/2016, ammended and re-tested 12/02/2017
 *
 *************************************************/

#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <string.h>

#define VECT_INITCAPACITY 10   /* initial capacity of the 
                                  container, used during
				  first call to push_back, 
                                  must be greater than 1 */

struct vector 
{
  void *elem;

  int size;
  int capacity;
  int elem_size;
};

/* append vector container with the value val */
void vect_push_back(struct vector *vect, void *value_ptr)
{
  /* if size=capacity=0 */
  if (vect->capacity==0) {
    vect->elem = malloc( vect->elem_size*VECT_INITCAPACITY );
    vect->size = 1;
    vect->capacity = VECT_INITCAPACITY;
    memcpy(vect->elem, value_ptr, vect->elem_size);
  } 
  else if (vect->size==vect->capacity) {
    void *temp = malloc( vect->elem_size*2*vect->capacity );
    for (int i=0; i<vect->size*vect->elem_size; i+=vect->elem_size) 
      memcpy(((char *)temp)+i, 
             ((char *)vect->elem)+i, vect->elem_size);

    free(vect->elem);
    vect->elem = temp;
    memcpy(((char *)vect->elem)+((vect->size++)*vect->elem_size), 
           value_ptr, vect->elem_size);

    vect->capacity*=2;
  }
  else memcpy(((char *)vect->elem)+((vect->size++)*vect->elem_size), 
              value_ptr, vect->elem_size);
}

/* decrements vec->size by the value of n, 
   no reallocation is performed so that
   the capacity of the vector is unchanged.  */
void vect_pop_back(struct vector *vec, unsigned int n)
{
  if (vec->size>=n) 
    vec->size-=n; 
}

/* frees memory and sets size=capacity=0,
    */
void clear_vect(struct vector *vec) 
{
  if (vec->capacity>0) {
    free(vec->elem);
    vec->capacity=vec->size=0;
  }
}

/* changes the size of *vec, reallocation takes place
   if newsize>vec->capacity, in which case the existing 
   entries are copied over. Can be used on vectors with 
   capacity==0 to set the initial size (to save repetative
   reallocation if VEC_INITCAPACITY is too low for your needs). 
   use clear() instead of newsize=0. newsize must be greater
   thsan zero. */
void resize_vect(struct vector *vec, int newsize)
{
  if (newsize<=vec->capacity)
    vec->size=newsize;
  else if ((vec->capacity<newsize) && (vec->capacity>0)) {  
    void *temp=malloc(newsize*2*vec->elem_size);
    for (int i=0; i<vec->size*vec->elem_size; i+=vec->elem_size)
      memcpy((( char *)temp)+i,
              ((char *)vec->elem)+i, vec->elem_size);
    free(vec->elem);
    vec->elem = temp;
    vec->size=newsize;
    vec->capacity=2*newsize;
  } 
  else if (vec->capacity==0) {
    vec->elem = malloc(newsize*2*vec->elem_size);
    vec->size = newsize;
    vec->capacity = 2*newsize;
  }
}

/* element access [no range checking performed] */
void *vect_at(struct vector *vec, int index)
{
  return ((char *)vec->elem) + index*vec->elem_size;
}

void *vect_read(struct vector *vec, int index)
{
  return ((char *)vec->elem) + index*vec->elem_size;
}

void vect_write(struct vector *vec, int index, void *value_ptr)
{
  memcpy(((char *)vec->elem)+(index*vec->elem_size), 
         value_ptr, vec->elem_size);
}

/* sets the first size entries of the vec->elem 
   pointer that contain the entries to zero */
void vect_setnull(struct vector *vec)
{
  char *temp = (char *)malloc(vec->size*vec->elem_size);
  for (int i=0; i<vec->size*vec->elem_size; ++i)
    temp[i]=0;
  memcpy(vec->elem, temp, vec->size*vec->elem_size);
  free(temp);
}

/* returns vector of size size, 
   with all of these elemnts initialized to
   value_ptr, each elemnt is of size elem_size */
struct vector *init_vect(int size, void *value_ptr, int elem_size)
{
  struct vector *new_vect = malloc(sizeof(struct vector));
  new_vect->size=size;
  new_vect->capacity=2*size;
  new_vect->elem = 0;
  new_vect->elem_size = elem_size;
  if (size) {
    new_vect->elem = malloc(2*size*elem_size);
    for (int i=0; i<size; ++i)
    memcpy(((char *)new_vect->elem) + (i*elem_size),
           value_ptr, elem_size);
  }
  return new_vect;
}

int vect_size(struct vector *vec)
{
  return vec->size;
}

int vect_capacity(struct vector *vec)
{
  return vec->capacity;	
}

int vect_elem_size(struct vector *vec)
{
  return vec->elem_size;
}

/* frees/destroys the vector at address vect,
   that had been previously dynamically allocated */
void free_vect(struct vector *vect)
{
  clear_vect(vect);
  free(vect);
}
#endif
