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

Follow the steps below to compile threadpool static library, the mapreduce static library, the distributed wordcount executable and the tests.

<!-- TODO -->

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) for details.

## ThreadPool

The ThreadPool allows a user to create a fixed number of worker threads and distribute work among them.

```ThreadPool_create``` creates a new ThreadPool object and starts the specified number worker threads. The worker threads enter an idle state until work is added to the threadpool. See Worker Lifecycle below for more information.

```ThreadPool_destroy``` must be called once the ThreadPool is no longer needed. This function waits for all work to processed before joining the working and freeing any memory allocated by ```ThreadPool_create```.

```ThreadPool_add_work``` adds work to the ThreadPool. If any worker threads are currently idling, this function will wake up a single thread to process the work added to the ThreadPool.

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
    while running {
        aquire lock
        while (there is work to do) {
            get work
            release lock
            run work
            aquire lock
        }
        if (threadpool is still running) {
            wait for more work
        }
        else {
            stop thread
        }
        release lock
    }
```

This algorithm ensures that the mutex is always aquired before accessing the work queue and that the worker thread idles whenever there is nothing to be done. The algorithm also guarantees that the lock will only have to be accessed once per each job completed.

## MapReduce

```MR_Run``` takes an array of files, a mapper function, a reducer function along with some other arguments and executes the MapReduce workflow. The input files are mapped in parallel to a set a key-value pairs by the mapper threads. These key-value pairs are then reduced in parallel by the reducer threads.

```MR_Emit``` is called by the user-defined mapper function to emits a key-value pair to the intermediate data structure. The key is mapped to a partition using the DJB2 hash function. 

**Time Complexity:** ```MR_Emit``` must ensure that the data structure stays sorted and takes O(log(n)) time in both the average and worst cases. See Intermediate Data Structure for further information about the programs internals.

```MR_Partition``` is an internal function used by MapReduce to compute which partition each key belongs in. It takes the key and returns the index of the partition for that key. Uses the DJB2 hash function provided with the assignment specification.

```MR_ProcessPartition``` is an internal function that is called by each reducer thread. It handles calling the user-defined reducer function to reduce each partition. The function loops until it has called reduce on all keys in the partition.

```MR_GetNext``` is called by the user-defined reducer threads to get the next value for a given key. The function returns NULL if there are no more values for the given key. 

**Time Complexity:** ```MR_GetNext``` returns the next key in O(1) time for the average case. C++ iterators guarantee to iteration over the entire dataset in O(n) time. See Intermediate Data Structure for further information about the programs internals.

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
