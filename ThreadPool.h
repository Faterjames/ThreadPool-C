//
// Created by sunruoxu on 17-4-2.
//

#ifndef CHATSERVER_THREADPOOL_H
#define CHATSERVER_THREADPOOL_H

#include <sys/types.h>
#include <stdbool.h>



struct bsem{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int vaule;
};
struct thread{
    pthread_t pthread;
    int id;
    struct ThreadPool* _threadPool;
};


struct ThreadQueue_item{
    struct work* work;
    struct ThreadQueue_item* next;
};

struct ThreadQueue{
    pthread_mutex_t rwmutext;
    struct bsem has_work;
    int size;
    struct ThreadQueue_item* pos;
    struct ThreadQueue_item* head;
};
struct ThreadPool{
    struct thread** threads;
    struct ThreadQueue threadQueue;
    int thread_alive_num;
    int thread_running_num;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}ThreadPool;

struct work{
    void (*func)(void*);
    void* arg;
};


struct ThreadPool* ThreadPool_init(int num);
void ThreadPool_work_in(struct ThreadPool* threadPool,void* func,void* arg);

void ThreadPool_wait(struct ThreadPool* threadPool);



#endif //CHATSERVER_THREADPOOL_H
