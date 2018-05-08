//
// Created by van on 17-9-2.
//

#ifndef TINY_SERVER_LOCKER_H
#define TINY_SERVER_LOCKER_H

#include <semaphore.h>
#include <pthread.h>

class Sem {
private:
    sem_t sem;

public:
    Sem() { sem_init(&sem, 0, 0); }
    ~Sem() { sem_destroy(&sem); }

    bool p() { return sem_wait(&sem) == 0; }
    bool v() { return sem_post(&sem) == 0; }

    bool tryp() { return sem_trywait(&sem) == 0; }
};

class Lock {
private:
    pthread_mutex_t mutex;

public:
    Lock() { pthread_mutex_init(&mutex, nullptr); }
    ~Lock() { pthread_mutex_destroy(&mutex); }

    bool lock() { return pthread_mutex_lock(&mutex) == 0; }
    bool unlock() { return pthread_mutex_unlock(&mutex) == 0; }
    bool trylock() { return pthread_mutex_trylock(&mutex) == 0; }
};


class Cond {
private:
    pthread_mutex_t mutex;
    pthread_cond_t cond;

public:
    Cond() {
        pthread_mutex_init(&mutex, nullptr);
        pthread_cond_init(&cond, nullptr);
    }

    ~Cond() {
        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&mutex);
    }

    bool wait() {
        pthread_mutex_lock(&mutex);
        int ret = pthread_cond_wait(&cond, nullptr);
        pthread_mutex_unlock(&mutex);
        return ret == 0;
    }

    bool singal() {
        return pthread_cond_signal(&cond) == 0;
    }
};

#endif //TINY_SERVER_LOCKER_H
