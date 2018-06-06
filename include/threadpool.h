// created by Van on 2018-06-05
//
// @brief: 线程池拥有多个工作线程，并协调它们完成工作
// @how to use: 
//   只要句柄类实现了process()方法即可，工作线程会自行
//   调用句柄类的process()方法。要让其运行首先要用init初始化
//   线程池，让后用append方法压入句柄类的指针即可。

#ifndef _THANOS_THREAD_POOL_H
#define _THANOS_THREAD_POOL_H

#include <queue>
#include <exception>
#include <cstdio>
#include <new>
#include <pthread.h>
#include <stdint.h>
#include "locker.h"
#include "connection.h"
#include "easylogging++.h"

namespace thanos {

template <typename T>
class ThreadPool {
public:
    ThreadPool();
    ~ThreadPool();

    bool append(T* req);
    void run();
    // 用于初始化线程池
    // @max_thread_num: 线程池的工作线程个数
    // @max_requests: 请求队列中最多允许的等待请求队列
    bool init(uint32_t max_thread_num, uint64_t max_requests);

private:
    // 此静态函数是工作线程的句柄
    static void* _thread_handler(void* arg);
    void _destroy();

    uint32_t _max_thread_num; // 线程池中最多几个工作线程
    uint64_t _max_requests;   // 请求队列中允许的最大请求数
    pthread_t* _work_threads; // 工作线程数组
    std::queue<T*> _req_que;  // 请求队列
    Locker _que_lock;         // 请求队列属于竞争资源，需要锁来保护
    Sem _que_stat;            // 信号量用于在请求队列非空时唤醒工作线程
    bool _is_run;             // 是否在运行
};

template <typename T>
ThreadPool<T>::ThreadPool() : 
        _max_thread_num(0), _max_requests(0), 
        _work_threads(nullptr), _req_que(),
        _que_lock(), _que_stat(), _is_run(false) {}

template <typename T>
ThreadPool<T>::~ThreadPool() {
    _destroy();
}

template <typename T>
void ThreadPool<T>::_destroy() {
    if (_work_threads != nullptr) {
        delete [] _work_threads;
    }
    _is_run = false;
}

template <typename T>
bool ThreadPool<T>::init(uint32_t max_thread_num, uint64_t max_requests) {
    if (max_thread_num < 0 || max_requests < 0) {
        LOG(WARNING) << "[ThreadPool::init]: invalid parameter";
        return false;
    }

    _max_thread_num = max_thread_num;
    _max_requests = max_requests;

    _work_threads = new (std::nothrow) pthread_t[_max_thread_num];
    if (_work_threads == nullptr) {
        LOG(WARNING) << "[ThreadPool::init]: new _work_threads failed";
        return false;
    }

    LOG(INFO) << "Create " << _max_thread_num << " threads in thread pool";
    for (int i = 0; i < _max_thread_num; ++i) {
        if (pthread_create(_work_threads + i, nullptr, _thread_handler, this) != 0) {
            LOG(WARNING) << "[ThreadPool::init]: pthread_create failed";
            _destroy();
            return false;
        }
        if (pthread_detach(_work_threads[i]) != 0) {
            LOG(WARNING) << "[ThreadPool::init]: pthread_detach failed";
            _destroy();
            return false;
        }
    }
    _is_run = true;
    return true;
}

template <typename T>
bool ThreadPool<T>::append(T* req) {
    if (!_que_lock.lock()) {
        LOG(WARNING) << "[ThreadPool::append]:lock failed";
        return false;
    }
    if (_req_que.size() > _max_requests) {
        if (!_que_lock.unlock()) {
            LOG(WARNING) << "[ThreadPool::append]:unlock failed in size > _max_requests";
        }
        return false;
    }
    _req_que.push(req);
    if (!_que_lock.unlock()) {
        LOG(WARNING) << "[ThreadPool::append]:unlock failed";
        return false;
    }
    if (!_que_stat.post()) {
        LOG(WARNING) << "[ThreadPool::append]:sem post failed";
        return false;
    }
    return true;
}

template <typename T>
void ThreadPool<T>::run() {
    while (_is_run) {
        _que_stat.wait();

        if (!_que_lock.lock()) {
            LOG(WARNING) << "[ThreadPool::thread_handler]: que lock failed";
            return;
        }
        if (_req_que.empty()) {
            _que_lock.unlock();
            continue;
        }

        T* req = _req_que.front();
        _req_que.pop();

        if (!_que_lock.unlock()) {
            LOG(WARNING) << "[ThreadPool::thread_handler]: que unlock failed";
            return;
        }
        if (req == nullptr) {
            continue;
        }

        req->process();
    }
}

template <typename T>
void* ThreadPool<T>::_thread_handler(void* arg) {
    if (arg == nullptr) {
        LOG(WARNING) << "[ThreadPool::thread_handler]: nullptr";
        return nullptr;
    }

    ThreadPool* threadpool = reinterpret_cast<ThreadPool*>(arg);
    if (threadpool == nullptr) {
        LOG(WARNING) << "[ThreadPool::thread_handler]: cast failed";
        return nullptr;
    }

    threadpool->run();
    return threadpool;
}

} // namespace thanos


#endif
