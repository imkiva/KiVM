//
// Created by kiva on 2018/4/20.
//

#include <kivm/memory/universe.h>
#include <chrono>

#define TIMES 1000000

using namespace kivm;

void bench(const char *tag, void(*func)()) {
    auto start = std::chrono::system_clock::now();
    func();
    auto end = std::chrono::system_clock::now();
    auto cost = end - start;
    printf("benchmark %s: %lld\n", tag, cost.count());
}

void benchMalloc() {
    for (int i = 0; i < TIMES; ++i) {
        auto m = (int *) malloc(sizeof(int));
        *m = i;
    }
}

void benchUniverse() {
    for (int i = 0; i < TIMES; ++i) {
        auto m = (int *) Universe::allocHeap(sizeof(int));
        *m = i;
    }
}

int main() {
    Universe::initialize();
    bench("malloc", benchMalloc);
    bench("universe", benchUniverse);
}
