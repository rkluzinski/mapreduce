#include <stdlib.h>         // for malloc, free
#include <string.h>         // for strdup

#include "mapreduce.h"
#include "threadpool.h"     // for threadpool implementation
#include "sort.h"           // for sort_files

typedef struct {
    size_t position;
    Vector_t *vector;
} VectorIterator_t;

// intermediate data structure
typedef struct {
    size_t num_partitions;      // number of partitions
    pthread_mutex_t *locks;     // array of locks
    HashTable_t **hashtables;   // array of hashtables
    
    Reducer reducer;            // reducer function
    VectorIterator_t *vits;  // for iterating over values
} MRData_t;

MRData_t *shared_data;

/**
 * C stye constructor for MRData
 * Parameters:
 *      num_partitions - The number of partitions to create
 * Returns:
 *      A pointer to the new MRData
 *      NULL on failure
 */
MRData_t *MRData_create(int num_partitions, Reducer reducer) {
    MRData_t *data = malloc(sizeof(MRData_t));

    if (data != NULL) {
        data->num_partitions = num_partitions;
        data->locks = malloc(sizeof(pthread_mutex_t) * num_partitions);
        data->hashtables = malloc(sizeof(HashTable_t) * num_partitions);
       
        data->vits = malloc(sizeof(VectorIterator_t) * num_partitions);
        
        // initialize mutexs and HashTables
        for (int i = 0; i < num_partitions; i++) {
            pthread_mutex_init(&data->locks[i], NULL);
            data->hashtables[i] = HashTable_create();

            if (data->hashtables[i] == NULL) {
                // TODO error handle here
            }
        }

        data->reducer = reducer;
    }

    return data;
}

/**
 * A C style destructor for MRData
 * Parameters:
 *      data - The MRData to be destroyed
 */
void MRData_destroy(MRData_t *data) {
    for (int i = 0; i < data->num_partitions; i++) {
        HashTable_destroy(data->hashtables[i]);
        pthread_mutex_destroy(&data->locks[i]);
    }

    free(data->vits);

    free(data->hashtables);
    free(data->locks);
    free(data);
}

/**
 * Invokes MR_ProcessPartition in each thread.
 * Parameters:
 *      arg - Pointer to int that stores the partition number
 */
void Reducer_work(int *arg) {
    MR_ProcessPartition(*arg);
}

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
    
    // initialize data
    num_files = sort_by_size(filenames, num_files);
    shared_data = MRData_create(num_reducers, concate);

    // create and run mappers
    ThreadPool_t *mapperPool = ThreadPool_create(num_mappers);
    for (int i = 0; i < num_files; i++) {
        ThreadPool_add_work(mapperPool, (thread_func_t) map, filenames[i]);
    }
    ThreadPool_destroy(mapperPool);

    // create and run reducers
    int *args = malloc(sizeof(int) * num_reducers);
    ThreadPool_t *reducerPool = ThreadPool_create(num_reducers);
    
    for (int i = 0; i < num_reducers; i++) {
        args[i] = i;
        ThreadPool_add_work(reducerPool, (thread_func_t) Reducer_work, &args[i]);
    }
    
    ThreadPool_destroy(reducerPool);
    free(args);

    // destroy intermediate data structure
    MRData_destroy(shared_data);
}

/**
 * Writes a key-value pair to a partition
 * Parameters:
 *      key - The key to write to the partition
 *      value - The value to associate to that key
 */
void MR_Emit(char *key, char *value) {
    unsigned long partition = MR_Partition(key, shared_data->num_partitions);
    
    pthread_mutex_t *lock = &shared_data->locks[partition];
    HashTable_t *hashtable = shared_data->hashtables[partition];

    pthread_mutex_lock(lock);

    if (HashTable_contains(hashtable, key)) {
        Vector_t *vector = HashTable_get(hashtable, key);
        Vector_insert(vector, value);
    }
    else {
        key = strdup(key);
        Vector_t *vector = Vector_create();
        Vector_insert(vector, value);
        HashTable_insert(hashtable, key, vector);
    }

    pthread_mutex_unlock(lock);
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
    pthread_mutex_t *lock = &shared_data->locks[partition_number];
    HashTable_t *hashtable = shared_data->hashtables[partition_number];

    pthread_mutex_lock(lock);

    for (size_t i = 0; i < hashtable->num_buckets; i++) {
        char *key = hashtable->table[i].key;
        if (key != NULL) {
            Vector_t *vector = hashtable->table[i].value;
            VectorIterator_t *vit = &shared_data->vits[partition_number];
            
            vit->position = 0;
            vit->vector = hashtable->table[i].value;
            
            shared_data->reducer(key, partition_number);

            Vector_destroy(vector);
            free(key);
        }
    }

    pthread_mutex_unlock(lock);
}

/**
 * Gets the next value for that key from the given partition
 * Parameters:
 *      key - The key to get the value from
 *      partition_number - The partition number to look in
 */
char *MR_GetNext(char *key, int partition_number) {
    VectorIterator_t *vit = &shared_data->vits[partition_number];

    if (vit->position < Vector_size(vit->vector)) {
        return Vector_get(vit->vector, vit->position++);
    }
    else {
        return NULL;
    }
}
