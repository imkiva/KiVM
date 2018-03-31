//
// Created by kiva on 2018/2/28.
//
#pragma once

#include <kivm/oop/mirrorKlass.h>
#include <kivm/oop/instanceOop.h>

namespace kivm {
    class mirrorOopDesc : public instanceOopDesc {
    private:
        Klass *_mirror_target;
        ValueType _mirroring_primitive_type;

    public:
        explicit mirrorOopDesc(Klass *mirror);

        Klass *getMirrorTarget() const {
            return _mirror_target;
        }

        void setMirrorTarget(Klass *_mirror_target) {
            mirrorOopDesc::_mirror_target = _mirror_target;
        }

        ValueType getMirroringPrimitiveType() const {
            return _mirroring_primitive_type;
        }

        void setMirroringPrimitiveType(ValueType _mirroring_primitive_type) {
            setMirrorTarget(nullptr);
            mirrorOopDesc::_mirroring_primitive_type = _mirroring_primitive_type;
        }

        bool isMirroringPrimitiveTypes() const {
            return getMirrorTarget() != nullptr;
        }
    };
}
