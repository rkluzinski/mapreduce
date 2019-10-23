#ifndef PARTITION_H
#define PARTITION_H

#include <pthread.h>

#include "vector.h"
#include "hashtable.h"

// typedef key-value data types
typedef hkey_t pkey_t;
typedef vec_data_t pvalue_t;

/**
 * Thread safe partitioned HashTable
 */
typedef struct {
    int num_partitions;
    pthread_mutex_t *locks;
    HashTable_t **hashtables;
} PHashTable_t;

/**
 * C style constructor for PHashTable
 * Parameters:
 *      num_partitions - The number of partitions to create
 * Returns:
 *      Pointer the new PHashTable
 */
PHashTable_t *PHashTable_create(int num_partitions);

/**
 * C style destructor for PHashTable
 * Parameters:
 *      phashtable - PHashTable to be destroyed
 */
void PHashTable_destroy(PHashTable_t *phashtable);

/**
 * Insert a key-value pair into a partition of PHashTable
 * Parameters:
 *      phashtable - PHashTable to insert into
 *      partition - The partition to insert to
 *      key - The key to insert
 *      value - The value to insert
 */
int PHashTable_insert(PHashTable_t *phashtable, int partition, pkey_t key, pvalue_t value);

/**
 * Delete a key from PHashTable
 * Parameters:
 *      phashtable - PHashTable to insert to
 *      partition - THe partition to delete from
 *      key - The key to delete
 */
int PHashTable_delete(PHashTable_t *phashtable, int partition, pkey_t key);

/**
 * Get a value from the partition of PHashTable
 * Will segfault if items does not exist
 * Parameters:
 *      phashtable - PHashTable to search
 *      partition - The partition to seach
 *      key - The key to search for
 * Returns:
 *      Value associated with the key
 */
pvalue_t PHashTable_get(PHashTable_t *phashtable, int partition, pkey_t key);

#endif