/*================================;
 *  
 *  File: vector.c
 *  Content: Function and object
 *  definitions for the vector
 *  data structure.
 *  Date: 30/09/2018
 *
 ********************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>

struct vector { 
  int size;      // number of elements that the vector is holding
  int capacity;  // number of possible elements the vector can hold
  int data_size; // size of each element in bytes
  void *elem;    // dynamically allocated block of memory that holds the elements
  void (*print) (void *v); // Function that prints to the terminal the value
			 // of the element whose address is givien in the 
			 // arguement. (This function is used to print all
			 // elements in the vector)  
};



int vector_search(struct vector *v, void *data)
{
  if (!data) 
    return -1;
  int index = -1;
  unsigned char *d = (unsigned char *)data;
  unsigned char *curr_elem = (unsigned char *)(v->elem);
  for (int i=0; i<(v->size); ++i) {
    int match = 1;
    for (int j=0; j<(v->data_size); ++j) {
      if (curr_elem[((v->data_size)*i) + j] != d[j]) {
        match = 0;
	break;
      } 
    }
    if (match==1) {
      return i;
    }
  }
  return -1;
}

// creates a vector with capacity 10, size 0. Each elem being of size data_size bytes
struct vector *vector_null_init(int data_size, void (*print) (void *v)) {

  struct vector *vect = malloc(sizeof(struct vector));
  vect->size = 0;
  vect->capacity = 10;
  vect->data_size = data_size;
  vect->elem = malloc(10*data_size);
  memset(vect->elem, 0, 10*data_size);
  vect->print = print;
  return vect;

}

// Returns the current size (how many elements are stored in the vector)
int vector_get_size(struct vector *vect) {
  return vect->size;
}

// appends to the end of the vector the value at address data
// if size==capacity then the capacity is extended (doubled) to hold
// the appended data
void vector_push_back(struct vector *vect, void *data) {
  
  if ( (vect->size) < (vect->capacity) ) {
    memcpy(&(((unsigned char *)vect->elem)[(vect->size)*(vect->data_size)]), data, vect->data_size);
    vect->size = vect->size + 1;
    return;
  }
  else {
    void *new_elem = malloc( (vect->data_size)*(vect->capacity)*2 );
    memcpy(new_elem, vect->elem, (vect->capacity)*(vect->data_size) );
    free(vect->elem);
    vect->elem = new_elem;
    memcpy(&(((unsigned char *)vect->elem)[(vect->size)*(vect->data_size)]), data, vect->data_size);
    vect->size = vect->size + 1;
    vect->capacity = (vect->capacity)*2;
    return;
  }

}

// function that removes the last element in the vector and resets
// the vectors size accordingly
void vector_pop_back(struct vector *vect) {
  if (vect->size==0) {
    return;
  }
  else { // if vect->size > 0
    memset(&(((unsigned char *)vect->elem)[(vect->size-1)*(vect->data_size)]), 0, vect->data_size);
    vect->size = vect->size - 1;
    return;
  }
}


// inserts a data element at the front of the vector 
void vector_push_front(struct vector *vect, void *data) {
  if ( (vect->size) < (vect->capacity) ) {
    for (int i=0; i<vect->size; ++i) {
      memcpy(&(((unsigned char *)vect->elem)[(vect->size -i)*(vect->data_size)]) , 
             &(((unsigned char *)vect->elem)[(vect->size-1-i)*(vect->data_size)]), 
	     vect->data_size);
    }    
    memcpy(vect->elem, data, vect->data_size);
    vect->size = vect->size + 1;
    return;
  }
  else { // if size == capacity
    void *new_elem = malloc( (vect->capacity)*(vect->data_size)*2 );
    memcpy(&(((unsigned char *)new_elem)[vect->data_size]), vect->elem, (vect->size)*(vect->data_size));
    memcpy(new_elem, data, vect->data_size);
    free(vect->elem);
    vect->elem = new_elem;
    vect->size = vect->size + 1;
    vect->capacity = vect->capacity*2;
  }
}

//remove the first data element of the vector
void vector_pop_front(struct vector *vect) {
  if (vect->size==0) {
    return;
  }
  else if ((vect->size)==1) {
    memset(vect->elem, 0, vect->data_size);
    vect->size = 0;
    return;
  }
  else if ((vect->size)>1) {
    for (int i=0; i<((vect->size)-1); ++i) {
      memcpy(&(((unsigned char *)vect->elem)[i*(vect->data_size)]), 
             &(((unsigned char *)vect->elem)[(i+1)*(vect->data_size)]), 
	     vect->data_size);
    }
    memset(&(((unsigned char *)vect->elem)[((vect->size)-1)*(vect->data_size)]), 0, vect->data_size);
    vect->size = vect->size - 1;
    return;
  }
}

// write the data element at address data to the vector vect at 0 starting index index.
// vect[index] = *data
// index must to be within [0, vect->size-1]
void vector_write(struct vector *vect, int index, void *data) {
  memcpy(&(((unsigned char *)vect->elem)[(vect->data_size)*index]), data, vect->data_size);
}

// returns the address of the element inside vect having 0 starting index index.
// index must to be within [0, vect->size-1]
void *vector_read(struct vector *vect, int index) {
  return &(((unsigned char *)vect->elem)[index*(vect->data_size)]);
}

// remove from vector v the element having index index where index is in the
// inclusive range [0, v->size-1]. The size of vector is decremented by 1 and
// if index lies outside the valid raneg then the function does nothing.
void vector_remove(struct vector *v, int index)
{
  if (index == 0) {
    vector_pop_front(v);
    return;
  }
  else if ( (index > 0) && (index < (v->size)) ) {
    int new_elem_len = (v->capacity)*(v->data_size);
    unsigned char *new_elem = malloc(new_elem_len);
    memset(new_elem, 0, new_elem_len);
    memcpy(new_elem, v->elem, index*(v->data_size));
    memcpy(&(new_elem[index*(v->data_size)]), &(((unsigned char *)(v->elem))[(index+1)*(v->data_size)]), (v->size - index - 1)*(v->data_size));
    free(v->elem);
    v->elem = new_elem;
    v->size -= 1;
    return;
  }
  else {
    return;
  }
}

// compare takes the addresss of two data types as arguements and returns the 
// address of the data type having the greater value. Compare returns null if
// the two data instances are considered as identical. This function sorts the
// data elements in vector v in ascending order.
void vector_sort(struct vector *v, void *(*compare)(void *, void *))
{
  if ((v->size) == 0)
    return;
  unsigned char *new_elem = (unsigned char *)malloc((v->data_size)*(v->capacity));
  memset(new_elem, 0, (v->data_size)*(v->capacity));
  int new_elem_index = 0;
  while (vector_get_size(v)) {
    void *min = vector_read(v,0);
    int index = 0;
    for (int i=0; i<vector_get_size(v); ++i) {
      if (compare(min, vector_read(v,i))==min) {
        min = vector_read(v,i);
	index = i;
      }
    }
    vector_remove(v, index);
    memcpy(&(new_elem[new_elem_index*(v->data_size)]), min, v->data_size);
    ++new_elem_index;
  }
  free(v->elem);
  v->elem = new_elem;
  v->size = new_elem_index;    
}


void *vector_get_elems(struct vector *v)
{
  if ((v->size)==0) {
    return 0;
  }
  else {
    unsigned char *elems = (unsigned char *)malloc((v->size)*(v->data_size));
    memcpy(elems, v->elem, (v->size)*(v->data_size));
    return (void *)elems;
  }
}

void vector_print(struct vector *vect)
{
	for (int index=0; index<(vect->size); ++index) {
		vect->print(&(((unsigned char *)vect->elem)[index*(vect->data_size)]));
		printf(" ");
	}
}

// frees all memory associated with vector pointed to by vect
void vector_free(struct vector *vect)
{
  free(vect->elem);
  free(vect);
}

void vector_insert(struct vector *vect, void *(*cmp)(void *, void *), void *d)
{
	if ((vect->size) == 0) {
		memcpy(vect->elem, d, vect->data_size);
		vect->size = vect->size + 1;
		return;
	}
	else {
		uint8_t vector_already_sorted = 1;
		for (int i=0; i<(vector_get_size(vect)-1); ++i) {
			void *ith_elem = &(((unsigned char *)(vect->elem))[i*(vect->data_size)]);
			void *iplus_one_elem = &(((unsigned char *)vect->elem)[(i+1)*(vect->data_size)]);
			void *cmp_res = cmp(ith_elem, iplus_one_elem);
			if (cmp_res == ith_elem) {
				vector_already_sorted = 0;
				break;
			}
		}
		if (!vector_already_sorted) {
			vector_sort(vect, cmp);
		}
		if (vector_search(vect, d) >= 0) {
			// d already in vect
			return;
		}
		if ((vect->size) == (vect->capacity)) {
			void *new_elem = malloc((vect->capacity)*2*(vect->data_size));
			if (new_elem == NULL) {
				printf("Error allocating memory during vector"
				       " insertion. %s.\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			memcpy(new_elem, vect->elem, (vect->capacity)*(vect->data_size)	);
			free(vect->elem);
			vect->elem = new_elem;
			vect->capacity = (vect->capacity)*2;
		}
		if ((vect->size) == 1) {
			if (cmp(&(((unsigned char *)(vect->elem))[0]), d) == d) {
				memcpy(&(((unsigned char *)(vect->elem))[1*(vect->data_size)]),
				       d,
		       		       vect->data_size);			
			}
			else {
				memcpy(&(((unsigned char *)(vect->elem))[1*(vect->data_size)]),
				       &(((unsigned char *)(vect->elem))[0]), vect->data_size);
				memcpy(&(((unsigned char *)(vect->elem))[0]), d, vect->data_size);
			}
			vect->size = vect->size + 1;
			return;
		}
		int lower_index = 0;
		int upper_index = (vect->size)-1;
		int middle_index = (lower_index + upper_index)/2;
		void *least_elem = &(((unsigned char *)(vect->elem))[0]);
		void *largest_elem = &(((unsigned char *)(vect->elem))[((vect->size)-1)*(vect->data_size)]);

		if (cmp(least_elem, d) == least_elem) {
			for (int i=(vect->size); i>0; --i) {
				memcpy(&(((unsigned char *)(vect->elem))[i*(vect->data_size)]),
				       &(((unsigned char *)(vect->elem))[(i-1)*(vect->data_size)]),
			               vect->data_size);	       
			}		
			memcpy(&(((unsigned char *)(vect->elem))[0]), d, vect->data_size);
			vect->size = vect->size + 1;
			return;
		}
		if (cmp(largest_elem, d)==d) {
			//vector_print(vect);
			//printf("here0\n");
			memcpy(&(((unsigned char *)(vect->elem))[(vect->size)*(vect->data_size)]),
				d,
				vect->data_size);
			vect->size = vect->size + 1;
			return;
		}
		while (1) {
			void *middle_elem = &(((unsigned char *)(vect->elem))[middle_index*(vect->data_size)]);
			void *cmp_res = cmp(middle_elem,
					    d);
			if ((cmp_res == d) && ((middle_index + 1)==(upper_index))) {
				// d is between middle_index and upper_index
				for (int i= vect->size; i>middle_index+1; --i) {
					memcpy(&(((unsigned char *)(vect->elem))[i*(vect->data_size)]),
					       &(((unsigned char *)(vect->elem))[(i-1)*(vect->data_size)]),
					       vect->data_size);
				}
				memcpy(&(((unsigned char *)(vect->elem))[(middle_index+1)*(vect->data_size)]), d, vect->data_size);
				vect->size = vect->size + 1;
				return;
			}
			if ((cmp_res == middle_elem) && ((middle_index -1)==(lower_index))) {
                                // d is between middle_index and lower_index
                                for (int i= vect->size; i>middle_index; --i) {
                                        memcpy(&(((unsigned char *)(vect->elem))[i*(vect->data_size)]),
                                               &(((unsigned char *)(vect->elem))[(i-1)*(vect->data_size)]),
                                               vect->data_size);
                                }
                                memcpy(&(((unsigned char *)(vect->elem))[middle_index*(vect->data_size)]), d, vect->data_size);
                                vect->size = vect->size + 1;
                                return;
                        }
			if (cmp_res == d) {
				lower_index = middle_index;
			}
			else if (cmp_res == middle_elem) {
				upper_index = middle_index;			
			}
			middle_index = (lower_index + upper_index)/2;	
		}
	}
}
