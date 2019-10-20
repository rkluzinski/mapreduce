#include "threadpool.h"

/**
 * A C style constructor for a ThreadPool_work object
 * Parameters:
 *      func - A function pointer to be executed
 *      arg - The arguments to the function pointer
 * Return:
 *      ThreadPool_work_t* - The pointer the new ThreadPool_work object
 *      NULL - If failed to allocate memory
 */
ThreadPool_work_t *ThreadPool_work_create(thread_func_t func, void *arg) {
    typedef ThreadPool_work_t work_t;
    
    work_t *work = malloc(sizeof(work_t));
    if (work == NULL) {
        return NULL;
    }

    work->func = func;
    work->arg = arg;
    work->next = NULL;

    return work;
}

/**
 * A C style destructor for a ThreadPool_work object
 * Parameters:
 *      work - The pointer the ThreadPool_work bbject
 */
void ThreadPool_work_destroy(ThreadPool_work_t *work) {
    free(work);
}

/**
* A C style constructor for creating a new ThreadPool_work_queue object
* Return:
*       ThreadPool_work_queue_t* - The pointer to the new ThreadPool_work_queue
*/
ThreadPool_work_queue_t *ThreadPool_work_queue_create() {
    typedef ThreadPool_work_queue_t work_queue_t;
    
    // allocate a new ThreadPool_work_queue object
    work_queue_t *work_queue = malloc(sizeof(work_queue_t));
    if (work_queue == NULL) {
        perror("ThreadPool_work_queue_create: malloc");
        exit(1);
    }

    // TODO initialize threadpool work queue
    work_queue->head = NULL;
    work_queue->tail = NULL;

    return work_queue;
}

/**
* A C style destructor to destroy a ThreadPool_work_queue
* Parameters:
*       work_queue - The pointer to the ThreadPool_work_queue to be destroyed
*/
void ThreadPool_work_queue_destroy(ThreadPool_work_queue_t *work_queue) {
    // free any remaining nodes before deleting
    ThreadPool_work_t *current = work_queue->head, *next;
    while (current != NULL) {
        next = current->next;
        ThreadPool_work_destroy(current);
        current = next;
    }

    free(work_queue);
}

/**
 * Push a new ThreadPool_work object to the back of a ThreadPool_work_queue
 * Parameters:
 *      work_queue - The pointer to a ThreadPool_work_queue_t
 *      work - The pointer the ThreadPool_work_t to be pushed
 */
void ThreadPool_work_queue_push(ThreadPool_work_queue_t *work_queue, ThreadPool_work_t *work) {
    // TODO error handling

    // if pushing the first item
    if (work_queue->head == NULL) {
        work_queue->head = work;
    }

    // if not pushing the first item
    if (work_queue->tail != NULL) {
        work_queue->tail->next = work;
    }

    work_queue->tail = work;
}

/**
 * Pop a ThreadPool_work object from the front of a ThreadPool_work_queue
 * Parameters:
 *      work_queue - The pointer to a ThreadPool_work_queue_t
 * Return:
 *      ThreadPool_work_t* - The pointer to the ThreadPool_work object
 */
ThreadPool_work_t *ThreadPool_work_queue_pop(ThreadPool_work_queue_t *work_queue) {
    ThreadPool_work_t *work = work_queue->head;

    // if popping last item in queue
    if (work_queue->head == work_queue->tail) {
        work_queue->head = NULL;
        work_queue->tail = NULL;
    }
    else {
        work_queue->head = work->next;
    }

    return work;
}

/**
 * Check if a ThreadPool_work_queue object is empty
 * Parameters:
 *      work_queue - The ThreadPool_work_queue to check
 * Returns:
 *      true - If the ThreadPool_work_queue is empty
 *      false - Otherwise
 */
bool ThreadPool_work_queue_empty(ThreadPool_work_queue_t *work_queue) {
    // TODO make thread safe

    return (work_queue->head == NULL);
}

