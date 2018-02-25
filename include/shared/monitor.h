//
// Created by kiva on 2018/2/25.
//

#ifndef KIVAVM_MONITOR_H
#define KIVAVM_MONITOR_H

#include <pthread.h>
#include <sys/time.h>

class Monitor {
private:
    pthread_mutex_t _mutex;
    pthread_mutexattr_t _attr;
    pthread_cond_t _cond;

public:
    Monitor() {
        pthread_mutexattr_init(&_attr);
        pthread_mutexattr_settype(&_attr, PTHREAD_MUTEX_RECURSIVE);

        pthread_mutex_init(&_mutex, &_attr);
        pthread_cond_init(&_cond, nullptr);
    }

    void enter() {
        pthread_mutex_lock(&_mutex);
    }

    void wait() {
        pthread_cond_wait(&_cond, &_mutex);
    }

    void wait(long timesec) {
        struct timeval val{};
        gettimeofday(&val, nullptr);
        val.tv_usec += timesec;

        struct timespec spec{};
        spec.tv_sec = val.tv_sec;
        spec.tv_nsec = val.tv_usec * 1000;
        pthread_cond_timedwait(&_cond, &_mutex, &spec);
    }

    void notify() {
        pthread_cond_signal(&_cond);
    }

    void notify_all() {
        pthread_cond_broadcast(&_cond);
    }

    void leave() {
        pthread_mutex_unlock(&_mutex);
    }

    void force_unlock_when_athrow() {
        pthread_mutex_trylock(&_mutex);
        pthread_mutex_unlock(&_mutex);
    }

    ~Monitor() {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
    }
};

#endif //KIVAVM_MONITOR_H
