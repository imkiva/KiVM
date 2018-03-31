//
// Created by kiva on 2018/3/23.
//

#include <cassert>
#include <kivm/runtime/stack.h>

using namespace kivm;

int main() {
    Stack s(6);
    s.pushInt(1);
    s.pushFloat(3.14f);
    s.pushDouble(6.28);
    s.pushLong(10086);

    jlong j = s.popLong();
    assert(j == 10086L);
    jdouble d = s.popDouble();
    assert(d == 6.28);
    jfloat f = s.popFloat();
    assert(f == 3.14f);
    assert(s.popInt() == 1);
    return 0;
}
