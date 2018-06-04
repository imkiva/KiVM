//
// Created by kiva on 2018/6/4.
//
#pragma once

#include <kivm/kivm.h>
#include <kivm/runtime/vmThread.h>
#include <shared/monitor.h>

namespace kivm {
    class GCThread : public VMThread {
    private:
        static GCThread *sGCThreadInstance;

    public:
        inline static GCThread *get() {
            if (sGCThreadInstance == nullptr) {
                PANIC("GCThread not initialized");
            }
            return sGCThreadInstance;
        }

        static void initialize();

    private:
        Monitor _triggerMonitor;

    private:
        bool isAllThreadInSafePoint();

        void doGarbageCollection();

    protected:
        void run() override;

    public:
        void required();
    };
}
