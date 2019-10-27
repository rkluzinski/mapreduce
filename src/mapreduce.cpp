#include <iostream>     // for debugging

#include <map>          // for std::multimap
#include <vector>       // for std::vector

#include <unistd.h>     // for stat syscall
#include <sys/stat.h>   // for struct stat data type

#include <pthread.h>    // for mutexes

// avoid name mangling C headers library
extern "C" {
#include "mapreduce.h"
#include "threadpool.h"
}

struct MRData {
    std::size_t num_partitions;
    
    pthread_mutex_t *mutex;
    std::multimap<std::string, std::string> *partition;
    std::multimap<std::string, std::string>::iterator *partition_it;

    MRData(std::size_t n) {
        num_partitions = n;
        mutex = new pthread_mutex_t[n];
        partition = new std::multimap<std::string, std::string>[n];
        partition_it = new std::multimap<std::string, std::string>::iterator[n];

        for (std::size_t i = 0; i < num_partitions; i++) {
            pthread_mutex_init(&mutex[i], NULL);
        }
    }

    ~MRData() {
        for (std::size_t i = 0; i < num_partitions; i++) {
            pthread_mutex_destroy(&mutex[i]);
        }

        delete[] mutex;
        delete[] partition;
        delete[] partition_it;
    }
};

// MapReduce shared data
// declared static so it cannot be accessed from other files
MRData *shared_data;

// Not passed to MR_ProcessPartition
// so stored as a global variable instead
Reducer g_reducer;

/**
 * 
 */
void Reducer_Entry(int *partition_number) {
    MR_ProcessPartition(*partition_number);
}

/**
 * 
 */
void MR_Map(int num_files, char *filenames[], Mapper map, int num_mappers) {
    // sort files by size in ascending order
    std::multimap<int, const char *> sorted_files;

    for (int i = 0; i < num_files; i++) {
        // if the file does not exist, disregard it
        struct stat statbuf;
        if (stat(filenames[i], &statbuf) == 0) {
            sorted_files.emplace(statbuf.st_size, filenames[i]);
        }
    }

    ThreadPool_t *mapperPool = ThreadPool_create(num_mappers);

    // push the files into the work queue in descending order
    for (auto i = sorted_files.rbegin(); i != sorted_files.rend(); i++) {
        ThreadPool_add_work(mapperPool, (thread_func_t) map, (void *) i->second);
    }

    ThreadPool_destroy(mapperPool);
}

/**
 * 
 */
void MR_Reduce(Reducer reducer, int num_reducers) {
    g_reducer = reducer;
    
    int *args = new int[num_reducers];
    ThreadPool_t *reducerPool = ThreadPool_create(num_reducers);
    
    for (int i = 0; i < num_reducers; i++) {
        args[i] = i;
        ThreadPool_add_work(reducerPool, (thread_func_t) Reducer_Entry, &args[i]);
    }
    
    ThreadPool_destroy(reducerPool);
    delete[] args;
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
    shared_data = new MRData(num_reducers);

    MR_Map(num_files, filenames, map, num_mappers);
    MR_Reduce(concate, num_reducers);

    delete shared_data;
}

/**
 * Writes a key-value pair to a partition
 * Parameters:
 *      key - The key to write to the partition
 *      value - The value to associate to that key
 */
void MR_Emit(char *key, char *value) {
    std::size_t index = MR_Partition(key, shared_data->num_partitions);
    pthread_mutex_lock(&shared_data->mutex[index]);
    shared_data->partition[index].emplace(key, value);
    pthread_mutex_unlock(&shared_data->mutex[index]);
}

/**
 * Assigns a key to a partition using a hash function
 * Uses DJB2 hashing algorithm provided with assignment specification
 * Parameters:
 *      key - The key to hash
 *      num_partitions - The total number of partitions
 */
unsigned long MR_Partition(char *key, int num_partitions) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++) != 0) {
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
    auto &partition = shared_data->partition[partition_number];
    /* const */ auto &it = shared_data->partition_it[partition_number];
    
    it = partition.begin();
    while(it != partition.end()) {
        char *key = (char *) it->first.c_str();
        g_reducer(key, partition_number);
    }
}

/**
 * Gets the next value for that key from the given partition
 * Parameters:
 *      key - The key to get the value from
 *      partition_number - The partition number to look in
 */
char *MR_GetNext(char *key, int partition_number) {
    /* const */ auto end = shared_data->partition[partition_number].end();
    /* const */ auto &it = shared_data->partition_it[partition_number];

    if (it != end && it->first.compare(key) == 0) {
        return (char *) (it++)->second.c_str();
    }
    else {
        return NULL;
    }
}