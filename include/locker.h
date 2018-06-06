// Created by Van on 2018-06-05
// @brief: this file is created for thread pool
// thread pool is used to hold some work thread for server
#ifndef _THANOS_LOCKER_H
#define _THANOS_LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

namespace thanos {

class Sem {
public:
    Sem();
    ~Sem();

    bool sem_init();
    bool sem_destroy();

    bool wait();
    bool post();

private:
    sem_t _sem;

    // disallow copy & assign
    Sem(const Sem& other);
    Sem& operator=(const Sem& other);
};

class Locker {
public:
    Locker();
    ~Locker();

    bool locker_init();
    bool locker_destroy();

    bool lock();
    bool unlock();

private:
    pthread_mutex_t _mutex;

    // disallow copy & assign
    Locker(const Locker& other);
    Locker& operator=(const Locker& other);
};

class Cond {
public:
    Cond();
    ~Cond();

    bool cond_init();
    bool cond_destroy();

    bool wait();
    bool signal();

private:
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;

    // disallow copy&assign
    Cond(const Cond& other);
    Cond& operator=(const Cond& other);
};

} // namespace thanos
#endif
