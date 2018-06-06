// Created by Van on 2018-06-05
#include "locker.h"
#include "easylogging++.h"

namespace thanos {

Sem::Sem() : _sem() {};

Sem::~Sem() = default;

bool Sem::sem_init() {
    if (::sem_init(&_sem, 0, 0) == -1) {
        LOG(WARNING) << "[Sem::sem_init]: sem_init failed";
        return false;
    }
    return true;
}

bool Sem::sem_destroy() {
    if (::sem_destroy(&_sem) == -1) {
        LOG(WARNING) << "[Sem::sem_destroy]: failed";
        return false;
    }
    return true;
}

bool Sem::wait() {
    if (::sem_wait(&_sem) == -1) {
        LOG(WARNING) << "[Sem::sem_wait]: failed";
        return false;
    }
    return true;
}

bool Sem::post() {
    if (::sem_post(&_sem) == -1) {
        LOG(WARNING) << "[Sem::sem_post]: failed";
        return false;
    }
    return true;
}

Locker::Locker() : _mutex() {};

Locker::~Locker() = default;

bool Locker::locker_init() {
    if (pthread_mutex_init(&_mutex, nullptr) != 0) {
        LOG(WARNING) << "[Locker::locker_init]: failed";
        return false;
    }
    return true;
}

bool Locker::locker_destroy() {
    if (pthread_mutex_destroy(&_mutex) != 0) {
        LOG(WARNING) << "[Locker::locker_destroy]: failed";
        return false;
    }
    return true;
}

bool Locker::lock() {
    if (pthread_mutex_lock(&_mutex) != 0) {
        LOG(WARNING) << "[Locker::lock]: failed";
        return false;
    }
    return true;
}

bool Locker::unlock() {
    if (pthread_mutex_unlock(&_mutex) != 0) {
        LOG(WARNING) << "[Locker::unlock]: failed";
        return false;
    }
    return true;
}

Cond::Cond() : _mutex(), _cond() {};

Cond::~Cond() = default;

bool Cond::cond_init() {
    if (pthread_mutex_init(&_mutex, nullptr) != 0) {
        LOG(WARNING) << "[Cond::cond_init]: mutex init failed";
        return false;
    }
    if (pthread_cond_init(&_cond, nullptr) != 0) {
        LOG(WARNING) << "[Cond::cond_init]: cond init failed";
        pthread_mutex_destroy(&_mutex);
        return false;
    }
    return true;
}

bool Cond::cond_destroy() {
    if (pthread_mutex_destroy(&_mutex) != 0) {
        LOG(WARNING) << "[Cond::cond_destroy]: mutex_destroy failed";
        return false;
    }
    if (pthread_cond_destroy(&_cond) != 0) {
        LOG(WARNING) << "[Cond::cond_destroy]: cond_destroy failed";
        return false;
    }
    return true;
}

bool Cond::wait() {
    if (pthread_mutex_lock(&_mutex) != 0) {
        LOG(WARNING) << "[Cond::wait]: mutex lock failed";
        return false;
    }
    if (pthread_cond_wait(&_cond, &_mutex) != 0) {
        LOG(WARNING) << "[Cond::wait]: cond wait failed";
        return false;
    }
    if (pthread_mutex_unlock(&_mutex) != 0) {
        LOG(WARNING) << "[Cond::wait]: mutex unlock failed";
        return false;
    }
    return true;
}

bool Cond::signal() {
    if (pthread_cond_signal(&_cond) != 0) {
        LOG(WARNING) << "[Cond::signal]: signal failed";
        return false;
    }
    return true;
}

}
