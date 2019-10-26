#include <iostream>     // for debugging
#include <vector>       // for std::vector
#include <map>          // for std::multimap

#include <unistd.h>     // for stat
#include <sys/stat.h>   // for struct stat data type

#include <pthread.h>    // for mutexs

// avoid name mangling the mapreduce library
extern "C" {
#include "mapreduce.h"
#include "threadpool.h"
}

class MapReduce {
private:
    std::vector<pthread_mutex_t> locks;
    std::vector<std::multimap<std::string, std::string>> partitions;

    Mapper map;
    Reducer reduce;

public:
    MapReduce();
    ~MapReduce();

    void emit(std::string key, std::string value);
    std::string getNext(std::string key, int partition_num);
};

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
    
    // sort files by size using std::map
    struct stat statbuf;
    std::map<int, const char *> sorted_files;

    // get the size of each file and push it to the map
    for (int i = 0; i < num_files; i++) {
        // if the file does not exist disregard it
        if (stat(filenames[i], &statbuf)) {
            sorted_files.emplace(statbuf.st_size, filenames[i]);
        }
    }

    // create mappers
    ThreadPool_t *mapperPool = ThreadPool_create(num_mappers);

    // push the files into the work queue descending order
    for (auto i = sorted_files.rbegin(); i != sorted_files.rend(); i++) {
        ThreadPool_add_work(mapperPool, (thread_func_t) map, (void *) i->second);
    }

    // destroy mappers
    ThreadPool_destroy(mapperPool);
}

/**
 * Writes a key-value pair to a partition
 * Parameters:
 *      key - The key to write to the partition
 *      value - The value to associate to that key
 */
void MR_Emit(char *key, char *value) {

}

/**
 * Assigns a key to a partition using a hash function
 * Uses DJB2 hashing algorithm provided with assignment spec
 * Parameters:
 *      key - The key to hash
 *      num_partitions - The total number of partitions
 */
unsigned long MR_Partition(char *key, int num_partitions) {

}

/**
 * Processes a partition using the reducer function
 * Parameters:
 *      partition_number - The partition to process
 */
void MR_ProcessPartition(int partition_number) {

}

/**
 * Gets the next value for that key from the given partition
 * Parameters:
 *      key - The key to get the value from
 *      partition_number - The partition number to look in
 */
char *MR_GetNext(char *key, int partition_number) {

}