//
// Created by kiva on 2018/4/20.
//
#include <kivm/kivm.h>
#include <kivm/memory/collectedHeap.h>

namespace kivm {
    class Universe {
    private:
        static CollectedHeap *sCollectedHeapInstance;

    public:
        static void initialize();

        static void destroy();

        static void *allocVirtual(size_t size);

        static void deallocVirtual(void *memory);

        static void *allocHeap(size_t size);

        static void *allocCObject(size_t size);

        static void deallocCObject(void *memory);
    };
}
