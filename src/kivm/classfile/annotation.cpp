//
// Created by kiva on 2018/5/11.
//

#include <kivm/classfile/annotation.h>
#include <kivm/memory/universe.h>

namespace kivm {
    ParameterAnnotation::ParameterAnnotation(cp_info **pool, parameter_annotations_t *source)
        : _count(source->num_annotations), _annos(nullptr) {
        _annos = (Annotation *) Universe::allocCObject(sizeof(Annotation) * _count);
        for (int i = 0; i < _count; ++i) {
            auto anno = _annos + i;
            ::new((void *) anno) Annotation(pool, source->annotations + i);
        }
    }

    ParameterAnnotation::~ParameterAnnotation() {
        for (int i = 0; i < _count; ++i) {
            auto anno = _annos + i;
            anno->~Annotation();
        }
        Universe::deallocCObject(_annos);
    }

    TypeAnnotation::TypeAnnotation(cp_info **pool, type_annotation *source)
        : _anno(new Annotation(pool, source->annotations)), _target_info(source->target_info) {
    }

    TypeAnnotation::~TypeAnnotation() {
        delete _anno;
    }

    Annotation::Annotation(cp_info **pool, annotation *anno)
        : _typeName(requireConstant<CONSTANT_Utf8_info>(pool, anno->type_index)->getConstant()) {
    }
}
