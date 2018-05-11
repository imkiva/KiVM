//
// Created by kiva on 2018/5/11.
//
#pragma once

#include <kivm/classfile/attributeInfo.h>

namespace kivm {
    struct Annotation {
        String _typeName;
        annotation *_anno;

        Annotation(cp_info **pool, annotation *anno);

        inline bool checkTypeName(const String &typeName) {
            return typeName == _typeName;
        }
    };

    struct ParameterAnnotation {
        int _count;
        Annotation *_annos;

        explicit ParameterAnnotation(cp_info **pool, parameter_annotations_t *source);

        ~ParameterAnnotation();

        inline bool checkTypeName(const String &typeName) {
            for (int i = 0; i < _count; ++i) {
                if (_annos[i].checkTypeName(typeName)) {
                    return true;
                }
            }
            return false;
        }
    };

    struct TypeAnnotation {
        type_annotation::target_info_t *_target_info;
        Annotation *_anno;

        explicit TypeAnnotation(cp_info **pool, type_annotation *source);

        ~TypeAnnotation();

        inline bool checkTypeName(const String &typeName) {
            return _anno->checkTypeName(typeName);
        }
    };
}
