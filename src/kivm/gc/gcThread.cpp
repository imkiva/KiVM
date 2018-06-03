//
// Created by kiva on 2018/6/4.
//
#include <kivm/gc/gcThread.h>
#include <kivm/runtime/javaThread.h>

namespace kivm {

    void GCThread::run() {
        while (getThreadState() != ThreadState::DIED) {
            waitForSafePoint();
            doGarbageCollection();
        }
    }

    bool GCThread::isAllThreadInSafePoint() {
        LockGuard lockGuard(Threads::threadStateChangeLock());
        int total = Threads::getRunningJavaThreadCountLocked();
        int inSafepoint = 0;

        Threads::forEach([&](JavaThread *thread) {
            if (thread->getThreadState() != ThreadState::DIED
                && thread->isInSafepoint()) {
                ++inSafepoint;
            }
            return false;
        });

        return inSafepoint == total;
    }

    void GCThread::waitForSafePoint() {
        for (;;) {
            // Wait until all threads are in safepoint
            if (isAllThreadInSafePoint()) {
                break;
            }

            sched_yield();
        }
    }

    void GCThread::doGarbageCollection() {

    }
}
