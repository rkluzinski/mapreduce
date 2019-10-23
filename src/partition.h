#ifndef PARTITION_H
#define PARTITION_H

#include <pthread.h>

#include "vector.h"
#include "hashtable.h"

// typedef key-value data types
typedef hkey_t pkey_t;
typedef vec_data_t pvalue_t;

/**
 * Thread safe Intermediate Data Structure
 */
typedef struct {
    int num_partitions;
    
    pthread_mutex_t *locks;
    HashTable_t **hashtables;

    // iterators over vectors
} Partition_t;

/**
 * C style constructor for data
 * Parameters:
 *      num_partitions - The number of partitions to create
 * Returns:
 *      Pointer the new data
 */
Partition_t *Partition_create(int num_partitions);

/**
 * C style destructor for data
 * Parameters:
 *      data - data to be destroyed
 */
void Partition_destroy(Partition_t *data);

/**
 * Insert a key-value pair into a partition of data
 * Parameters:
 *      data - data to insert into
 *      partition - The partition to insert to
 *      key - The key to insert
 *      value - The value to insert
 */
int Partition_insert(Partition_t *data, int partition, pkey_t key, pvalue_t value);

/**
 * Get a value from the partition of data
 * Will segfault if items does not exist
 * Parameters:
 *      data - data to search
 *      partition - The partition to seach
 *      key - The key to search for
 * Returns:
 *      Value associated with the key
 */
pvalue_t Partition_get(Partition_t *data, int partition, pkey_t key);

#endif