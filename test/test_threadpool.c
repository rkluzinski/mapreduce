#include <stdio.h>
#include <assert.h>

#include "../src/threadpool.h"

pthread_mutex_t mutex;
int tasks_completed = 0;

void mock_work(void *args) {
    pthread_mutex_lock(&mutex);
    tasks_completed += 1;
    pthread_mutex_unlock(&mutex);
}

void test_threadpool(int num_workers, int num_tasks) {
    tasks_completed = 0;
    ThreadPool_t *threadpool = ThreadPool_create(num_workers);

    for (int i = 0; i < num_tasks; i++) {
        ThreadPool_add_work(threadpool, mock_work, NULL);
    }

    ThreadPool_destroy(threadpool);
    assert(tasks_completed == num_tasks);
}

int main(int argc, char *argv[]) {
    fputs("Testing ThreadPool: ", stdout);
    pthread_mutex_init(&mutex, NULL);

    test_threadpool(8, 256);
    test_threadpool(8, 1024 * 1024);
    test_threadpool(256, 64);

    pthread_mutex_destroy(&mutex);
    fputs("Passed \n", stdout);
    return 0;
}