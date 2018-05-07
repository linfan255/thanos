//
// Created by van on 17-9-2.
//
/*
 * 一个线程池由两大部分组成：
 *      1、一个任务队列，用来保存要执行的任务的队列，在服务器中通常是一个待处理的连接
 *      2、固定数量（这里设为10个）的cpu执行实体，即线程
 *
 * 设计思路：
 *      考虑没有任务要处理的情景，此时所有10个线程由于没有要执行的任务，所以阻塞。（用信号量）
 *      如果来了一个任务要处理，压入队列。此时每个线程在要去执行之前判断的信号量大于0.故而不再阻塞。处理该任务
 *
 * 对队列的操作需要用互斥量来保证原子性
 * */

#ifndef TINY_SERVER_THREADPOOL_H
#define TINY_SERVER_THREADPOOL_H
#include "locker.h"
#include <deque>
#include <pthread.h>

template <typename T>
class Threadpool {
private:
    std::deque<T*> task_que;
    pthread_t* threads;         //任务队列和线程数组

    Lock mutex;
    Sem sem;                    //用于同步的互斥量和信号量

    int task_maxnum;    //任务的上限数量
    int threads_num;    //当前线程池中线程的数量
    bool isContinue;

public:
    Threadpool(int task_maxnum = 1024, int threads_num = 10);
    ~Threadpool();
    bool add_task(T* task);

protected:
    static void* thread_headler(void* tp); //每个线程执行的运行函数
};

template <typename T>
void* Threadpool<T>::thread_headler(void *tp) {
    Threadpool* p = (Threadpool*)tp;

    while(p->isContinue) {
        p->sem.p();
        p->mutex.lock();
        if(p->task_que.empty()) {
            p->mutex.unlock();
            continue;
        }
        T* task = p->task_que.front();
        p->task_que.pop_front();
        p->mutex.unlock();

        task->process();
    }

    return p;
}

template <typename T>
Threadpool<T>::Threadpool(int _task_maxnum, int _thread_num):
        task_maxnum(_task_maxnum), threads_num(_thread_num),
        threads(nullptr), isContinue(true)
{
    threads = new pthread_t[threads_num];
    if(threads == nullptr) {
        std::exception();
    }

    for(int i = 0; i < threads_num; i++) {
        if(pthread_create(threads + i, nullptr, thread_headler, this) != 0) {
            delete[] threads;
            std::exception();
        }

        if(pthread_detach(threads[i]) != 0) {
            delete[] threads;
            std::exception();
        }
    }

    task_que.clear();
}


template <typename T>
Threadpool<T>::~Threadpool() {
    delete[] threads;
    isContinue = false;
}


template <typename T>
bool Threadpool<T>::add_task(T *task) {
    mutex.lock();

    if(task_que.size() >= task_maxnum) {
        mutex.unlock();
        return false;
    }

    task_que.push_back(task);
    mutex.unlock();
    sem.v();
    return true;
}


#endif //TINY_SERVER_THREADPOOL_H
