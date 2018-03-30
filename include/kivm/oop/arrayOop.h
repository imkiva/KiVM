//
// Created by kiva on 2018/2/28.
//

#pragma once

#include <kivm/oop/instanceOop.h>
#include <kivm/oop/arrayKlass.h>
#include <vector>

namespace kivm {
    class arrayOopDesc : public oopDesc {
    private:
        std::vector<oop> _elements;

    public:
        explicit arrayOopDesc(ArrayKlass *array_klass, oopType type, int length);

        int get_dimension() const;

        int get_length() const;

        oop get_element_at(int position) const;

        void set_element_at(int position, oop element);
    };

    class typeArrayOopDesc : public arrayOopDesc {
    public:
        typeArrayOopDesc(TypeArrayKlass *array_klass, int length);
    };

    class objectArrayOopDesc : public arrayOopDesc {
    public:
        objectArrayOopDesc(ObjectArrayKlass *array_klass, int length);
    };
}
