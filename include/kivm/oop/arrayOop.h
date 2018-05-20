//
// Created by kiva on 2018/2/28.
//

#pragma once

#include <kivm/oop/instanceOop.h>
#include <kivm/oop/arrayKlass.h>
#include <vector>

namespace kivm {
    class arrayOopDesc : public oopDesc {
        // copyArrayTo() needs to use _elements
        friend class TypeArrayKlass;

    private:
        std::vector<oop> _elements;

    private:
        void arrayIndexOutOfBounds(int position) const;

    public:
        explicit arrayOopDesc(ArrayKlass *arrayClass, oopType type, int length);

        inline arrayOop shallowCopy() {
            return nullptr;
        }

        inline int getDimension() const {
            return ((ArrayKlass *) getClass())->getDimension();
        }

        inline int getLength() const {
            return static_cast<int>(_elements.size());
        }

        oop getElementAt(int position) const;

        void setElementAt(int position, oop element);
    };

    class typeArrayOopDesc : public arrayOopDesc {
    public:
        typeArrayOopDesc(TypeArrayKlass *arrayClass, int length);
    };

    class objectArrayOopDesc : public arrayOopDesc {
    public:
        objectArrayOopDesc(ObjectArrayKlass *arrayClass, int length);
    };
}
