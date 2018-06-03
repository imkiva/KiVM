//
// Created by kiva on 2018/6/4.
//
#include <kivm/gc/gcThread.h>
#include <kivm/runtime/javaThread.h>
#include <sched.h>

namespace kivm {
    GCThread *GCThread::sGCThreadInstance = nullptr;

    void GCThread::initialize() {
        GCThread::sGCThreadInstance = new GCThread;
    }

    void GCThread::run() {
        setThreadName(L"GCThread");

        while (getThreadState() != ThreadState::DIED) {
            // Wait until all threads are in safepoint
            if (isAllThreadInSafePoint()) {
                D("GCThread: triggering garbage collection");
                doGarbageCollection();
            }

            sched_yield();
        }

        D("GCThread: VM exited, stopping GC thread");
    }

    bool GCThread::isAllThreadInSafePoint() {
        int total = 0;
        int inSafepoint = 0;

        Threads::forEach([&](JavaThread *thread) {
            ++total;
            if (thread->getThreadState() != ThreadState::DIED
                && thread->isInSafepoint()) {
                ++inSafepoint;
            }
            return false;
        });

        return inSafepoint == total;
    }

    void GCThread::doGarbageCollection() {
        D("TODO");
    }
}
