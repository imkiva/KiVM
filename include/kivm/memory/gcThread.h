//
// Created by kiva on 2018/6/4.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/runtime/vmThread.h>
#include <shared/monitor.h>

namespace kivm {
    enum GCState {
        GC_STOPPED,
        GC_RUNNING,
        ENJOYING_HOLIDAY,
        WAITING_FOR_SAFEPOINT,
    };

    class GCThread : public VMThread {
    private:
        static GCThread *sGCThreadInstance;

    public:
        inline static GCThread *get() {
            if (sGCThreadInstance == nullptr) {
                WARN("GCThread not initialized");
                return nullptr;
            }
            return sGCThreadInstance;
        }

        static void initialize();

    private:
        GCState _gcState;
        Monitor _triggerMonitor;
        Monitor _gcWaitMonitor;

    private:
        bool isAllThreadInSafePoint();

        void doGarbageCollection();

    protected:
        void run() override;

    public:
        Monitor *required();

        void wait();

        void stop();

        inline GCState getState() const {
            return _gcState;
        }
    };
}
