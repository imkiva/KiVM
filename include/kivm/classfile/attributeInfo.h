//
// Created by kiva on 2018/2/26.
//

#pragma once

#include <kivm/kivm.h>
#include <kivm/classfile/constantPool.h>

#define ITEM_Top                0
#define ITEM_Integer            1
#define ITEM_Float              2
#define ITEM_Double             3
#define ITEM_Long               4
#define ITEM_Null               5
#define ITEM_UninitializedThis  6
#define ITEM_Object             7
#define ITEM_Uninitialized      8

#define ATTRIBUTE_ConstantValue 0
#define ATTRIBUTE_Code 1
#define ATTRIBUTE_StackMapTable 2
#define ATTRIBUTE_Exceptions 3
#define ATTRIBUTE_InnerClasses 4
#define ATTRIBUTE_EnclosingMethod 5
#define ATTRIBUTE_Synthetic 6
#define ATTRIBUTE_Signature 7
#define ATTRIBUTE_SourceFile 8
#define ATTRIBUTE_SourceDebugExtension 9
#define ATTRIBUTE_LineNumberTable 10
#define ATTRIBUTE_LocalVariableTable 11
#define ATTRIBUTE_LocalVariableTypeTable 12
#define ATTRIBUTE_Deprecated 13
#define ATTRIBUTE_RuntimeVisibleAnnotations 14
#define ATTRIBUTE_RuntimeInvisibleAnnotations 15
#define ATTRIBUTE_RuntimeVisibleParameterAnnotations 16
#define ATTRIBUTE_RuntimeInvisibleParameterAnnotations 17
#define ATTRIBUTE_RuntimeVisibleTypeAnnotations 18
#define ATTRIBUTE_RuntimeInvisibleTypeAnnotations 19
#define ATTRIBUTE_AnnotationDefault 20
#define ATTRIBUTE_BootstrapMethods 21
#define ATTRIBUTE_MethodParameters 22

#define ATTRIBUTE_INVALID 99

namespace kivm {
    class ClassFileStream;

    /**
     * Attribute info
     */
    struct attribute_info {
        /**
         * The value of the {@code attribute_name_index} must be
         * a valid unsigned 16-bit index into the {@code constant_pool} of the class.
         * The {@code constant_pool} entry at attribute_name_index must be
         * a {@code CONSTANT_Utf8_info} structure (§4.4.7)
         * representing the name of the attribute.
         */
        u2 attribute_name_index;

        /**
         * The value of the {@code attribute_length} item indicates
         * the length of the subsequent information in bytes.
         * The length does not include the initial six bytes
         * that contain the {@code attribute_name_index}
         * and {@code attribute_length} items.
         */
        u4 attribute_length;

        attribute_info() = default;

        virtual ~attribute_info() = default;
    };

    struct ConstantValue_attribute : public attribute_info {
        u2 constant_index;
    };

    struct exception_table_t {
        u2 start_pc;
        u2 end_pc;
        u2 handler_pc;
        u2 catch_type;
    };

    struct Code_attribute : public attribute_info {
        u2 max_stack;
        u2 max_locals;

        u4 code_length;
        u1 *code;

        u2 exception_table_length;
        exception_table_t *exception_table;

        u2 attributes_count;
        attribute_info **attributes;

        Code_attribute();

        ~Code_attribute() override;

        void init(ClassFileStream &stream, cp_info **constant_pool);
    };

    struct StackMapTable_attribute : public attribute_info {
        struct verification_type_info {
            u1 tag;

            virtual ~verification_type_info() = default;
        };

        struct Top_variable_info : public verification_type_info {
        };

        struct Integer_variable_info : public verification_type_info {
        };

        struct Float_variable_info : public verification_type_info {
        };

        struct Double_variable_info : public verification_type_info {
        };

        struct Long_variable_info : public verification_type_info {
        };

        struct Null_variable_info : public verification_type_info {
        };

        struct UninitializedThis_variable_info : public verification_type_info {
        };

        struct Object_variable_info : public verification_type_info {
            u2 constant_pool_index;
        };

        struct Uninitialized_variable_info : public verification_type_info {
            u2 offset;
        };

        // stack_map_frame
        struct stack_map_frame {
            u1 frame_type;

            stack_map_frame() = default;

            virtual ~stack_map_frame() = default;
        };

        /**
         * Frame type: 0 - 63
         */
        struct same_frame : public stack_map_frame {
        };

        /**
         * Frame type: 64 - 127
         */
        struct same_locals_1_stack_item_frame : public stack_map_frame {
            verification_type_info *stack[1];

            ~same_locals_1_stack_item_frame() override;
        };

