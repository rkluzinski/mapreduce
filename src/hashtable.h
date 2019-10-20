#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <pthread.h>

#include "vector.h"

typedef struct Node_t {
    Node_t *next;
    char *key;
    Vector_t vector;
} HashNode_t;

typedef struct {
    size_t num_buckets;
    HashNode_t *buckets;
} HashTable_t;

#endif