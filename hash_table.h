/*==================================;
 *
 * File: hash_table.h
 * Contents: Hash  table that uses 
 * buckets of doubly linked lists to 
 * insert and traverse through its internal ekeents.
 * Hash table contains one type of data type/object type 
 * (possibly user defined), although this type is copeltely arbitrary
 * Date: 26/02/2017. not checked or tested.
 *
 *************************************/

#ifndef __HASH_TABLE_H_INCLUDED__
#define __HASH_TABLE_H_INCLUDED__

#include "doubly linked list.h"
#include "vector.h"
#include <stdio.h>

struct key_val_pair {
  void *key;
  void *val;
};

struct bucket {
  struct doubly_linked_list *pairs;   
};

struct hash_table
{
  int key_size;   // number of bytes of each key object
  int value_size; // number of bytes of each value object
 
  int size;       // number of (key, value) pairs currently stored
  int (*hash_func)(void *); /* hash function, takes as arguement the address
                              of the key to be hashed, and returns the hashed
                              value. Must be such that each unique key object
                              has a unique hash value. */
  
  void *(*key_compare)(void *key1_addr, 
                       void *key2_addr); /* function that compares the two keys
                                        located at addr1 and addr2 and returns
                                        the address whos key is larger, or
                                        if the keys are equal the null pointer
                                        is returned */
  struct bucket *buckets; 
  int NoOfBuckets; 
};


struct hash_table *init_hash_table(int key_size, 
                                   int value_size,
                                   int (*hash_fnc)(void *),  
                                   void *(*key_compare)(void *, void *),
                                   int NoOfBuckets)
{
  struct hash_table *new_htable = malloc(sizeof(struct hash_table));
  new_htable->key_size = key_size;
  new_htable->value_size = value_size;
  new_htable->hash_func = hash_fnc;
  new_htable->buckets = malloc( sizeof(struct bucket)*NoOfBuckets );
  for (int i=0; i<NoOfBuckets; ++i) 
    ((new_htable->buckets)[i]).pairs = init_dlist(sizeof(struct key_val_pair), 0); 
  new_htable->key_compare = key_compare;
  new_htable->NoOfBuckets = NoOfBuckets;
  new_htable->size = 0;
  return new_htable; 
}

// insert the key value pair with the key and values given by their addresses
// into the hash tablea given at table. After this call the pointers key and val may be freed.
// Returns 1 if pair is inserted, 0 otherwise (a value object is already in table with the the key at void *key)
int htable_insert(struct hash_table *table, void *key, void *val)
{ 
  int index = table->hash_func(key) % table->NoOfBuckets;
  struct doubly_linked_list *currBucket_pairs = ((table->buckets)[index]).pairs;
  int key_present = 0;
  if (dlist_size(currBucket_pairs)>0) {
    struct link *currLink = dlist_beg(currBucket_pairs), *endLink = dlist_end(currBucket_pairs);
    while (currLink!=endLink) {
      struct key_val_pair *currPair = currLink->val;
      if ( table->key_compare(currPair->key, key)== 0 ) {
        key_present = 1;
        break;
      }
      currLink = currLink->succ;
    }
    if (key_present)
      return 0;
    void *newkey = malloc(table->key_size), *newval = malloc(table->value_size);
    memcpy(newkey, key, table->key_size);
    memcpy(newval, val, table->value_size);
    struct key_val_pair newkeyvalpair = {newkey, newval};
    dlist_push_back(currBucket_pairs, &newkeyvalpair );
    ++(table->size);
    return 1; 
  }
  void *newkey = malloc(table->key_size), *newval = malloc(table->value_size);
  memcpy(newkey, key, table->key_size);
  memcpy(newval, val, table->value_size);
  struct key_val_pair newkeyvalpair = {newkey, newval};
  dlist_push_back(currBucket_pairs, &newkeyvalpair);
  ++(table->size);
  return 1;
}

// Return the address of (a copy of) the value within the table that
// has the key given at address void *key. Null pointer is returned if no keyvalpair is found
// in the table with a key value matching that at the address specified. If non null pointer
// is returned it must be freed eventually
void *htable_get_value(struct hash_table *table, void *key)
{
  int index = table->hash_func(key) % table->NoOfBuckets;
  struct doubly_linked_list *targetBucket = ((table->buckets)[index]).pairs;
  struct link *currLink = dlist_beg(targetBucket), *endLink = dlist_end(targetBucket);
  int key_present = 0;
  while (currLink!=endLink) {
    struct key_val_pair *currkeyvalpair = currLink->val;
    if (table->key_compare(currkeyvalpair->key, key)==0) {
      key_present = 1;
      break;
    }
    currLink = currLink->succ;
  }
  if (key_present) {
    void *value = malloc(table->value_size);
    memcpy(value, ((struct key_val_pair *)currLink->val)->val, 
                  table->value_size);
    return value;
  }
  return 0;
}

