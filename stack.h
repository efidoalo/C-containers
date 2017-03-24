/*==================================;
 *
 * File: stack.h
 * Contents: Definitions for the stack
 * data structure, to be used to 
 * contain arbitrary sized data types.
 * Date: 21/03/2016
 *
 ***********************************/

#ifndef __STACK_H_INCLUDED__
#define __STACK_H_INCLUDED__

#include <stdlib.h>
#include <string.h>

// stack grows upwards, pops downwards
struct stack {
  int size;      // number of objects currently contained 
  int data_size; // size in bytes of each object
  void *data;    // pointer to bottom of stack
  int capacity;  // max number of objects that can be contained
};

// data_size arguement is number of bytes of the
// data type to be stored in the stack container
struct stack *init_stack(int data_size)
{
  struct stack *new_stack = malloc(sizeof(struct stack));
  new_stack->data = malloc(data_size);
  new_stack->size=0;
  new_stack->data_size = data_size;
  new_stack->capacity=1;
  return new_stack;
}

// push the value at value_ptr onto the stack
void push(struct stack *stack, void *value_ptr)
{
  if (stack->capacity > stack->size) 
    memcpy( &(((char *)stack->data)[(stack->size)*(stack->data_size)]),
            value_ptr, 
            stack->data_size);
  else {
    stack->data = realloc(stack->data,
                          (stack->capacity)*(stack->data_size)*2);
    memcpy( &(((char *)stack->data)[(stack->size)*(stack->data_size)]),
            value_ptr,
            stack->data_size);
    (stack->capacity)*=2;
  }
  ++(stack->size);
}

// pops the topmost value off of the stack and returns a pointer
// to the object, or if the stack is empty does nothing and returns
// the null pointer. Non null returned pointer must be freed
void *pop(struct stack *stack)
{
  if ((stack->size)>0) 
    return memcpy(malloc(stack->data_size),
                  &(((char *)stack->data)[(--(stack->size))*(stack->data_size)]),
                  stack->data_size);
  else return 0;
}

// frees all memory allocated for the stack data structure
void free_stack(struct stack *stack)
{
  free(stack->data);
  free(stack);
}
  
#endif            

