//
// Created by kiva on 2018/4/20.
//

#include <kivm/memory/universe.h>
#include <sparsepp/spp.h>
#include <unordered_map>
#include <string>
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

void benchSppWithString() {
    spp::sparse_hash_map<std::string, std::string> m;
    for (int i = 0; i < TIMES; ++i) {
        m.insert(std::make_pair<std::string, std::string>(std::to_string(i), "hello fuck"));
    }
}

void benchStdWithString() {
    std::unordered_map<std::string, std::string> m;
    for (int i = 0; i < TIMES; ++i) {
        m.insert(std::make_pair<std::string, std::string>(std::to_string(i), "hello fuck"));
    }
}

void benchSppWithWString() {
    spp::sparse_hash_map<std::wstring, std::wstring> m;
    for (int i = 0; i < TIMES; ++i) {
        m.insert(std::make_pair<std::wstring, std::wstring>(std::to_wstring(i), L"hello fuck"));
    }
}

void benchStdWithWString() {
    std::unordered_map<std::wstring, std::wstring> m;
    for (int i = 0; i < TIMES; ++i) {
        m.insert(std::make_pair<std::wstring, std::wstring>(std::to_wstring(i), L"hello fuck"));
    }
}

int main() {
    bench("spp<std::string> ", benchSppWithString);
    bench("std<std::string> ", benchStdWithString);
    bench("spp<std::wstring>", benchSppWithWString);
    bench("std<std::wstring>", benchStdWithWString);
}
