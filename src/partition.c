#include <stdlib.h>     // for malloc and free
#include "partition.h"

/**
 * C style constructor for PHashTable
 * Parameters:
 *      num_partitions - The number of partitions to create
 * Returns:
 *      Pointer the new PHashTable
 */
PHashTable_t *PHashTable_create(int num_partitions) {
    PHashTable_t *phashtable = malloc(sizeof(PHashTable_t));

    if (phashtable == NULL) {
        return NULL;
    }

    // initialize PHashTable
    phashtable->num_partitions = num_partitions;
    phashtable->locks = malloc(sizeof(pthread_mutex_t) * num_partitions);
    phashtable->hashtables = malloc(sizeof(HashTable_t) * num_partitions);
    
    // initialize mutexs and HashTables
    for (int i = 0; i < num_partitions; i++) {
        pthread_mutex_init(&phashtable->locks[i], NULL);
        phashtable->hashtables[i] = HashTable_create();

        // TODO error checking for HashTable_create
    }

    return phashtable;
}

/**
 * C style destructor for PHashTable
 * Parameters:
 *      phashtable - PHashTable to be destroyed
 */
void PHashTable_destroy(PHashTable_t *phashtable) {
    for (int i = 0; i < phashtable->num_partitions; i++) {
        pthread_mutex_destroy(&phashtable->locks[i]);
        HashTable_destroy(phashtable->hashtables[i]);
    }

    free(phashtable->hashtables);
    free(phashtable->locks);
    free(phashtable);
}

/**
 * Insert a key-value pair into a partition of PHashTable
 * Parameters:
 *      phashtable - PHashTable to insert into
 *      partition - The partition to insert to
 *      key - The key to insert
 *      value - The value to insert
 */
int PHashTable_insert(PHashTable_t *phashtable, int partition, pkey_t key, pvalue_t value) {
    pthread_mutex_lock(&phashtable->locks[partition]);

    HashTable_t *hashtable = phashtable->hashtables[partition];
    if (HashTable_contains(hashtable, key)) {
        Vector_t *vector = HashTable_get(hashtable, key);
        Vector_insert(vector, value);
    }
    else {
        Vector_t *vector = Vector_create();
        Vector_insert(vector, value);
        HashTable_insert(hashtable, key, vector);
    }

    pthread_mutex_unlock(&phashtable->locks[partition]);
}

/**
 * Delete a key from PHashTable
 * Parameters:./
 *      phashtable - PHashTable to insert to
 *      partition - THe partition to delete from
 *      key - The key to delete
 */
int PHashTable_delete(PHashTable_t *phashtable, int partition, pkey_t key) {
    int deleted;
    pthread_mutex_lock(&phashtable->locks[partition]);
    
    HashTable_t *hashtable = phashtable->hashtables[partition];
    if (HashTable_contains(hashtable, key)) {
        // delete key from Hashtable
        Vector_t *vector = HashTable_get(hashtable, key);
        Vector_destroy(vector);
        deleted = 1;
    }
    else {
        deleted = 0;
    }

    pthread_mutex_unlock(&phashtable->locks[partition]);
    return deleted;
}

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
pvalue_t PHashTable_getNext(PHashTable_t *phashtable, int partition, pkey_t key);