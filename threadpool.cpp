#include "threadpool.h"

// for debugging
#include <iostream>
#include <unistd.h>

ThreadPoolWork::ThreadPoolWork(thread_func_t func, void *arg): func(func), arg(arg) {

}

ThreadPoolWork::~ThreadPoolWork() {

}

void ThreadPoolWork::execute() {
    func(arg);
}

void *ThreadPool::threadStart(void *arg) {
    ThreadPool *threadpool = (ThreadPool *) arg;
    
    while (true) {
        pthread_mutex_lock(&threadpool->queue_mutex);
        pthread_cond_wait(&threadpool->queue_cond, &threadpool->queue_mutex);

        pthread_mutex_unlock(&threadpool->queue_mutex);

        ThreadPoolWork *work = threadpool->getWork();

        if (work != NULL) {
            work->execute();
            delete work;
        }

        return NULL;
    }
}

ThreadPool::ThreadPool(int num_threads) {
    work_queue = std::deque<ThreadPoolWork *>();

    for (int i = 0; i < num_threads; i++) {
        pthread_t thread;
        pthread_create(&thread, NULL, threadStart, this);
        threads.push_back(thread);
    }
}

ThreadPool::~ThreadPool() {
    for (auto thread : threads) {
        void **return_val = NULL;
        pthread_join(thread, return_val);
    }
}

bool ThreadPool::addWork(thread_func_t func, void *arg) {
    pthread_mutex_lock(&queue_mutex);
    
    ThreadPoolWork *work = new ThreadPoolWork(func, arg);
    work_queue.push_back(work);
    
    pthread_cond_signal(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);

    // when does this return false?
    return true;
};

ThreadPoolWork *ThreadPool::getWork() {
    pthread_mutex_lock(&queue_mutex);
    
    if (work_queue.empty()) {
        return NULL;
    }
    ThreadPoolWork *work = work_queue.front();
    work_queue.pop_front();
    
    pthread_mutex_unlock(&queue_mutex);
    
    return work;
}

void *ThreadPool::run() {
    // run the next task
    return NULL;
}