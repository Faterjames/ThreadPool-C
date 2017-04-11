# ThreadPool-C
a C threadPool

# USE

add the .h and .c files to your project,remember to make sure you compile you project with pthread.h

//ThreadPool_init(int num)
use for create a new threadpool ,the number of this threadpool cover is the num

//ThreadPool_work_in(ThreadPool* th)
use for add new work to your threapool

//ThreadPool_wait(ThreadPool* th)
use for wait all the tasks finish 

//ThreadPool_destroy(ThreadPool* th)
use for destroy your threadpool

# Quite a simple threadpool to learn and use :)
