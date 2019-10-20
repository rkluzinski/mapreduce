#include <stdio.h>
#include <assert.h>

#include "../src/threadpool.h"
 
// ThreadPool_work function prototypes
ThreadPool_work_t *ThreadPool_work_create(thread_func_t func, void *arg);
void ThreadPool_work_destroy(ThreadPool_work_t *work);

// ThreadPool_work_queue function prototypes
ThreadPool_work_queue_t *ThreadPool_work_queue_create();
void ThreadPool_work_queue_destroy(ThreadPool_work_queue_t *work_queue);
void ThreadPool_work_queue_push(ThreadPool_work_queue_t *work_queue, ThreadPool_work_t *work);
ThreadPool_work_t *ThreadPool_work_queue_pop(ThreadPool_work_queue_t *work_queue);
bool ThreadPool_work_queue_empty(ThreadPool_work_queue_t *work_queue);

// testing data
const char *message[5] = {
    "ThreadPool_work 1",
    "ThreadPool_work 2",
    "ThreadPool_work 3",
    "ThreadPool_work 4",
    "ThreadPool_work 5",
};

void test_normal_use() {
    ThreadPool_work_queue_t *work_queue = ThreadPool_work_queue_create();
    assert(ThreadPool_work_queue_empty(work_queue) == 1);

    for (int i = 0; i < 5; i++) {
        ThreadPool_work_t *work = ThreadPool_work_create(NULL, (void *) message[i]);
        ThreadPool_work_queue_push(work_queue, work);
    }

    // assert queue not empty
    assert(ThreadPool_work_queue_empty(work_queue) == 0);

    for (int i = 0; i < 5; i++) {
        ThreadPool_work_t *work = ThreadPool_work_queue_pop(work_queue);
        // assert first-in first-out
        assert(work->arg == message[i]);
        ThreadPool_work_destroy(work);
    }

    // assert empty
    assert(ThreadPool_work_queue_empty(work_queue) == 1);
    ThreadPool_work_queue_destroy(work_queue);
}

void test_delete_full() {
    ThreadPool_work_queue_t *work_queue = ThreadPool_work_queue_create();

    for (int i = 0; i < 5; i++) {
        ThreadPool_work_t *work = ThreadPool_work_create(NULL, (void *) message[i]);
        ThreadPool_work_queue_push(work_queue, work);
    }

    // assert queue not empty
    assert(ThreadPool_work_queue_empty(work_queue) == 0);
    ThreadPool_work_queue_destroy(work_queue);
}

int main(int argc, char *argv[]) {
    fputs("Testing ThreadPool_work_queue: ", stdout);
    
    test_normal_use();
    test_delete_full();

    fputs("Passed \n", stdout);
    return 0;
}