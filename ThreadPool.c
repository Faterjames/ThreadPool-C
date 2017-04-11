//
// Created by sunruoxu on 17-4-5.
//
#include <stdio.h>
#include <stdbool.h>
#include "ThreadPool.h"
#include <pthread.h>
#include <malloc.h>

#define err(str) printf(str)

static volatile int threadlive = 1;



struct ThreadPool* ThreadPool_init(int num);

void ThreadPool_work_in(struct ThreadPool* threadPool,void* func,void* arg);

void ThreadPool_wait(struct ThreadPool* threadPool);

void* do_thread(struct thread* pthread);

int thread_init(struct ThreadPool* threadPool_,struct thread* pthread,int id);

void thread_destroy(struct thread* thread);

void ThreadQueue_init(struct ThreadQueue* threadQueue);

struct ThreadQueue_item* ThreadQueue_out(struct ThreadQueue* threadQueue);

void ThreadQueue_in(struct ThreadQueue* threadQueue,struct ThreadQueue_item* new_work);

void bsem_init(struct bsem* bsem,int vaule);

void bsem_reset(struct bsem* bsem);

void bsem_post(struct bsem* bsem);

void bsem_post_all(struct bsem* bsem);

void bsem_wait(struct bsem* bsem);
/////////////////////////////ThreadPool//////////////////////////////////////////////

struct ThreadPool* ThreadPool_init(int num){
    struct ThreadPool* threadPool = (struct ThreadPool*) malloc(sizeof(struct ThreadPool));
    if(threadPool == NULL){
        err("can not malloc space for threadpool");
        return NULL;
    }
    threadPool->thread_alive_num = num;
    threadPool->thread_running_num = 0;
    ThreadQueue_init(&threadPool->threadQueue);
    pthread_mutex_init(&threadPool->mutex,NULL);
    pthread_cond_init(&threadPool->cond,NULL);
    threadPool->threads = (struct thread**)malloc(num * sizeof(struct thread*));
    if(threadPool->threads == NULL){
        err("can not malloc space for threads");
        return NULL;
    }
    for(int i = 0;i < num;i ++){
        thread_init(threadPool,threadPool->threads[i],i);
    }
    return threadPool;
}

void ThreadPool_work_in(struct ThreadPool* threadPool,void* func,void* arg){
    struct ThreadQueue_item* item = (struct ThreadQueue_item*)malloc(sizeof(struct ThreadQueue_item));
    if(item == NULL){
        err("can not malloc space for your work");
        return;
    }
    struct work* new_work = (struct work*)malloc(sizeof(struct work));
    new_work->func = func;
    new_work->arg = arg;

    item->work = new_work;
    item->next = NULL;

    ThreadQueue_in(&threadPool->threadQueue,item);
}

void ThreadPool_wait(struct ThreadPool* threadPool){
    pthread_mutex_lock(&threadPool->mutex);
    while(threadPool->thread_running_num || threadPool->threadQueue.size){
        pthread_cond_wait(&threadPool->cond,&threadPool->mutex);
    }
    pthread_mutex_unlock(&threadPool->mutex);
}

int ThreadPool_running_num(struct ThreadPool * threadPool){
    return threadPool->thread_running_num;
}

int ThreadPool_alive_num(struct ThreadPool* threadPool){
    return threadPool->thread_alive_num;
}

void ThreadPool_destroy(struct ThreadPool* threadPool){
    pthread_mutex_lock(&threadPool->mutex);
    for (int i = 0; i < threadPool->thread_alive_num;i ++) {
        thread_destroy(threadPool->threads[i]);
    }
    free(threadPool->threadQueue.head);
    free(threadPool->threadQueue.pos);
    free(&threadPool->threadQueue);
    free(threadPool);
}

/////////////////////////////Thread//////////////////////////////////////////////////////
int thread_init(struct ThreadPool* threadPool_,struct thread* pthread,int id){
    pthread = (struct thread*) malloc(sizeof(struct thread));
    if(pthread == NULL){
        err("thread init can not malloc space");
        return 1;
    }
    pthread ->_threadPool = threadPool_;
    pthread ->id = id;

    pthread_create(&pthread->pthread,NULL,(void*)do_thread,pthread);
    pthread_detach(pthread->pthread);

    return 0;
}

