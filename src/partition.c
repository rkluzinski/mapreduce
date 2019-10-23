#include <stdlib.h>     // for malloc and free
#include "partition.h"

/**
 * C style constructor for data
 * Parameters:
 *      num_partitions - The number of partitions to create
 * Returns:
 *      Pointer the new data
 */
Partition_t *Partition_create(int num_partitions) {
    Partition_t *data = malloc(sizeof(Partition_t));

    if (data == NULL) {
        return NULL;
    }

    // initialize data
    data->num_partitions = num_partitions;
    data->locks = malloc(sizeof(pthread_mutex_t) * num_partitions);
    data->hashtables = malloc(sizeof(HashTable_t) * num_partitions);
    
    // initialize mutexs and HashTables
    for (int i = 0; i < num_partitions; i++) {
        pthread_mutex_init(&data->locks[i], NULL);
        data->hashtables[i] = HashTable_create();

        // TODO error checking for HashTable_create
    }

    return data;
}

/**
 * C style destructor for data
 * Parameters:
 *      data - data to be destroyed
 */
void Partition_destroy(Partition_t *data) {
    for (int i = 0; i < data->num_partitions; i++) {
        pthread_mutex_destroy(&data->locks[i]);
        HashTable_destroy(data->hashtables[i]);
    }

    free(data->hashtables);
    free(data->locks);
    free(data);
}

/**
 * Insert a key-value pair into a partition of data
 * Parameters:
 *      data - data to insert into
 *      partition - The partition to insert to
 *      key - The key to insert
 *      value - The value to insert
 */
int Partition_insert(Partition_t *data, int partition, pkey_t key, pvalue_t value) {
    pthread_mutex_lock(&data->locks[partition]);

    HashTable_t *hashtable = data->hashtables[partition];
    if (HashTable_contains(hashtable, key)) {
        Vector_t *vector = HashTable_get(hashtable, key);
        Vector_insert(vector, value);
    }
    else {
        Vector_t *vector = Vector_create();
        Vector_insert(vector, value);
        HashTable_insert(hashtable, key, vector);
    }

    pthread_mutex_unlock(&data->locks[partition]);
}

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
pvalue_t Partition_getNextValue(Partition_t *data, int partition, pkey_t key);