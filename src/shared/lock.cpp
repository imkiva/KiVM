//
// Created by kiva on 2018/2/25.
//

#include <shared/lock.h>

Lock::Lock() {
    pthread_mutexattr_init(&_attr);
    pthread_mutexattr_settype(&_attr, PTHREAD_MUTEX_RECURSIVE);

    pthread_mutex_init(&_mutex, &_attr);
}

void Lock::lock() {
    pthread_mutex_lock(&_mutex);
}

void Lock::unlock() {
    pthread_mutex_unlock(&_mutex);
}

Lock::~Lock() {
    pthread_mutexattr_destroy(&_attr);
    pthread_mutex_destroy(&_mutex);
}
