//
// Created by kiva on 2018/5/11.
//
#pragma once

#include <kivm/classfile/attributeInfo.h>

namespace kivm {
    struct ParameterAnnotation {
        int _count;
        annotation *_annos;

        explicit ParameterAnnotation(parameter_annotations_t *source);
    };

    struct TypeAnnotation {
        TypeAnnotation(type_annotation *source);
    };
}
