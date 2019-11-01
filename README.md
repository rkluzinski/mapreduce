# MapReduce

A threadpool and map-reduce library written in C

** This project is not publicly accessable on GitHub.**

## Author

* **Ryan Kluzinski** - *Initial Work*
    * CCID: rkluzins
    * Student ID: 1492614

## Getting Started

### Prequisites

* Linux-based Operating System
* C++11 and C99 compatible Compiler

### Building

To compile the object files and link the wordcount executable run ```make``` or ```make wc```.

To only compile the object file run ```make compile```.

To remove all object files and the final executable run: ```make clean```.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) for details.

## ThreadPool

The ThreadPool allows a user to create a fixed number of worker threads and distribute work among them. The example below shows how the ThreadPool library can be used.

```C
void some_work(void *arg) {
    // do some work here
}

void run_threadpool() {
    // create a threadpool with 10 worker threads
    ThreadPool_t *threadpool = ThreadPool_create(10);

    // add 100 jobs to the ThreadPool
    for (int i = 0; i < 100; i++) {
        ThreadPool_add_work(threadpool, some_work, NULL);
    }

    // wait for the threadpool to finish and free memory
    ThreadPool_destroy(threadpool);
}

```

```C
ThreadPool_t *ThreadPool_create(int num_threads)
``` 
Creates a new ThreadPool object and starts ```num_threads``` worker threads. The worker threads enter an idle state until work is added to the threadpool. See Worker Lifecycle below for more information.

```C
void ThreadPool_destroy(ThreadPool_t *threadpool)
``` 
Waits for all work to processed before joining all worker threads and destroying the ThreadPool object.

```C
bool ThreadPool_add_work(ThreadPool_t *threadpool, thread_func_t work, void *arg)
```
Adds the function pointer ```work``` to be run with argument ```arg``` to the ThreadPool's work queue. If any worker threads are idling, one of them will be woken up to execute this work.

### Removed/Modified Functions

```ThreadPool_get_work``` was removed since it was replaced by functionality implemented by the work queue. The work queue offers the ability to check if the work queue is empty, and pop the first work object. See Work Queue below for more information.

```Thread_run``` has been removed and replaced with ```Thread_entry``` is not the entry point for each worker thread. It takes a void pointer as an arguments to avoid needing to cast when calling ```pthread_create```. See Worker Lifecycle below for more information.

### Work Queue

The work queue is implemented as singly-linked list. The singly-linked list supports the push and pop operations which push to the back of the queue and pops from the front. This first-in first-out ordering ensures that work is executed by threadpool in the order it is given. The operations implemented also allow for work to be added and removed in O(1) time. The queue does not lock when operations are being executed and instead relies on the worker threads to aquire the lock before modifying the work queue.

Each node in the work queue stores a pointer to the function to execute, the arguments to pass to that function and a pointer to the next node in the work queue.

### Worker Lifecycle

This section discusses the lifecycle of each worker thread, and how they manage concurrent access to the work queue efficiently. The pseudocode below describes the behaviour of the worker threads.

```
Thread_entry:
    while thread running {
        aquire lock
        while (there is work to do) {
            get work
            release lock
            run work
            aquire lock
        }
        if (threadpool is running) {
            wait for more work
        }
        else {
            stop thread
        }
        release lock
    }
    exit thread
```

This algorithm ensures that the mutex is always aquired before accessing the work queue and that the worker thread idles whenever there is nothing to be done. The algorithm also guarantees that the lock will only have to be accessed once per each job completed.

## MapReduce

The MapReduce libary allows the user easily parallelize any program that can be broken down into a mapping and reducing phase. The example below (provided by the instructor) shows how to uses the MapReduce paradigm to implement a distributed wordcount (some include statements are left out).

```C
#include "mapreduce.h"

void Map(char *file_name) {
    FILE *fp = fopen(file_name, "r");
    assert(fp != NULL);
    char *line = NULL;
    size_t size = 0;
    while (getline(&line, &size, fp) != -1) {
        char *token, *dummy = line;
        while ((token = strsep(&dummy, " \t\n\r")) != NULL)
            MR_Emit(token, "1");
    }
    free(line);
    fclose(fp);
}

void Reduce(char *key, int partition_number) {
    int count = 0;
    char *value, name[100];
    while ((value = MR_GetNext(key, partition_number)) != NULL)
        count++;
    sprintf(name, "result-%d.txt", partition_number);
    FILE *fp = fopen(name, "a");
    printf("%s: %d\n", key, count);
    fprintf(fp, "%s: %d\n", key, count);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    MR_Run(argc - 1, &(argv[1]), Map, 10, Reduce, 10);
    return 0;
}
```

To use the library, the user must define their program as a mapper function and reducer function. These functions mut have the following signatures:

```C
void Map(char *filename);
void Reduce(char *key, int partition_number);
```