// Returns an allocated memory block (which must be fred), whereby the first 4 bytes
// define an unsigned integer representing the number of keys in the memory block 
// and the rest of the memory block is the keys allocated contiguously. If no keys are 
// found that match the specified value then the null poitner is returned. compare_val 
// is a function poitner that when dereferenced takes the addresses of two value 
// objects and returns the address of the greater value object, if both values 
// are equal then the function returns a null pointer.
void *htable_get_keys(struct hash_table *table, void *val, 
                              void *(*compare_val)(void *,void *))
{
  unsigned int key_counter=0;
  struct vector *keys = init_vect(0,0,table->key_size);
  for (int i=0; i<table->NoOfBuckets; ++i) {
    struct doubly_linked_list *currBucketInternals = ((table->buckets)[i]).pairs;
    if (dlist_size(currBucketInternals)>0) { 
      struct link *currLink = dlist_beg(currBucketInternals), 
                  *endLink = dlist_end(currBucketInternals);
      while (currLink!=endLink) {
        struct key_val_pair *currPair = (struct key_val_pair *)(currLink->val);
        if (compare_val(currPair->val, val)==0) { 
          vect_push_back(keys, ((struct key_val_pair *)(currLink->val))->key);
          ++key_counter;
        }
        currLink = currLink->succ;
      }
    } 
  }
  if (key_counter) {
    void *key_array = malloc(sizeof(unsigned int) + key_counter*(table->key_size));
    memcpy(key_array, &key_counter, sizeof(unsigned int));
    for (int i=0; i<key_counter; ++i) 
      memcpy(((char *)key_array) + sizeof(unsigned int) + i*(table->key_size),
             vect_read(keys, i), table->key_size);
    free_vect(keys);
    return key_array;
  } 
  return 0;
}

// remove the value in the hashtable with the key given at address void *key.
// Returns 1 if value is deleted or 0 if no value is deleted (ie no key
// exists in the table with the value at address void *key)
int htable_remVal_byKey(struct hash_table *table, void *key)
{
  int index = table->hash_func(key) % table->NoOfBuckets;
  struct doubly_linked_list *bucket_internals = ((table->buckets)[index]).pairs;  
  struct link *currLink = dlist_beg(bucket_internals), *endLink=dlist_end(bucket_internals);
  while (currLink!=endLink) {
    if (table->key_compare( ((struct key_val_pair *)(currLink->val))->key, key)==0) {
      dlist_erase(bucket_internals, currLink);
      --(table->size);
      return 1;
    }
    currLink = currLink->succ;
  }
  return 0;
}

// Removes all of the key-value pairs that have a value identical to the value
// given at the address void *val, function returns number of key-value pairs deleted
// from the hash table. Fucntion pointer compare_val is a function that takes the 
// addresses of two value objects and returns the addrss of the greater value, or 
// returns the null pointer if the value objects are of equal value
int htable_remVal_byVal(struct hash_table *table, void *val, 
                        void *(*compare_val)(void *,void *))
{
  int rem_counter =0;
  struct doubly_linked_list *currBucketPairs;
  for (int i=0; i<table->NoOfBuckets; ++i) {
    if (dlist_size((currBucketPairs = (table->buckets[i]).pairs))>0) {
      struct link *currLink = dlist_beg(currBucketPairs),
                  *endLink = dlist_end(currBucketPairs);
      while (currLink!=endLink) {
        if (compare_val(((struct key_val_pair *)(currLink->val))->val,val)==0) {
          dlist_erase(currBucketPairs, currLink);
          --(table->size);
          ++rem_counter;
        }
        currLink=currLink->succ;
      }
    }
  } 
  return rem_counter;
}

// frees all of the memory associated with key-val pairs in the table so that the size
// of the table is zero
void empty_htable(struct hash_table *table)
{
  for (int i=0; i<table->NoOfBuckets; ++i) {
    struct doubly_linked_list *currBucketInternals = (table->buckets[i]).pairs;
    if (dlist_size(currBucketInternals)>0) {
      struct link *currLink = dlist_beg(currBucketInternals),
                  *endLink = dlist_end(currBucketInternals);
      while (currLink!=endLink) {
        struct key_val_pair *currpair = (struct key_val_pair *)currLink->val;
        free(currpair->key);
        free(currpair->val);
        currLink = currLink->succ;
      } 
      clear_dlist(currBucketInternals);
    }
  }
  table->size = 0;
}

// free all resources taken by the hash_tale object at table.
void free_htable(struct hash_table *table)
{
  struct doubly_linked_list *currBucketInternals;
  for (int i=0; i<table->NoOfBuckets; ++i) {
    if (dlist_size((currBucketInternals = ((table->buckets)[i]).pairs))>0) {
      struct link *currLink = dlist_beg(currBucketInternals),
                  *endLink = dlist_end(currBucketInternals);
      while (currLink!=endLink) {
        free( ((struct key_val_pair *)currLink->val)->key);
        free( ((struct key_val_pair *)currLink->val)->val);
        currLink = currLink->succ;
      }
    }
    free_dlist(currBucketInternals);
  }
  free(table->buckets);
  free(table);
}

#endif 
