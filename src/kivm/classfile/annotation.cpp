//
// Created by kiva on 2018/5/11.
//
#include <kivm/classfile/annotation.h>
#include <kivm/classfile/classFileStream.h>

namespace kivm {
    ClassFileStream &operator>>(ClassFileStream &, element_value_pairs_t &);

    ClassFileStream &operator>>(ClassFileStream &, array_value_t &);

    ClassFileStream &operator>>(ClassFileStream &stream, const_value_t &info) {
        info.const_value_index = stream.get2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, enum_const_value_t &info) {
        info.type_name_index = stream.get2();
        info.const_name_index = stream.get2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, class_info_t &info) {
        info.class_info_index = stream.get2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, annotation &info) {
        info.type_index = stream.get2();
        info.num_element_value_pairs = stream.get2();
        info.element_value_pairs = new element_value_pairs_t[info.num_element_value_pairs];
        for (int i = 0; i < info.num_element_value_pairs; i++) {
            stream >> info.element_value_pairs[i];
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, element_value &info) {
        info.tag = stream.get1();
        switch ((char) info.tag) {
            case 'B':
            case 'C':
            case 'D':
            case 'F':
            case 'I':
            case 'J':
            case 'S':
            case 'Z':
            case 's': {
                info.value = new const_value_t;
                stream >> *(const_value_t *) info.value;
                break;
            }
            case 'e': {
                info.value = new enum_const_value_t;
                stream >> *(enum_const_value_t *) info.value;
                break;
            }
            case 'c': {
                info.value = new class_info_t;
                stream >> *(class_info_t *) info.value;
                break;
            }
            case '@': {
                info.value = new annotation;
                stream >> *(annotation *) info.value;
                break;
            }
            case '[': {
                info.value = new array_value_t;
                stream >> *(array_value_t *) info.value;
                break;
            }
            default: {
                assert(false);
            }
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, element_value_pairs_t &info) {
        info.element_name_index = stream.get2();
        stream >> info.value;
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, array_value_t &info) {
        info.num_values = stream.get2();
        info.values = new element_value[info.num_values];
        for (int i = 0; i < info.num_values; i++) {
            stream >> info.values[i];
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, type_annotation::type_parameter_target &info) {
        info.type_parameter_index = stream.get1();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, type_annotation::supertype_target &info) {
        info.supertype_index = stream.get2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, type_annotation::type_parameter_bound_target &info) {
        info.type_parameter_index = stream.get1();
        info.bound_index = stream.get1();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, type_annotation::empty_target &info) {
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, type_annotation::formal_parameter_target &info) {
        info.formal_parameter_index = stream.get1();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, type_annotation::throws_target &info) {
        info.throws_type_index = stream.get2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, type_annotation::table_t &info) {
        info.start_pc = stream.get2();
        info.length = stream.get2();
        info.index = stream.get2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, type_annotation::localvar_target &info) {
        info.table_length = stream.get2();
        info.table = new type_annotation::table_t[info.table_length];
        for (int i = 0; i < info.table_length; i++) {
            stream >> info.table[i];
        }

        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, type_annotation::catch_target &info) {
        info.exception_table_index = stream.get2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, type_annotation::offset_target &info) {
        info.offset = stream.get2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, type_annotation::type_argument_target &info) {
        info.offset = stream.get2();
        info.type_argument_index = stream.get1();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, type_annotation::path_t &info) {
        info.type_path_kind = stream.get1();
        info.type_argument_index = stream.get1();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, type_annotation::type_path &info) {
        info.path_length = stream.get1();
        info.path = new type_annotation::path_t[info.path_length];
        for (int i = 0; i < info.path_length; i++) {
            stream >> info.path[i];
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, type_annotation &info) {
        info.target_type = stream.get1();
        if (info.target_type == 0x00 || info.target_type == 0x01) {
            auto *result = new type_annotation::type_parameter_target;
            stream >> *result;
            info.target_info = result;
        } else if (info.target_type == 0x10) {
            auto *result = new type_annotation::supertype_target;
            stream >> *result;
            info.target_info = result;
        } else if (info.target_type == 0x11 || info.target_type == 0x12) {
            auto *result = new type_annotation::type_parameter_bound_target;
            stream >> *result;
            info.target_info = result;
        } else if (info.target_type == 0x13 || info.target_type == 0x14 || info.target_type == 0x15) {
            auto *result = new type_annotation::empty_target;
            stream >> *result;
            info.target_info = result;
        } else if (info.target_type == 0x16) {
            auto *result = new type_annotation::formal_parameter_target;
            stream >> *result;
            info.target_info = result;
        } else if (info.target_type == 0x17) {
            auto *result = new type_annotation::throws_target;
            stream >> *result;
            info.target_info = result;
        } else if (info.target_type == 0x40 || info.target_type == 0x41) {
            auto *result = new type_annotation::localvar_target;
            stream >> *result;
            info.target_info = result;
        } else if (info.target_type == 0x42) {
            auto *result = new type_annotation::catch_target;
            stream >> *result;
            info.target_info = result;
        } else if (info.target_type == 0x43 || info.target_type == 0x44 || info.target_type == 0x45 ||
                   info.target_type == 0x46) {
            auto *result = new type_annotation::offset_target;
            stream >> *result;
            info.target_info = result;
        } else if (info.target_type == 0x47 || info.target_type == 0x48 || info.target_type == 0x49 ||
                   info.target_type == 0x4A || info.target_type == 0x4B) {
            auto *result = new type_annotation::type_argument_target;
            stream >> *result;
            info.target_info = result;
        } else {
            assert(false);
        }
        stream >> info.target_path;
        info.annotations = new annotation;
        stream >> *info.annotations;
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, parameter_annotations_t &info) {
        info.num_annotations = stream.get2();
        info.annotations = new annotation[info.num_annotations];
        for (int i = 0; i < info.num_annotations; i++) {
            stream >> info.annotations[i];
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, RuntimeVisibleAnnotations_attribute &info) {
        stream >> *((attribute_info *) &info);
        stream >> info.parameter_annotations;
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, RuntimeInvisibleAnnotations_attribute &info) {
        stream >> *((attribute_info *) &info);
        stream >> info.parameter_annotations;
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, RuntimeVisibleParameterAnnotations_attribute &info) {
        stream >> *((attribute_info *) &info);
        info.num_parameters = stream.get1();
        info.parameter_annotations = new parameter_annotations_t[info.num_parameters];
        for (int i = 0; i < info.num_parameters; i++) {
            stream >> info.parameter_annotations[i];
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, RuntimeInvisibleParameterAnnotations_attribute &info) {
        stream >> *((attribute_info *) &info);
        info.num_parameters = stream.get1();
        info.parameter_annotations = new parameter_annotations_t[info.num_parameters];
        for (int i = 0; i < info.num_parameters; i++) {
            stream >> info.parameter_annotations[i];
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, RuntimeVisibleTypeAnnotations_attribute &info) {
        stream >> *((attribute_info *) &info);
        info.num_annotations = stream.get2();
        info.annotations = new type_annotation[info.num_annotations];
        for (int i = 0; i < info.num_annotations; i++) {
            stream >> info.annotations[i];
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, RuntimeInvisibleTypeAnnotations_attribute &info) {
        stream >> *((attribute_info *) &info);
        info.num_annotations = stream.get2();
        info.annotations = new type_annotation[info.num_annotations];
        for (int i = 0; i < info.num_annotations; i++) {
            stream >> info.annotations[i];
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, AnnotationDefault_attribute &info) {
        stream >> *((attribute_info *) &info);
        stream >> info.default_value;
        return stream;
    }

    array_value_t::~array_value_t() {
        delete[] values;
    }

    element_value::~element_value() {
        delete value;
    }

    annotation::~annotation() {
        delete[] element_value_pairs;
    }

    type_annotation::localvar_target::~localvar_target() {
        delete[] table;
    }

    type_annotation::type_path::~type_path() {
        delete[] path;
    }

    type_annotation::~type_annotation() {
        delete target_info;
        delete annotations;
    }

    parameter_annotations_t::~parameter_annotations_t() {
        delete[] annotations;
    }

    RuntimeVisibleParameterAnnotations_attribute::~RuntimeVisibleParameterAnnotations_attribute() {
        delete[] parameter_annotations;
    }

    RuntimeInvisibleParameterAnnotations_attribute::~RuntimeInvisibleParameterAnnotations_attribute() {
        delete[] parameter_annotations;
    }

    RuntimeVisibleTypeAnnotations_attribute::~RuntimeVisibleTypeAnnotations_attribute() {
        delete[] annotations;
    }

    RuntimeInvisibleTypeAnnotations_attribute::~RuntimeInvisibleTypeAnnotations_attribute() {
        delete[] annotations;
    }
}
