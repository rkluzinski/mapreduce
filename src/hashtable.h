#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>    // for boolean types
#include <stddef.h>     // for size_t
#include "vector.h"     // for Vector implementation

// HashTable key-value types
typedef char *ht_key_t;
typedef Vector_t *ht_value_t;

// what to return when key is not in table
extern const ht_value_t key_not_in_table;

/**
 * Stores a key-value pair
 * Used to implement HashTable
 */
typedef struct {
    ht_key_t key;       // stores the key of this bucket
    ht_value_t value;  // stores the values of this bucket
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
 * Returns:
 *      0 on success, -1 on failure
 */
int HashTable_insert(HashTable_t *hashtable, ht_key_t key, ht_value_t value);

/**
 * Get the value associated with the given key
 * Parameters:
 *      hashtable - The HashTable to search
 *      key - The key to search for
 * Returns:
 *      If key was found: value associated with that key
 *      Otherwise: value defined by key_not_in_table
 */
ht_value_t HashTable_get(HashTable_t *hashtable, ht_key_t key);

/**
 * Check if the HashTable contains a given key
 * Parameters:
 *      hashtable - The HashTable to search
 *      key - The key to search for
 * Returns:
 *      1 if the key is found, 0 otherwise
 */
int HashTable_contains(HashTable_t *hashtable, ht_key_t key);

#endif