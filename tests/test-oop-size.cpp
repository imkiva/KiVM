//
// Created by kiva on 2018/4/20.
//

#include <kivm/oop/oop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/oop/mirrorOop.h>

using namespace kivm;

int main() {
    printf("intOopDesc: %zd\n", sizeof(intOopDesc));
    printf("floatOopDesc: %zd\n", sizeof(floatOopDesc));
    printf("doubleOopDesc: %zd\n", sizeof(doubleOopDesc));
    printf("longOopDesc: %zd\n", sizeof(longOopDesc));
    printf("instanceOopDesc: %zd\n", sizeof(instanceOopDesc));
    printf("arrayOopDesc: %zd\n", sizeof(arrayOopDesc));
    printf("mirrorOopDesc: %zd\n", sizeof(mirrorOopDesc));
}
