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

class Partition {
    pthread_mutex_t mutex;
    std::multimap<std::string, std::string> data;
    std::multimap<std::string, std::string>::iterator it;

public:
    Partition() {
        pthread_mutex_init(&mutex, NULL);
        it = data.begin();
    }

    ~Partition() {
        pthread_mutex_destroy(&mutex);
    }

    void insert(char *key, char *value) {
        pthread_mutex_lock(&mutex);
        data.emplace(std::string(key), std::string(value));
        pthread_mutex_unlock(&mutex);
    }

    char *next(char *key) {
        if (it != data.end() && it->first.compare(key) == 0) {
            return (char *) (it++)->second.c_str();
        }
        else {
            return NULL;
        }
    }

    char *nextKey() {
        if (it != data.end()) {
            return (char *) it->second.c_str();
        }
        else {
            return NULL;
        }
    }

    bool finished() {
        return it == data.end();
    }
};

class MRData {
    std::size_t num_partitions;
    Partition *partitions;

public:
    MRData(std::size_t n) {
        num_partitions = n;
        partitions = new Partition[num_partitions];
    }

    ~MRData() {
        delete[] partitions;
    }

    std::size_t size() {
        return num_partitions;
    }

    void insert(std::size_t n, char *key, char *value) {
        partitions[n].insert(key, value);
    }

    char *next(std::size_t n, char *key) {
        return partitions[n].next(key);
    }

    char *nextKey(std::size_t n) {
        return partitions[n].nextKey();
    }

    bool finished(std::size_t n) {
        return partitions[n].finished();
    }
};

// MapReduce shared data
// declared static so it cannot be accessed from other files
static MRData *shared_data;

// Not passed to MR_ProcessPartition
// so stored as a global variable instead
static Reducer g_reducer;

// class MapReduce {
// private:
//     // shared data structure
//     std::vector<pthread_mutex_t> locks;
//     std::vector<std::multimap<std::string, std::string>> partitions;

//     // iterators used by getNext
//     std::vector<std::multimap<std::string, std::string>::iterator> iterators; 

// public:
//     MapReduce(int n_mappers, int n_reducers);
//     ~MapReduce();

//     // MapReduce functions
//     void map(Mapper mapper, int num_files, char *filenames[]);
//     void reduce(Reducer reducer);

//     // called by Mapper and Reducer threads
//     void emit(char* key, char* value);
//     void processPartition(int partition_number);
//     char *getNext(char* key, int partition_num);
// };

// // global MapReduce object
// static MapReduce *mapreduce;

// /**
//  * MapReduce Constructor
//  * Parameters:
//  *      n_mappers - The number of mapper threads to create
//  *      n_reducers - The number of reducer threads to create
//  */
// MapReduce::MapReduce(int n_mappers, int n_reducers) {
    
//     // create mutexes and multimaps
//     for (int i = 0; i < num_reducers; i++) {
//         locks.emplace_back();
//         pthread_mutex_init(&locks.back(), NULL);

//         partitions.emplace_back();
//         iterators.emplace_back();
//     }
// }

// /**
//  * MapReduce Destructor
//  */
// MapReduce::~MapReduce() {
//     // destory mutexes
//     for (auto &mutex : locks) {
//         pthread_mutex_destroy(&mutex);
//     }
// }

// void Reducer_entry(int *arg) {
//     MR_ProcessPartition(*arg);
// }

// /**
//  * Create and run reducer threads
//  * Parameters:
//  *      reducer - The reducer function to apply
//  */
// void MapReduce::reduce(Reducer reducer) {
//     reduce_fn = reducer;
//     ThreadPool_t *reducerPool = ThreadPool_create(num_reducers);

//     // arguments MR_ProcessPartition 
//     int *args = new int[num_reducers];
//     for (int i = 0; i < num_reducers; i++) {
//         // allows integer to be passed to new thread
//         args[i] = i;
//         ThreadPool_add_work(reducerPool, 
//             (thread_func_t) Reducer_entry, (void *) &args[i]);
//     }
//     delete args;

//     ThreadPool_destroy(reducerPool);
// }

// /**
//  * Emit a key-value pair to the shared data structure
//  * Parameters:
//  *      key - The key to emit
//  *      value - The value to emit
//  */
// void MapReduce::emit(char* key, char* value) {
//     unsigned long partition = MR_Partition(key, num_reducers);
//     pthread_mutex_lock(&locks.at(partition));
//     partitions[partition].emplace(std::string(key), std::string(value));
//     pthread_mutex_unlock(&locks.at(partition));
// }

// /**
//  * 
//  */
// void MapReduce::processPartition(int partition_number) {
//     pthread_mutex_lock(&locks.at(partition_number));
    
//     auto partition = partitions.at(partition_number);
//     auto i = partition.begin();
//     while (i != partition.end()) {
//         iterators.assign(partition_number, i);
//         reduce_fn((char *) (i++)->first.c_str(), partition_number);
//     }

//     pthread_mutex_unlock(&locks.at(partition_number));
// }

// /**
//  * 
//  */
// char *MapReduce::getNext(char *key, int partition_number) {
//     return NULL;
// }

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
    
    ThreadPool_t *reducerPool = ThreadPool_create(num_reducers);
    int *args = new int[num_reducers];
    
    for (int i = 0; i < num_reducers; i++) {
        args[i] = i;
        ThreadPool_add_work(reducerPool, (thread_func_t) Reducer_Entry, &args[i]);
    }
    
    delete[] args;
    ThreadPool_destroy(reducerPool);
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
    std::size_t partition = MR_Partition(key, shared_data->size());
    shared_data->insert(partition, key, value);
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
    while(!shared_data->finished(partition_number)) {
        char *key = shared_data->nextKey(partition_number);
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
    return shared_data->next(partition_number, key);
}