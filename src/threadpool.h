#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>    // for multithreading
#include <stdbool.h>    // for boolean types
#include <stdio.h>      // for perror, fprintf
#include <stdlib.h>     // for malloc, free

typedef void (*thread_func_t)(void *arg);

/**
 * ThreadPool_work_t stores a function pointer and arguments
 * Used as node for the ThreadPool_work_queue_t
 */
typedef struct ThreadPool_work_t {
    thread_func_t func;                 // function pointer
    void *arg;                          // arguments to func
    
    struct ThreadPool_work_t *next;     // next task in the queue
} ThreadPool_work_t;

/**
 * ThreadPool_work_queue_t is implemented as a singly linked list
 * Pushing to and popping from the list are not blocking operations
 */
typedef struct {
    ThreadPool_work_t *head;
    ThreadPool_work_t *tail;
} ThreadPool_work_queue_t;

typedef struct {
    bool running;

    // tracks threads that belong to this threadpool
    int num_workers;
    pthread_t *workers;
    
    // work queue, mutex and condition
    ThreadPool_work_queue_t *work_queue;
    pthread_mutex_t mutex;      // mutex for the work queue
    
    // TODO change to a more representative name
    pthread_cond_t not_empty;   // work queue is not empty
} ThreadPool_t;


/**
* A C style constructor for creating a new ThreadPool object
* Parameters:
*       num - The number of threads to create
* Return:
*       ThreadPool_t* - The pointer to the newly created ThreadPool object
*/
ThreadPool_t *ThreadPool_create(int num);

/**
* A C style destructor to destroy a ThreadPool object
* Parameters:
*       tp - The pointer to the ThreadPool object to be destroyed
*/
void ThreadPool_destroy(ThreadPool_t *tp);

/**
* Add a task to the ThreadPool's task queue
* Parameters:
*       tp   - The ThreadPool object to add the task to
*       func - The function pointer that will be called in the thread
*       arg  - The arguments for the function
* Return:
*       true  - If successful
*       false - Otherwise
*/
bool ThreadPool_add_work(ThreadPool_t *tp, thread_func_t func, void *arg);

/**
* Get a task from the given ThreadPool object
* Parameters:
*       tp - The ThreadPool object being passed
* Return:
*       ThreadPool_work_t* - The next task to run
*/
ThreadPool_work_t *ThreadPool_get_work(ThreadPool_t *tp);

/**
* Run the next task from the task queue
* Parameters:
*       tp - The ThreadPool Object this thread belongs to
*/
void *Thread_run(ThreadPool_t *tp);

#endif
