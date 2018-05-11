//
// Created by kiva on 2018/5/11.
//
#pragma once

#include <kivm/classfile/attributeInfo.h>

namespace kivm {
    struct value_t {
        virtual ~value_t() = default;
    };

    struct const_value_t : public value_t {
        u2 const_value_index;
    };

    struct enum_const_value_t : public value_t {
        u2 type_name_index;
        u2 const_name_index;
    };

    struct class_info_t : public value_t {
        u2 class_info_index;
    };

    struct element_value {
        u1 tag;
        value_t *value = nullptr;

        ~element_value();
    };

    struct element_value_pairs_t {
        u2 element_name_index;
        element_value value;
    };

    struct annotation : public value_t {
        u2 type_index;
        u2 num_element_value_pairs;
        element_value_pairs_t *element_value_pairs = nullptr;

        ~annotation() override;
    };

    struct array_value_t : public value_t {
        u2 num_values;
        element_value *values = nullptr;

        ~array_value_t() override;
    };

    struct type_annotation {
        // target_type
        struct target_info_t {
            virtual ~target_info_t() = default;
        };

        struct type_parameter_target : target_info_t {
            u1 type_parameter_index;
        };

        struct supertype_target : target_info_t {
            u2 supertype_index;
        };

        struct type_parameter_bound_target : target_info_t {
            u1 type_parameter_index;
            u1 bound_index;
        };

        struct empty_target : target_info_t {
        };

        struct formal_parameter_target : target_info_t {
            u1 formal_parameter_index;
        };

        struct throws_target : target_info_t {
            u2 throws_type_index;
        };

        struct table_t {
            u2 start_pc;
            u2 length;
            u2 index;
        };

        struct localvar_target : target_info_t {
            u2 table_length;
            table_t *table = nullptr;

            ~localvar_target() override;
        };

        struct catch_target : target_info_t {
            u2 exception_table_index;
        };

        struct offset_target : target_info_t {
            u2 offset;
        };

        struct type_argument_target : target_info_t {
            u2 offset;
            u1 type_argument_index;
        };

        struct path_t {
            u1 type_path_kind;
            u1 type_argument_index;
        };

        // type_path
        struct type_path {
            u1 path_length;
            path_t *path = nullptr;

            ~type_path();
        };

        // basic
        u1 target_type;
        target_info_t *target_info = nullptr;
        type_path target_path;
        annotation *annotations = nullptr;

        ~type_annotation();
    };

    struct parameter_annotations_t {
        u2 num_annotations;
        annotation *annotations = nullptr;

        ~parameter_annotations_t();
    };

    struct RuntimeVisibleAnnotations_attribute : public attribute_info {
        parameter_annotations_t parameter_annotations;
    };

    struct RuntimeInvisibleAnnotations_attribute : public attribute_info {
        parameter_annotations_t parameter_annotations;
    };

    struct RuntimeVisibleParameterAnnotations_attribute : public attribute_info {
        u1 num_parameters;
        parameter_annotations_t *parameter_annotations = nullptr;

        ~RuntimeVisibleParameterAnnotations_attribute() override;
    };

    struct RuntimeInvisibleParameterAnnotations_attribute : public attribute_info {
        u1 num_parameters;
        parameter_annotations_t *parameter_annotations = nullptr;

        ~RuntimeInvisibleParameterAnnotations_attribute() override;
    };

    struct RuntimeVisibleTypeAnnotations_attribute : public attribute_info {
        u2 num_annotations;
        type_annotation *annotations = nullptr;

        ~RuntimeVisibleTypeAnnotations_attribute() override;
    };

    struct RuntimeInvisibleTypeAnnotations_attribute : public attribute_info {
        u2 num_annotations;
        type_annotation *annotations = nullptr;

        ~RuntimeInvisibleTypeAnnotations_attribute() override;
    };

    struct AnnotationDefault_attribute : public attribute_info {
        element_value default_value;
    };
}

