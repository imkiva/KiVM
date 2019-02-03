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
        friend class ObjectArrayKlass;

        friend class CopyingHeap;

    protected:
        std::vector<oop> _elements;

    private:
        explicit arrayOopDesc(arrayOop other)
            : oopDesc(other->getClass(), other->getMarkOop()->getOopType()),
              _elements(other->_elements) {
        }

    public:
        explicit arrayOopDesc(ArrayKlass *arrayClass, oopType type, int length);

        inline arrayOop copy() override {
            return new arrayOopDesc(this);
        }

        inline int getDimension() const {
            return ((ArrayKlass *) getClass())->getDimension();
        }

        inline int getLength() const {
            return static_cast<int>(_elements.size());
        }

        inline oop getElementAt(int position) const {
            assert(position >= 0 && position < getLength());
            return _elements[position];
        }

        inline void setElementAt(int position, oop element) {
            assert(position >= 0 && position < getLength());
            _elements[position] = element;
        }

        inline oop* getElementUnsafe(int position) {
            assert(position >= 0 && position < getLength());
            return &_elements[position];
        }
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
