//
// Created by kiva on 2018/2/28.
//
#pragma once

#include <kivm/oop/mirrorKlass.h>
#include <kivm/oop/instanceOop.h>

namespace kivm {
    class mirrorOopDesc : public instanceOopDesc {
        friend class CopyingHeap;

    private:
        Klass *_mirrorTarget = nullptr;
        ValueType _mirroringPrimitiveType;

    public:
        explicit mirrorOopDesc(Klass *mirror);

        Klass *getTarget() const {
            return _mirrorTarget;
        }

        void setTarget(Klass *_mirrorTarget) {
            mirrorOopDesc::_mirrorTarget = _mirrorTarget;
        }

        ValueType getPrimitiveType() const {
            return _mirroringPrimitiveType;
        }

        void setPrimitiveType(ValueType mirroringPrimitiveType) {
            setTarget(nullptr);
            mirrorOopDesc::_mirroringPrimitiveType = mirroringPrimitiveType;
        }

        bool isPrimitiveMirror() const {
            return getTarget() == nullptr;
        }
    };
}