void* do_thread(struct thread* pthread){
    struct ThreadPool* _threadPool = pthread->_threadPool;
    while(threadlive){
        bsem_wait(&_threadPool->threadQueue.has_work);
        if(threadlive){
            pthread_mutex_lock(&_threadPool->mutex);
            _threadPool->thread_running_num ++;
            pthread_mutex_unlock(&_threadPool->mutex);

            void (*func)(void* arg);
            void* arg;

            struct work* work;
            struct ThreadQueue_item* item = ThreadQueue_out(&_threadPool->threadQueue);
            work = item->work;
            if(work){
                func = work->func;
                arg = work->arg;
                func(arg);
                free(work);
            }

            pthread_mutex_lock(&_threadPool->mutex);
            _threadPool->thread_running_num --;

            if(!_threadPool->thread_running_num){
                pthread_cond_signal(&_threadPool->cond);
            }
            pthread_mutex_unlock(&_threadPool->mutex);
        }
    }
    pthread_mutex_lock(&_threadPool->mutex);
    _threadPool->thread_alive_num --;
    pthread_mutex_unlock(&_threadPool->mutex);
}

void thread_destroy(struct thread* thread){
    free(thread);
}


////////////////////////////ThreadQueue/////////////////////////////////////////
void ThreadQueue_init(struct ThreadQueue* threadQueue){
    threadQueue->size = 0;
    struct ThreadQueue_item* head = (struct ThreadQueue_item*)malloc(sizeof(struct ThreadQueue_item));
    head->work = NULL;
    head->next = NULL;
    threadQueue->head = head;
    threadQueue->pos = head;

    pthread_mutex_init(&threadQueue->rwmutext,NULL);
    bsem_init(&threadQueue->has_work,0);
}

struct ThreadQueue_item* ThreadQueue_out(struct ThreadQueue* threadQueue){
    pthread_mutex_lock(&threadQueue->rwmutext);
    struct ThreadQueue_item* item;
    struct ThreadQueue_item* now_pos;
    if(threadQueue->size == 0) return NULL;
    else{
        item = threadQueue->head->next;
        now_pos = threadQueue->head;
        while(now_pos->next != threadQueue->pos) now_pos = now_pos->next;
        threadQueue->pos = now_pos;
        threadQueue->head->next = threadQueue->head->next->next;
        threadQueue->size --;
    }
    pthread_mutex_unlock(&threadQueue->rwmutext);
    return item;
}


void ThreadQueue_in(struct ThreadQueue* threadQueue,struct ThreadQueue_item* new_work){
    pthread_mutex_lock(&threadQueue->rwmutext);
    threadQueue->pos->next = new_work;
    new_work->next = NULL;
    threadQueue->pos = threadQueue->pos->next;
    threadQueue->size ++;

    bsem_post(&threadQueue->has_work);
    pthread_mutex_unlock(&threadQueue->rwmutext);
}


////////////////////////////////bsem/////////////////////////////////////////////////

void bsem_init(struct bsem* bsem,int value){
    if(value < 0 || value > 1){
        err("only 0 and 1 can be used for bsem");
        return;
    }
    pthread_cond_init(&bsem->cond,NULL);
    pthread_mutex_init(&bsem->mutex,NULL);
    bsem->vaule = 0;
}

void bsem_reset(struct bsem* bsem){
    bsem_init(bsem,0);
}

void bsem_post(struct bsem* bsem){
    pthread_mutex_lock(&bsem->mutex);
    bsem->vaule = 1;
    pthread_cond_signal(&bsem->cond);
    pthread_mutex_unlock(&bsem->mutex);
}

void bsem_post_all(struct bsem* bsem){
    pthread_mutex_lock(&bsem->mutex);
    bsem->vaule = 1;
    pthread_cond_broadcast(&bsem->cond);
    pthread_mutex_unlock(&bsem->mutex);
}

void bsem_wait(struct bsem* bsem){
    pthread_mutex_lock(&bsem->mutex);
    while(bsem->vaule == 0){
        pthread_cond_wait(&bsem->cond,&bsem->mutex);
    }
    bsem->vaule = 0;
    pthread_mutex_unlock(&bsem->mutex);
}
//void *func(){
//    printf("sss");
//}
////test place
//int main(){
//    ThreadPool_init();
//    pthread_t a;
//    struct ThreadQueue_item ab;
//    ab.threadid = &a;
//    ab.func = func;
//    ab.arg = NULL;
//    int i = ThreadQueue_in(&threadQueue,&ab);
////    printf("%d   %d",threadQueue.threadqueue_tail,threadQueue.threadqueue_head);
//    struct ThreadQueue_item* pthread = ThreadQueue_out(&threadQueue);
//    pthread_create(pthread->threadid,NULL,pthread->func,NULL);
//    pthread_join(*pthread->threadid,NULL);
//}

