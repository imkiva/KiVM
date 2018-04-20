//
// Created by kiva on 2018/4/20.
//
#include <kivm/kivm.h>

namespace kivm {
    class Universe {
    private:
        static CollectedHeap* sCollectedHeapInstance;

    public:
        static void initialize();

        static void *allocVirtual(size_t size);

        static void *deallocVirtual(void *memory, size_t size);

        static inline void memoryZero(void *memory, size_t size) {
            if (memory != nullptr) {
                memset(memory, '\0', size);
            }
        }
    };
}
