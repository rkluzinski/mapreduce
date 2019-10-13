#include <stdio.h>
#include <assert.h>

#include "../threadpool.h"

// ThreadPool_work function prototypes
ThreadPool_work_t *ThreadPool_work_create(thread_func_t func, void *arg);
void ThreadPool_work_destroy(ThreadPool_work_t *work);

// ThreadPool_work_queue function prototypes
ThreadPool_work_queue_t *ThreadPool_work_queue_create();
void ThreadPool_work_queue_destroy(ThreadPool_work_queue_t *work_queue);
void ThreadPool_work_queue_push(ThreadPool_work_queue_t *work_queue, ThreadPool_work_t *work);
ThreadPool_work_t *ThreadPool_work_queue_pop(ThreadPool_work_queue_t *work_queue);
bool ThreadPool_work_queue_empty(ThreadPool_work_queue_t *work_queue);

int main(int argc, char *argv[]) {
    puts("ThreadPool_work_queue test");
    puts("creating work queue");
    ThreadPool_work_queue_t *work_queue = ThreadPool_work_queue_create();

    // check if empty, should be 1
    printf("Queue empty? %d\n", ThreadPool_work_queue_empty(work_queue));

    const char *message[5] = {
        "ThreadPool_work 1",
        "ThreadPool_work 2",
        "ThreadPool_work 3",
        "ThreadPool_work 4",
        "ThreadPool_work 5",
    };

    // push to queue
    for (int i = 0; i < 5; i++) {
        printf("push %s to the queue\n", message[i]);
        ThreadPool_work_t *work = ThreadPool_work_create(NULL, (void *) message[i]);
        ThreadPool_work_queue_push(work_queue, work);
    }

    // check if empty, should be 0
    printf("Queue empty? %d\n", ThreadPool_work_queue_empty(work_queue));

    // pop from queue
    for (int i = 0; i < 5; i++) {
        ThreadPool_work_t *work = ThreadPool_work_queue_pop(work_queue);
        printf("pop %s from the queue\n", (const char *) work->arg);
        ThreadPool_work_destroy(work);
    }

    // check if empty, should be 1
    printf("Queue empty? %d\n", ThreadPool_work_queue_empty(work_queue));

    // destroy empty queue
    puts("destroying work queue");
    ThreadPool_work_queue_destroy(work_queue);

    puts("creating queue");
    work_queue = ThreadPool_work_queue_create();
    
    // check if empty, should be 1
    printf("Queue empty? %d\n", ThreadPool_work_queue_empty(work_queue));

    // push to queue
    for (int i = 0; i < 5; i++) {
        printf("push %s to the queue\n", message[i]);
        ThreadPool_work_t *work = ThreadPool_work_create(NULL, (void *) message[i]);
        ThreadPool_work_queue_push(work_queue, work);
    }

    // check if empty, should be 0
    printf("Queue empty? %d\n", ThreadPool_work_queue_empty(work_queue));

    // destroy full queue
    puts("destroying the queue");
    ThreadPool_work_queue_destroy(work_queue);

    return 0;
}