The map function must emit key-value pairs by calling ```MR_Emit``` and the reducer function must retrieve key-value pairs by calling ```MR_GetNext```.

```C
void MR_Run(int num_files, char *filenames[], Mapper map, int num_mappers, Reducer concate, int num_reducers)
``` 
Takes an array of files, a mapper function, a reducer function along with some other arguments and executes the MapReduce workflow. The input files are mapped in parallel to a set a key-value pairs by the mapper threads. These key-value pairs are then reduced in parallel by the reducer threads.

```C
void MR_Emit(char *key, char *value)
``` 
Called by the user-defined mapper function to emits a key-value pair to the intermediate data structure. The key is mapped to a partition using the DJB2 hash function. 

```MR_Emit``` must ensure that the data structure stays sorted and takes O(log(n)) time where n is number of key-value pairs in the parition. See Intermediate Data Structure for further information about the programs internals.

```C
unsigned long MR_Partition(char *key, int num_partitions);
``` 
Is an internal function used by MapReduce to compute which partition each key belongs in. It takes the key and returns the index of the partition for that key. Uses the DJB2 hash function provided with the assignment specification.

```C
void MR_ProcessPartition(int partition_number);
```
Is an internal function that is called by each reducer thread. It handles calling the user-defined reducer function to reduce each partition. The function loops until it has called reduce on all keys in the partition.

```C
char *MR_GetNext(char *key, int partition_number);
``` 
Is called by the user-defined reducer threads to get the next value for a given key. The function returns NULL if there are no more values for the given key. 

```MR_GetNext``` returns the next key in O(1) time (on average). This is guaranteed because C++ iterators guarantee to iteration over the entire dataset in O(n) time where n in the number of key-value pairs in the partition. See Intermediate Data Structure for further information about the programs internals.

### Global Variables

Since some of the functions specified by the MapReduce header are not passed all of the data they need by argument, some of the data must be stored as a global variable. 

MapReduce stores the reducer function as a global variable so that it can be accessed by ```MR_ProcessPartition```, which only takes the partition number as an argument. 

The intermediate data structure needs to be accessed by ```MR_Emit``` and ```MR_GetNext``` and is stored as a global variable.

### Intermediate Data Structure

The MapReduce library relies on the ```MRData``` intermediate data structure to store the key-value pairs emitted by the user-defined mapper threads in a thread-safe and efficient manner. Underlying ```MRData``` is an array of mutexes, multimaps and iterators. The arrays store one of each data type for every partition in the data structure.

**Thread Safety:** Mutexes are used to lock each partition when ```MR_Emit``` is inserting a new key-value pair into a partition. This guarantees that each partition can only be modified by one thread at any given time, while still allowing two threads to modify different partitions concurrently. 

The reducing phase does not need any concurrency control as each of the threads are processing different data, and no shared data is being modified. 

**Data Structures:** The key-value pairs are stored in a C++ STL multimap. Multimap allows for multiple values to be stored under the same key while maintaining the order of the data structure. The underlying data structure for multimap is a self-balancing binary tree, guaranteeing O(log(n)) insertion time for new keys.

**Iterators:** ```std::multimap::const_iterator``` is used to iterate over each of the partitions. They guarantee iteration over the entire dataset in O(n) time, meangin each element can be accessed in order in O(1) time, on average.

## Testing

To ensure that the program was fault free and met the assignments reequirements, extensive testing was performed.

### Unit Testing

The ThreadPool library is the backbone of the MapReduce library and therefore it was important to ensure that it was implemented correctly before proceeding with the remainder of the assignment.

Since the ThreadPool library was implemented in C, the data structure for the work queue was implemented alongside the ThreadPool. Test cases were developed to ensure that pushing and popping from the queue conformed to first-in first-out ordering and did not cause and segmentation faults.

Once the work queue data structure was verfied by the tests, the ThreadPool library was verified next. It tested a variety of conditions like processing over 1 million jobs and having hundreds of workers sharing a slect number of jobs. These tests verified that the ThreadPool was concurrency-safe and did not deadlock.

The files for the unit testing were left out from the submission because they were not required for assignment and I did not want to include any extraneous files.

### Integration Testing

The MapReduce library was difficult design unit tests for and was instead tested using the integration tests provided by Jihoon Og on the eClass discussion forums. These tests used the MapReduce library to perform a distributed wordcount on a large number of files (approximately 5 MB). The integration tests confirmed that the MapReduce library was performing correcty and the all of the components of the program were integrated properly. It also provided a benchmark for how the efficiency of MapReduce.

## Valgrind

Valgrind's memcheck tool was used with the various unit tests and integration tests to confirm that each of the programs individual components did not contain any memory leaks.

The helgrind tool was also used to check for any undiscovered data races in the MapReduce library.

## Acknowledgements
