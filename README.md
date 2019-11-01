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
* C++11 Compiler
* C11 Compiler

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

## ThreadPool Documentation

The ThreadPool allows a user to create a fixed number of worker threads and distribute work among them.

```ThreadPool_create``` creates a new ThreadPool object and starts the specified number worker threads. The worker threads enter an idle state until work is added to the threadpool. See Worker Lifecycle below for more information.

```ThreadPool_destroy``` must be called once the ThreadPool is no longer needed. This function waits for all work to processed before joining the working and freeing any memory allocated by ```ThreadPool_create```.

```ThreadPool_add_work``` adds work to the ThreadPool. If any worker threads are currently idling, this function will wake up a single thread to process the work added to the ThreadPool.

### Removed Functions

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

## MapReduce Documentation

### Intermediate Data Structure 

### MapReduce Library

* Explain data structure
* Analyze time complexity of MR_Emit and MR_GetNext

### Testing

## Acknowledgements
