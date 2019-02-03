//
// Created by kiva on 2019-02-02.
//

#include <kivm/oop/method.h>

int main() {
    using namespace kivm;
    parseArguments(L"([Ljava/lang/invoke/MethodType;[Ljava/lang/invoke/MethodType;)Ljava/lang/Object;");
    parseArguments(L"(BZSCIJFLjava/lang/Object;D)Ljava/lang/Object;");
    parseArguments(L"(BZSCIJFLjava/lang/Object;D)V");
    parseArguments(L"([[[[B[[[Z[[[Ljava/lang/String;)V");
}
