#include <stdlib.h>         // for malloc, free

#include "mapreduce.h"
#include "threadpool.h"     // for threadpool implementation
#include "sort.h"           // for sort_files
#include "partition.h"      // for threadsafe intermediate data structure

static PHashTable_t *phashtable;

/**
 * Executes MapReduce
 * Parameters:
 *      num_files - The length of the filenames array
 *      filenames - The array of files to processed
 *      map - The map function to apply to each file
 *      num_mappers - The number of mapper threads
 *      concate - The reduce function to apply to each file
 *      num_reducers - The number of reducer threads
 */
void MR_Run(int num_files, char *filenames[], 
            Mapper map, int num_mappers,
            Reducer concate, int num_reducers) {
    
    // sorts filenames and removes files that stat cannot open
    num_files = sort_by_size(filenames, num_files);

    // create the shared data structure
    phashtable = PHashTable_create(num_reducers);

    // create the threadpool of mappers
    ThreadPool_t *mapperPool = ThreadPool_create(num_mappers);

    // assign data splits to the mapper threads
    for (int i = 0; i < num_files; i++) {
        ThreadPool_add_work(mapperPool, (thread_func_t) map, filenames[i]);
    }

    ThreadPool_destroy(mapperPool);

    //reducer pool

    PHashTable_destroy(phashtable);
}

/**
 * Writes a key-value pair to a partition
 * Parameters:
 *      key - The key to write to the partition
 *      value - The value to associate to that key
 */
void MR_Emit(char *key, char *value) {
    unsigned long partition = MR_Partition(key, phashtable->num_partitions);
    PHashTable_insert(phashtable, partition, key, value);
}

/**
 * Assigns a key to a partition using a hash function
 * Uses DJB2 hashing algorithm provided with assignment spec
 * Parameters:
 *      key - The key to hash
 *      num_partitions - The total number of partitions
 */
unsigned long MR_Partition(char *key, int num_partitions) {
    unsigned long hash = 5381;
    for (int c = *key; *key != 0; key++) {
        hash = hash * 33 + c;
    }
    return hash % num_partitions;
}

/**
 * Processes a partition using the reducer function
 * Parameters:
 *      partition_number - The partition to process
 */
void MR_ProcessPartition(int partition_number) {
    // for all keys in partition
        // call reducer
        // delete key
}

/**
 * Gets the next value for that key from the given partition
 * Parameters:
 *      key - The key to get the value from
 *      partition_number - The partition number to look in
 */
char *MR_GetNext(char *key, int partition_number) {
    // return next value
}