/**
* A C style constructor for creating a new ThreadPool object
* Parameters:
*       num - The number of threads to create
* Return:
*       ThreadPool_t* - The pointer to the newly created ThreadPool object
*/
ThreadPool_t *ThreadPool_create(int num) {
    ThreadPool_t *threadpool = malloc(sizeof(ThreadPool_t));
    if (threadpool == NULL) {
        perror("ThreadPool_create: malloc");
        exit(1);
    }

    threadpool->work_queue = ThreadPool_work_queue_create();
    pthread_mutex_init(&threadpool->mutex, NULL);
    pthread_cond_init(&threadpool->not_empty, NULL);

    threadpool->running = true;
    threadpool->num_workers = num;
    
    threadpool->workers = (pthread_t *) malloc(sizeof(pthread_t) * num);
    if (threadpool->workers == NULL) {
        perror("ThreadPool_create: malloc");
        exit(1);
    }
    
    // create the workers for threadpool
    for (int i = 0; i < threadpool->num_workers; i++) {
        // cast Thread_run to avoid warnings
        typedef void *(*thread_entry)(void *);
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, (thread_entry) Thread_run, threadpool);
        threadpool->workers[i] = thread_id;
    }

    return threadpool;
}

/**
* A C style destructor to destroy a ThreadPool object
* Parameters:
*       tp - The pointer to the ThreadPool object to be destroyed
*/
void ThreadPool_destroy(ThreadPool_t *threadpool) {
    pthread_mutex_lock(&threadpool->mutex);
    threadpool->running = false;
    pthread_cond_broadcast(&threadpool->not_empty);
    pthread_mutex_unlock(&threadpool->mutex);

    // join the workers in the threadpool
    for (int i = 0; i < threadpool->num_workers; i++) {
        pthread_t thread_id = threadpool->workers[i];
        pthread_join(thread_id, NULL);
    }

    ThreadPool_work_queue_destroy(threadpool->work_queue);
    pthread_mutex_destroy(&threadpool->mutex);
    pthread_cond_destroy(&threadpool->not_empty);

    free(threadpool->workers);
    free(threadpool);
}

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
bool ThreadPool_add_work(ThreadPool_t *threadpool, thread_func_t func, void *arg) {
    ThreadPool_work_t *work = ThreadPool_work_create(func, arg);
    if (work == NULL) {
        return false;
    }

    pthread_mutex_lock(&threadpool->mutex);
    ThreadPool_work_queue_push(threadpool->work_queue, work);
    // awaken one idle thread (if any are idle)
    pthread_cond_signal(&threadpool->not_empty);
    pthread_mutex_unlock(&threadpool->mutex);

    return true;
}

/**
* Get a task from the given ThreadPool object
* Parameters:
*       tp - The ThreadPool object being passed
* Return:
*       ThreadPool_work_t* - The next task to run
*       NULL - If work queue is empty
*/
ThreadPool_work_t *ThreadPool_get_work(ThreadPool_t *threadpool) {
    ThreadPool_work_t *work = NULL;

    if (!ThreadPool_work_queue_empty(threadpool->work_queue)) {
        work = ThreadPool_work_queue_pop(threadpool->work_queue);
    }
    
    return work;
}

/**
* Run the next task from the task queue
* Parameters:
*       tp - The ThreadPool Object this thread belongs to
* Returns:
*       NULL
*/
void *Thread_run(ThreadPool_t *threadpool) {
    bool running = true;
    ThreadPool_work_t *work = NULL;
    
    // while the thread is running
    while (running) {
        pthread_mutex_lock(&threadpool->mutex);
        work = ThreadPool_get_work(threadpool);

        // execute work until the work queue is empty
        while (work != NULL) {
            pthread_mutex_unlock(&threadpool->mutex);

            // release lock and execute work
            work->func(work->arg);
            ThreadPool_work_destroy(work);

            // reaquire lock and get next work
            pthread_mutex_lock(&threadpool->mutex);
            work = ThreadPool_get_work(threadpool);
        }

        // if threadpool is still running    
        if (threadpool->running)
            // wait for more work
            pthread_cond_wait(&threadpool->not_empty, &threadpool->mutex);
        else
            // stop thread
            running = false;

        // release lock
        pthread_mutex_unlock(&threadpool->mutex);
    }

    return NULL;
}