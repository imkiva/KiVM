//
// Created by kiva on 2018/6/4.
//
#include <kivm/gc/gcThread.h>
#include <kivm/runtime/javaThread.h>
#include <sched.h>
#include <kivm/memory/universe.h>

namespace kivm {
    GCThread *GCThread::sGCThreadInstance = nullptr;

    void GCThread::initialize() {
        GCThread::sGCThreadInstance = new GCThread;
        sGCThreadInstance->_gcState = GCState::ENJOYING_HOLIDAY;
    }

    void GCThread::run() {
        setThreadName(L"GCThread");

        while (getThreadState() != ThreadState::DIED) {
            // Wait until GC is required
            if (getState() == GCState::ENJOYING_HOLIDAY) {
                _triggerMonitor.wait();
            }

            // Wait until all threads are in safepoint
            if (isAllThreadInSafePoint()) {
                doGarbageCollection();
            }

            sched_yield();
        }

        D("GCThread: VM exited, stopping GC thread");
    }

    bool GCThread::isAllThreadInSafePoint() {
        _gcState = GCState::WAITING_FOR_SAFEPOINT;
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
        _gcState = GCState::RUNNING;
        Universe::sCollectedHeapInstance->doGarbageCollection();
        _gcState = GCState::ENJOYING_HOLIDAY;

        // notify all threads that GC is finished
        _gcWaitMonitor.notifyAll();
    }

    void GCThread::required() {
        _triggerMonitor.notify();
    }

    void GCThread::wait() {
        _gcWaitMonitor.wait();
    }
}
