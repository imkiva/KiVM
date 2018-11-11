//
// Created by kiva on 2018/6/4.
//
#include <kivm/memory/gcThread.h>
#include <kivm/runtime/javaThread.h>
#include <sched.h>
#include <kivm/memory/universe.h>

namespace kivm {
    static Lock sGCTriggerLock;
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
                D("[GCThread]: waiting to be woken up");
                _triggerMonitor.enter();
                _triggerMonitor.wait();
                _triggerMonitor.leave();

                auto state = getState();
                if (state == GCState::WAITING_FOR_SAFEPOINT) {
                    D("[GCThread]: stopping the world");

                } else if (state == GCState::GC_STOPPED) {
                    D("GCThread: VM exited, stopping GC thread");
                    break;
                }
            }

            // Wait until all threads are in safepoint
            if (isAllThreadInSafePoint()) {
                D("[GCThread]: collecting");
                doGarbageCollection();
                continue;
            }

            sched_yield();
        }
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
        _gcState = GCState::GC_RUNNING;
        Universe::sCollectedHeapInstance->doGarbageCollection();
        _gcState = GCState::ENJOYING_HOLIDAY;

        // wake up all threads to continue their jobs
        _gcWaitMonitor.notifyAll();
    }

    Monitor *GCThread::required() {
        LockGuard lockGuard(sGCTriggerLock);
        if (_gcState != GCState::WAITING_FOR_SAFEPOINT) {
            // it is much safer to change status before triggering gc
            _gcState = GCState::WAITING_FOR_SAFEPOINT;

            // enter monitor so that other threads can do wait on it
            // and when gc finishes, just notify this monitor to wake
            // up all waiting threads to continue their jobs
            _gcWaitMonitor.enter();

            // notify our gc thread to work
            _triggerMonitor.notify();

            // if one triggered gc successfully
            // it should be responsible for this lock
            return &_gcWaitMonitor;
        }
        return nullptr;
    }

    void GCThread::stop() {
        if (_gcState == GCState::ENJOYING_HOLIDAY) {
            _gcState = GCState::GC_STOPPED;
            setThreadState(ThreadState::DIED);
            _triggerMonitor.notify();
            return;
        }

        if (_gcState == GCState::GC_RUNNING
            || _gcState == GCState::WAITING_FOR_SAFEPOINT) {
            setThreadState(ThreadState::DIED);
            this->_nativeThread->join();
        }
    }

    void GCThread::wait() {
        _gcWaitMonitor.wait();
    }
}
