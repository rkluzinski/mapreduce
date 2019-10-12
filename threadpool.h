#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <deque>
#include <vector>

#include <pthread.h>

// TODO change typedef in C++ equivalent
typedef void (*thread_func_t)(void *arg);

// TODO make into private inner class for ThreadPool
class ThreadPoolWork {
    thread_func_t func;
    void *arg;

public:
    ThreadPoolWork(thread_func_t func, void *arg);
    ~ThreadPoolWork();

    void execute();
};

class ThreadPool {
    std::vector<pthread_t> threads;

    pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;
    std::deque<ThreadPoolWork *> work_queue;

    // entry point for each pthread
    static void *threadStart(void *arg);

public:
    /**
    * ThreadPool constructor
    * Parameters:
    *     num_threads - The number of threads to create
    */
   ThreadPool(int num_threads);

    /**
    * ThreadPool destructor
    */
   ~ThreadPool();

   /**
    * Add a task to the ThreadPool's task queue
    * Parameters:
    *     func - The function pointer that will be called in the thread
    *     arg  - The arguments for the function
    * Return:
    *     true  - If successful
    *     false - Otherwise
    */
    bool addWork(thread_func_t fn, void *arg);

    /**
    * Get a task from the given ThreadPool object
    * Return:
    *     ThreadPoolWork* - The next task to run
    */
    ThreadPoolWork *getWork();

    /**
    * Run the next task from the task queue
    * Return:
    *     The return value of the thread_func_t run
    */
    void *run();
};

/**
 * C wrapper for C++ ThreadPool implementation
 * TODO: remove if not needed
 */
// extern "C" {
//     typedef struct ThreadPool_work_t ThreadPoolWork;
//     typedef struct ThreadPool_t ThreadPool;

//     ThreadPool_t *ThreadPool_create(int num);
//     void ThreadPool_destroy(ThreadPool_t *tp);
//     bool ThreadPool_add_work(ThreadPool_t *tp, thread_func_t func, void *arg);
//     ThreadPool_work_t *ThreadPool_get_work(ThreadPool_t *tp);
//     void *Thread_run(ThreadPool_t *tp);
// }


#endif