        /**
         * Frame type: 247
         */
        struct same_locals_1_stack_item_frame_extended : public stack_map_frame {
            u2 offset_delta;
            verification_type_info *stack[1];

            ~same_locals_1_stack_item_frame_extended() override;
        };

        /**
         * Frame type: 248 - 250
         */
        struct chop_frame : public stack_map_frame {
            u2 offset_delta;
        };

        /**
         * Frame type: 251
         */
        struct same_frame_extended : public stack_map_frame {
            u2 offset_delta;

        };

        /**
         * Frame type: 252-254
         */
        struct append_frame : public stack_map_frame {
            u2 offset_delta;
            /**
             * Length: frame_type - 251
             */
            verification_type_info **locals;

            append_frame();

            ~append_frame() override;
        };

        /**
         * Frame type: 255
         */
        struct full_frame : public stack_map_frame {
            u2 offset_delta;
            u2 number_of_locals;
            verification_type_info **locals;
            u2 number_of_stack_items;
            verification_type_info **stack;

            full_frame();

            ~full_frame() override;
        };

        u2 number_of_entries;

        stack_map_frame **entries;

        StackMapTable_attribute();

        ~StackMapTable_attribute() override;
    };

    struct Exceptions_attribute : public attribute_info {
        u2 number_of_exceptions;

        /**
         * CONSTANT_Class_info
         */
        u2 *exception_index_table;

        Exceptions_attribute();

        ~Exceptions_attribute() override;
    };

    struct classes_t {
        u2 inner_class_info_index;
        u2 outer_class_info_index;
        u2 inner_name_index;
        u2 inner_class_access_flags;
    };

    struct InnerClasses_attribute : public attribute_info {
        u2 number_of_classes;

        classes_t *classes;

        InnerClasses_attribute();

        ~InnerClasses_attribute() override;
    };

    struct EnclosingMethod_attribute : public attribute_info {
        u2 class_index;
        u2 method_index;
    };

    struct Synthetic_attribute : public attribute_info {
    };

    struct Signature_attribute : public attribute_info {
        u2 signature_index;
    };

    struct SourceFile_attribute : public attribute_info {
        u2 sourcefile_index;
    };

    struct SourceDebugExtension_attribute : public attribute_info {
        /**
         * Length: attribute_length
         */
        u1 *debug_extension;

        SourceDebugExtension_attribute();

        ~SourceDebugExtension_attribute() override;
    };

    struct line_number_table_t {
        u2 start_pc;
        u2 line_number;
    };

    struct LineNumberTable_attribute : public attribute_info {
        u2 line_number_table_length;

        line_number_table_t *line_number_table;

        LineNumberTable_attribute();

        ~LineNumberTable_attribute() override;
    };

    struct local_variable_table_t {
        u2 start_pc;
        u2 length;
        u2 name_index;
        u2 descriptor_index;
        u2 index;
    };

    struct LocalVariableTable_attribute : public attribute_info {
        u2 local_variable_table_length;

        local_variable_table_t *local_variable_table;

        LocalVariableTable_attribute();

        ~LocalVariableTable_attribute() override;
    };

    struct local_variable_type_table_t {
        u2 start_pc;
        u2 length;
        u2 name_index;
        u2 signature_index;
        u2 index;
    };

    struct LocalVariableTypeTable_attribute : public attribute_info {
        u2 local_variable_type_table_length;

        local_variable_type_table_t *local_variable_type_table;

        LocalVariableTypeTable_attribute();

        ~LocalVariableTypeTable_attribute() override;
    };

    struct Deprecated_attribute : public attribute_info {
    };

    struct bootstrap_methods_t {
        u2 bootstrap_method_ref;
        u2 num_bootstrap_arguments;
        u2 *bootstrap_arguments;

        bootstrap_methods_t();

        ~bootstrap_methods_t();
    };

    struct BootstrapMethods_attribute : public attribute_info {
        u2 num_bootstrap_methods;

        bootstrap_methods_t *bootstrap_methods;

        BootstrapMethods_attribute();

        ~BootstrapMethods_attribute() override;
    };

    struct parameters_t {
        u2 name_index;
        u2 access_flags;
    };

    struct MethodParameters_attribute : public attribute_info {
        u1 parameters_count;

        parameters_t *parameters;

        MethodParameters_attribute();

        ~MethodParameters_attribute() override;
    };

    class AttributeParser {
    public:
        static void read_attributes(attribute_info ***p, u2 count,
                                    ClassFileStream &stream, cp_info **constant_pool);

        static void dealloc_attributes(attribute_info ***p, u2 count);

        static u2 to_attribute_tag(u2 attribute_name_index, cp_info **constant_pool);

        static attribute_info *parse_attribute(ClassFileStream &stream, cp_info **constant_pool);
    };
}
