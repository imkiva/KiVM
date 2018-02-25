//
// Created by kiva on 2018/2/25.
//

#ifndef KIVAVM_LOCK_H
#define KIVAVM_LOCK_H

#include <pthread.h>
#include <shared/acl.h>

class Lock {
private:
    DISALLOW_COPY(Lock);
    pthread_mutexattr_t _attr;
    pthread_mutex_t _mutex;

public:
    Lock();

    void lock();

    void unlock();

    ~Lock();
};

class LockGuard {
private:
    Lock &lock;
public:
    LockGuard(Lock &lock) : lock(lock) { lock.lock(); }

    ~LockGuard() { lock.unlock(); }
};

#endif //KIVAVM_LOCK_H
