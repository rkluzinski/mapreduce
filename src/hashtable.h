#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>    // for boolean types
#include <stddef.h>     // for size_t
#include "vector.h"     // for Vector implementation

/**
 * 
 */
typedef struct {
    char *key;          // stores the key of this bucket
    Vector_t *value;    // stores the values of this bucket
} Node_t;

/**
 * HashTable provides efficient insertion and lookup of key-value pairs
 * Each unique key is associated with a vector of non-unique values
 * Implementation using open-addressing and linear-probing
 * Dynamically resized once load factor exceed threshhold defined in hashtable.c
 */
typedef struct {
    size_t unique_keys;     // number of unique keys in the table
    size_t num_buckets;     // number of buckets
    Node_t **buckets;       // array of buckets
} HashTable_t;

/**
 * A C style contructor for HashTable
 * Returns:
 *      A pointer to the new HashTable
 */
HashTable_t *HashTable_create();

/**
 * A C style destructor for HashTable
 * Parameters:
 *      hashtable - The HashTable to be destroyed
 */
void HashTable_destroy(HashTable_t *hashtable);

/**
 * Insert an item into the HashTable using a given key
 * The value is appended to the Vector associated with given key
 * Parameters:
 *      hashtable - The HashTable to insert the key to
 *      key - The key to insert the item to
 *      value - The value to insert
 */
int HashTable_insert(HashTable_t *hashtable, char *key, vector_data_t value);

/**
 * Returns the Vector associated with a given key
 * Parameters:
 *      hashtable - The HashTable to search
 *      key - The key to search for
 * Returns:
 *      The Vector associated with the given key
 *      NULL if the key is not found;
 */
Vector_t *HashTable_get(HashTable_t *hashtable, char *key);

#endif