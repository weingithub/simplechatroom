#include "threadpool.h"
#include "common.h"

using namespace std;

void* thread_routine(void *args)
{
    ThreadPool * pthr = (ThreadPool *) args;

    pthread_detach(pthread_self());  
    cout<<"*thread 0x"<<(unsigned long)pthread_self()<<" is starting..."<<endl;  

    //启动线程后，开始做事
     struct timespec outtime;  

    while(1)
    {
        ++pthr->idle;
        bool timeout = false;

        //要操作队列，先加锁
        
        pthread_mutex_lock(&pthr->task_mutex);

        //判断任务队列是否为空
        while (pthr->tasks.empty() && !pthr->isquit)
        {
            //任务队列为空，则当前线程等待
            outtime.tv_sec = time(NULL)+ 300; //10s等待时间
            outtime.tv_nsec = 0;

            int ret = pthread_cond_timedwait(&pthr->task_cond, &pthr->task_mutex, &outtime);        

            cout<<"pthread_cond_timedwait 的返回值是:"<<ret<<endl;
            if (ret == ETIMEDOUT)
            {
                //超时
                cout<<"线程超时了"<<endl;
                timeout = true;
                break;
            }
            else if (ret)
            {
                timeout = true;
                cout<<"条件触发了,reason:"<<strerror(errno)<<endl;
                break;
            }
        }

        //判断是否超时
        --pthr->idle;
        if (timeout && pthr->tasks.empty())
        {
            //线程结束
            --pthr->current;
            
            //当前没有工作或睡眠的线程
            if (0 == pthr->current)
            {
                //发送唤醒通知？
                pthread_cond_signal(&pthr->task_cond);
            }

            pthread_mutex_unlock(&pthr->task_mutex);
            break;
        }

        //有任务
        if (!pthr->tasks.empty())
        {
            cout<<"线程池准备执行线程了"<<endl;
            //执行任务
            ThreadPool::ThreadTask *t = pthr->tasks.front();
            pthr->tasks.pop();
            
            //如果方法是阻塞的，那么，这里也会阻塞住
            pthread_mutex_unlock(&pthr->task_mutex);
            t->run(t->args);

            delete t;       
        }

    }

    cout<<"*thread 0x"<<(unsigned long)pthread_self()<<" is end..."<<endl;  

    return (void *)0;
}

ThreadPool::ThreadPool():
    maxthread(5),
    current(0),
    isquit(false),
    idle(0)
{
    pthread_mutex_init(&task_mutex, NULL);   
    pthread_cond_init(&task_cond, NULL);
}

void ThreadPool::AddTask(callback_t func, void * arg)
{
    //先将任务加进队列
    ThreadTask *ptask = new ThreadTask;

    ptask->run = func;
    ptask->args = arg;

    //加锁
    pthread_mutex_lock(&task_mutex);

    tasks.push(ptask);

    //启动任务
    StartTask();

    pthread_mutex_unlock(&task_mutex);
}

void ThreadPool::StartTask()
{
    cout<<"StartTask"<<endl;
    
    if (idle  > 0)
    {
        //如何唤醒线程？
        pthread_cond_signal(&task_cond);

    }
    else if (current < maxthread)
    {
        pthread_t tid;  
        pthread_create(&tid, NULL, thread_routine, this);  
        ++current;  
    }
    else
    {
        //进行线程池的扩展
        maxthread *= 2;
        pthread_t tid;
        pthread_create(&tid, NULL, thread_routine, this);
        ++current;
    }

}
