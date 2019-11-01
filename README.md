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
* CMake

### Building

Follow the steps below to compile threadpool static library, the mapreduce static library, the distributed wordcount executable and the tests.

```
mkdir bin
cd bin
cmake ..
make
```

### Running Tests

The tests are compiled along with the executable and can be found in the build directory. The following tests are built:

```
./test_queue
./test_threadpool
```

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) for details.

## ThreadPool

The ThreadPool allows a user to create a fixed number of worker threads and distribute work among them. The example below shows how to use ThreadPool library.

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
Creates a new ThreadPool object and starts ```num_threads``` worker threads. Returns NULL if unable to allocate memory.

```C
void ThreadPool_destroy(ThreadPool_t *threadpool)
``` 
Waits for all work to processed before joining all worker threads and destroying the ThreadPool object.

```C
bool ThreadPool_add_work(ThreadPool_t *threadpool, thread_func_t work, void *arg)
```
Adds a function to be executed and arguments to pass to the work queue.

### Removed/Modified Functions

```C
ThreadPool_work_t *ThreadPool_get_work(ThreadPool_t *threadpool)
``` 
Replaced by the work queue implementation. The work queue's interface makes this function unnecessary.

```C
void *Thread_run(ThreadPool_t *threadpool)
``` 
Replaced by ```Thread_entry```, which handles the entire lifecycle of each worker thread. For more information see Worker Lifecycle.

### Work Queue

The work queue is implemented as singly-linked list supporting push and pop operations. Data is accessed first-in first-out to ensure that work is executed in the order it is given. Push and pop operations take O(1) time. The queue is not thread-safe and relies on an external mutex concurrent access.

### Worker Lifecycle

The pseudocode below describes the behaviour of the worker threads. Each ThreadPool has a mutex that must be aquired before the work queue can be modifed.

```
Thread_entry:
    while thread running {
        aquire mutex
        while (there is work to do) {
            get work
            release mutex
            run work
            aquire mutex
        }
        if (threadpool is running) {
            wait for more work
        }
        else {
            stop thread
        }
        release mutex
    }
    exit thread
```

Worker threads pull from the work queue until there is no more work. If there work queue is stopped they exit, otherwise they sleep until there is more work available. This algorithms ensures the lock only has to be aquired once per work executed.

## MapReduce

The MapReduce libary allows a user to write multithreaded program by defining a Map function and a Reduce function. The example below (provided by the instructor) shows how to implement a distributed wordcount program with MapReduce.

```C
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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

To use the library, the user must define their program as a mapper function and reducer function with the specified signatures:

```C
void Map(char *filename);
void Reduce(char *key, int partition_number);
```

The map function must call ```MR_Emit``` and the reducer function must call ```MR_GetNext```.

```C
void MR_Run(int num_files, char *filenames[], Mapper map, int num_mappers, Reducer concate, int num_reducers)
``` 
This function runs the MapReduce process. The input files are mapped to an intermediate data structure using the user defined Map function and then reduced by the user defined Reduce function. 

<!-- TODO: Throws an exception if any ThreadPool operations fail. -->

```C
void MR_Emit(char *key, char *value)
``` 
Called by the user-defined mapper function to emits a key-value pair to the intermediate data structure. Takes O(log(n)) time where n is number of key-value pairs currently in the parition.

```C
unsigned long MR_Partition(char *key, int num_partitions);
``` 
Internal function used to compute which partition each key belongs to. Uses the DJB2 hash function implementation provided on eClass to map a key to a partition.

```C
void MR_ProcessPartition(int partition_number);
```
Internal function that calls the user-defined reducer function until all keys have been reduced.

```C
char *MR_GetNext(char *key, int partition_number);
``` 
Called by the user-defined reducer threads to get the next value for that key. Takes O(1) time (on average) to return the next key. Returns NULL if there are no more values available. 

### Global Variables

The reducer function and intermediate data are kept in global variables so that they can be accessed without being passed as an argument. These global variables should not be modified directly by the user program.

### Intermediate Data Structure

The MapReduce library uses multimaps for each partitions of the intermediate data structure, to efficiently store the key-value pairs emitted by the user-defined mapper function.

**Thread Safety:** Access to each partition is controlled by its own mutex. This allows for two partitions to be modifed concurrently. The reducing phase does not use these mutexes, as each thread processes different data, and shared data is not modified. 

**Efficiency:** The key-value pairs are stored in a C++ STL multimap. Multimap is an ordered data structure that allows for multiple values to be stored using the same key. It guarantees an insertion time of O(log(n)), giving the time complexity of ```MR_Emit```. Iterating over the multimap is guaranteed to take O(n) time, which implies the O(1) run time of ```MR_GetNext```.

## Testing

To ensure that the program was fault free and met the assignments reequirements, extensive testing was performed.

### Unit Testing

*The files for the unit testing were left out from the submission because they were not required for assignment and I did not want to include any extraneous files.*

The work queue was testing to ensure that pushing and popping from the queue worked properly and did not cause any segmentation faults.

The ThreadPool was tested under number of conditions to ensure it was stable. These tests covered different numbers of workers, large amounts of work, and ensured that the ThreadPool did not deadlock or segfault.

### Integration Testing

The MapReduce library was tested using the integration tests provided by Jihoon Og on eClass. These tests used the MapReduce library to perform a distributed wordcount on a large number of files (approximately 5 MB). The integration tests confirmed that MapReduce and ThreadPool behaved correctly. It also provided a way to benchmark the program.

## Valgrind

Valgrind's memcheck tool was used alongside the unit tests and integration tests to confirm that each of the programs individual components did leak memory.

Helgrind was also used to check for any undiscovered data races in the MapReduce library.

## Acknowledgements
