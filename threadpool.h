#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H
#include <queue>
#include <pthread.h>

void* thread_routine(void *args);

class ThreadPool
{
public:
    ThreadPool();

    friend void * thread_routine(void *args);

    //添加任务
    typedef void * (*callback_t) (void *);
    
    struct ThreadTask
    {
        //任务需要什么？回调函数,函数参数?
        callback_t run;
        void * args;
    };

    void AddTask(callback_t func, void * arg);
private:
    void StartTask();

private:
    //这里的线程池原理是，让一个线程，可以处理多个事务，从而避免大量的线程创建以及销毁的时间损耗
    //但是，不适合处理长连接或者会阻塞的事务
    std::queue<ThreadTask *> tasks;

    int maxthread;

    int current;  //当前线程数
    int idle;  //当前等待线程数
    
    pthread_mutex_t task_mutex;   //控制队列操作的锁,使用mutex_clock，即共享锁
    pthread_cond_t  task_cond;

    bool isquit;
};

#endif


