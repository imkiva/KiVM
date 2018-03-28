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

        Klass *get_mirror_target() const {
            return _mirror_target;
        }

        void set_mirror_target(Klass *_mirror_target) {
            mirrorOopDesc::_mirror_target = _mirror_target;
        }

        ValueType get_mirroring_primitive_type() const {
            return _mirroring_primitive_type;
        }

        void set_mirroring_primitive_type(ValueType _mirroring_primitive_type) {
            set_mirror_target(nullptr);
            mirrorOopDesc::_mirroring_primitive_type = _mirroring_primitive_type;
        }

        bool is_mirror_for_primitive_types() const {
            return get_mirror_target() != nullptr;
        }
    };
}
