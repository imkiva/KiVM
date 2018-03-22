//
// Created by kiva on 2018/3/23.
//

#include <cassert>
#include <kivm/runtime/stack.h>

using namespace kivm;

int main() {
    Stack s(6);
    s.push_int(1);
    s.push_float(3.14f);
    s.push_double(6.28);
    s.push_long(10086);

    jlong j = s.pop_long();
    assert(j == 10086L);
    // FIXME: double
//    jdouble d = s.pop_double();
//    assert(d == 6.28);
    jfloat f = s.pop_float();
    assert(f == 3.14f);
    assert(s.pop_int() == 1);
    return 0;
}
