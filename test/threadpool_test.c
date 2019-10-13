#include <stdio.h>

#include "../threadpool.h"

// function prototype to destory work
void ThreadPool_work_destroy(ThreadPool_work_t *work);

int main(int argc, char *argv[]) {
    //MR_Run(argc - 1, &(argv[1]), Map, 10, Reduce, 10);

    // test will not work once threadpool is working
    // should segfault when a worker tries to dereference
    // a null pointer
    puts("ThreadPool test");
    puts("creating threadpool");
    ThreadPool_t *threadpool = ThreadPool_create(1);
    
    // add work
    for (int i = 0; i < 4; i++) {
        puts("adding work");
        ThreadPool_add_work(threadpool, NULL, NULL);
    }

    // get work, test if return null on empty
    for (int i = 0; i < 5; i++) {
        ThreadPool_work_t *work = ThreadPool_get_work(threadpool);
        if (work != NULL) {
            puts("got work");

            // destroy work
            ThreadPool_work_destroy(work);
        }
        else {
            // queue is empty
            puts("no more work");
        }
    }

    // destroy an empty thread pool
    puts("destroying work queue");
    ThreadPool_destroy(threadpool);

    // create a new threadpool
    puts("creating threadpool");
    ThreadPool_t *threadpool = ThreadPool_create(1);

    // add work
    for (int i = 0; i < 4; i++) {
        puts("adding work");
        ThreadPool_add_work(threadpool, NULL, NULL);
    }
    
    // destory full threadpool
    puts("destroying work queue");
    ThreadPool_destroy(threadpool);

    return 0;
}