//
// Created by kiva on 2018/2/26.
//

#include <kivm/classfile/attributeInfo.h>
#include <kivm/classfile/classFileStream.h>
#include <unordered_map>
#include <cassert>

namespace kivm {
    /**
     * Get next stack_map_frame
     * @param stream Class file stream
     * @return stack_map_frame
     */
    static StackMapTable_attribute::stack_map_frame *parse_stack_map_frame(ClassFileStream &stream);

    /**
     * Get next verification_type_info
     * @param stream Class stream
     * @return verification_type_info
     */
    static StackMapTable_attribute::verification_type_info *parse_verification_type(ClassFileStream &stream);


    /*******************************************************************
     * stream operators
     *******************************************************************/

    ClassFileStream &operator>>(ClassFileStream &stream, attribute_info &info) {
        info.attribute_name_index = stream.get_u2();
        info.attribute_length = stream.get_u4();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, StackMapTable_attribute::verification_type_info &info) {
        info.tag = stream.get_u1();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, StackMapTable_attribute::Top_variable_info &info) {
        info.tag = stream.get_u1();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, StackMapTable_attribute::Integer_variable_info &info) {
        info.tag = stream.get_u1();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, StackMapTable_attribute::Float_variable_info &info) {
        info.tag = stream.get_u1();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, StackMapTable_attribute::Double_variable_info &info) {
        info.tag = stream.get_u1();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, StackMapTable_attribute::Long_variable_info &info) {
        info.tag = stream.get_u1();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, StackMapTable_attribute::Null_variable_info &info) {
        info.tag = stream.get_u1();
        return stream;
    }

    ClassFileStream &
    operator>>(ClassFileStream &stream, StackMapTable_attribute::UninitializedThis_variable_info &info) {
        info.tag = stream.get_u1();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, StackMapTable_attribute::Object_variable_info &info) {
        info.tag = stream.get_u1();
        info.constant_pool_index = stream.get_u2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, StackMapTable_attribute::Uninitialized_variable_info &info) {
        info.tag = stream.get_u1();
        info.offset = stream.get_u2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, StackMapTable_attribute::stack_map_frame &info) {
        info.frame_type = stream.get_u1();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, StackMapTable_attribute::same_frame &info) {
        info.frame_type = stream.get_u1();
        return stream;
    }

    ClassFileStream &
    operator>>(ClassFileStream &stream, StackMapTable_attribute::same_locals_1_stack_item_frame &info) {
        info.frame_type = stream.get_u1();
        info.stack[0] = parse_verification_type(stream);
        return stream;
    }

    ClassFileStream &
    operator>>(ClassFileStream &stream, StackMapTable_attribute::same_locals_1_stack_item_frame_extended &info) {
        info.frame_type = stream.get_u1();
        info.offset_delta = stream.get_u2();
        info.stack[0] = parse_verification_type(stream);
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, StackMapTable_attribute::chop_frame &info) {
        info.frame_type = stream.get_u1();
        info.offset_delta = stream.get_u2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, StackMapTable_attribute::same_frame_extended &info) {
        info.frame_type = stream.get_u1();
        info.offset_delta = stream.get_u2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, StackMapTable_attribute::append_frame &info) {
        info.frame_type = stream.get_u1();
        info.offset_delta = stream.get_u2();
        if (info.frame_type - 251 != 0) {
            info.locals = new StackMapTable_attribute::verification_type_info *[info.frame_type - 251];
        }
        for (int i = 0; i < info.frame_type - 251; i++) {
            info.locals[i] = parse_verification_type(stream);
        }
        return stream;
    }

    StackMapTable_attribute::append_frame::~append_frame() {
        if (locals != nullptr) {
            for (int i = 0; i < frame_type - 251; i++) {
                delete locals[i];
            }
            delete[] locals;
        }
    }

    ClassFileStream &operator>>(ClassFileStream &stream, StackMapTable_attribute::full_frame &info) {
        info.frame_type = stream.get_u1();
        info.offset_delta = stream.get_u2();
        info.number_of_locals = stream.get_u2();
        info.locals = new StackMapTable_attribute::verification_type_info *[info.number_of_locals];
        for (int i = 0; i < info.number_of_locals; i++) {
            info.locals[i] = parse_verification_type(stream);
        }
        info.number_of_stack_items = stream.get_u2();
        info.stack = new StackMapTable_attribute::verification_type_info *[info.number_of_stack_items];
        for (int j = 0; j < info.number_of_stack_items; j++) {
            info.stack[j] = parse_verification_type(stream);
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, StackMapTable_attribute &attr) {
        stream >> *((attribute_info *) &attr);
        attr.number_of_entries = stream.get_u2();
        attr.entries = new StackMapTable_attribute::stack_map_frame *[attr.number_of_entries];
        for (int i = 0; i < attr.number_of_entries; i++) {
            attr.entries[i] = parse_stack_map_frame(stream);
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, ConstantValue_attribute &attr) {
        stream >> *((attribute_info *) &attr);
        attr.constant_index = stream.get_u2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, Exceptions_attribute &attr) {
        stream >> *((attribute_info *) &attr);
        attr.number_of_exceptions = stream.get_u2();
        attr.exception_index_table = new u2[attr.number_of_exceptions];
        for (int i = 0; i < attr.number_of_exceptions; i++) {
            attr.exception_index_table[i] = stream.get_u2();
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, classes_t &attr) {
        attr.inner_class_info_index = stream.get_u2();
        attr.outer_class_info_index = stream.get_u2();
        attr.inner_name_index = stream.get_u2();
        attr.inner_class_access_flags = stream.get_u2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, InnerClasses_attribute &attr) {
        stream >> *((attribute_info *) &attr);
        attr.number_of_classes = stream.get_u2();
        attr.classes = new classes_t[attr.number_of_classes];
        for (int i = 0; i < attr.number_of_classes; i++) {
            stream >> attr.classes[i];
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, EnclosingMethod_attribute &attr) {
        stream >> *((attribute_info *) &attr);
        attr.class_index = stream.get_u2();
        attr.method_index = stream.get_u2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, Synthetic_attribute &attr) {
        stream >> *((attribute_info *) &attr);
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, Signature_attribute &attr) {
        stream >> *((attribute_info *) &attr);
        attr.signature_index = stream.get_u2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, SourceFile_attribute &attr) {
        stream >> *((attribute_info *) &attr);
        attr.sourcefile_index = stream.get_u2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, SourceDebugExtension_attribute &attr) {
        stream >> *((attribute_info *) &attr);
        attr.debug_extension = new u1[attr.attribute_length];
        stream.get_u1_bytes(attr.debug_extension, attr.attribute_length);
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, line_number_table_t &attr) {
        attr.start_pc = stream.get_u2();
        attr.line_number = stream.get_u2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, LineNumberTable_attribute &attr) {
        stream >> *((attribute_info *) &attr);
        attr.line_number_table_length = stream.get_u2();
        attr.line_number_table = new line_number_table_t[attr.line_number_table_length];
        for (int i = 0; i < attr.line_number_table_length; i++) {
            stream >> attr.line_number_table[i];
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, local_variable_table_t &attr) {
        attr.start_pc = stream.get_u2();
        attr.length = stream.get_u2();
        attr.name_index = stream.get_u2();
        attr.descriptor_index = stream.get_u2();
        attr.index = stream.get_u2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, LocalVariableTable_attribute &attr) {
        stream >> *((attribute_info *) &attr);
        attr.local_variable_table_length = stream.get_u2();
        attr.local_variable_table = new local_variable_table_t[attr.local_variable_table_length];
        for (int i = 0; i < attr.local_variable_table_length; i++) {
            stream >> attr.local_variable_table[i];
        }
        return stream;
    }

    ClassFileStream &
    operator>>(ClassFileStream &stream, local_variable_type_table_t &attr) {
        attr.start_pc = stream.get_u2();
        attr.length = stream.get_u2();
        attr.name_index = stream.get_u2();
        attr.signature_index = stream.get_u2();
        attr.index = stream.get_u2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, LocalVariableTypeTable_attribute &attr) {
        stream >> *((attribute_info *) &attr);
        attr.local_variable_type_table_length = stream.get_u2();
        attr.local_variable_type_table =
                new local_variable_type_table_t[attr.local_variable_type_table_length];
        for (int i = 0; i < attr.local_variable_type_table_length; i++) {
            stream >> attr.local_variable_type_table[i];
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, Deprecated_attribute &attr) {
        stream >> *((attribute_info *) &attr);
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, bootstrap_methods_t &attr) {
        attr.bootstrap_method_ref = stream.get_u2();
        attr.num_bootstrap_arguments = stream.get_u2();
        attr.bootstrap_arguments = new u2[attr.num_bootstrap_arguments];
        for (int i = 0; i < attr.num_bootstrap_arguments; i++) {
            attr.bootstrap_arguments[i] = stream.get_u2();
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, BootstrapMethods_attribute &attr) {
        stream >> *((attribute_info *) &attr);
        attr.num_bootstrap_methods = stream.get_u2();
        attr.bootstrap_methods = new bootstrap_methods_t[attr.num_bootstrap_methods];
        for (int i = 0; i < attr.num_bootstrap_methods; i++) {
            stream >> attr.bootstrap_methods[i];
        }
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, parameters_t &attr) {
        attr.name_index = stream.get_u2();
        attr.access_flags = stream.get_u2();
        return stream;
    }

    ClassFileStream &operator>>(ClassFileStream &stream, MethodParameters_attribute &attr) {
        stream >> *((attribute_info *) &attr);
        attr.parameters_count = stream.get_u1();
        attr.parameters = new parameters_t[attr.parameters_count];
        for (int i = 0; i < attr.parameters_count; i++) {
            stream >> attr.parameters[i];
        }
        return stream;
    }

    /*******************************************************************
     * helpers
     *******************************************************************/

    /**
     * Get next verification_type_info
     * @param stream Class stream
     * @return verification_type_info
     */
    static StackMapTable_attribute::verification_type_info *parse_verification_type(ClassFileStream &stream) {
        switch (stream.peek_u1()) {
            case ITEM_Top: {
                auto *tvi = new StackMapTable_attribute::Top_variable_info;
                stream >> (*tvi);
                return tvi;
            }
            case ITEM_Integer: {
                auto *ivi = new StackMapTable_attribute::Integer_variable_info;
                stream >> (*ivi);
                return ivi;
            }
            case ITEM_Float: {
                auto *fvi = new StackMapTable_attribute::Float_variable_info;
                stream >> (*fvi);
                return fvi;
            }
            case ITEM_Double: {
                auto *dvi = new StackMapTable_attribute::Double_variable_info;
                stream >> (*dvi);
                return dvi;
            }
            case ITEM_Long: {
                auto *lvi = new StackMapTable_attribute::Long_variable_info;
                stream >> (*lvi);
                return lvi;
            }
            case ITEM_Null: {
                auto *nvi = new StackMapTable_attribute::Null_variable_info;
                stream >> (*nvi);
                return nvi;
            }
            case ITEM_UninitializedThis: {
                auto *utvi = new StackMapTable_attribute::UninitializedThis_variable_info;
                stream >> (*utvi);
                return utvi;
            }
            case ITEM_Object: {
                auto *ovi = new StackMapTable_attribute::Object_variable_info;
                stream >> (*ovi);
                return ovi;
            }
            case ITEM_Uninitialized: {
                auto *uvi = new StackMapTable_attribute::Uninitialized_variable_info;
                stream >> (*uvi);
                return uvi;
            }
            default:
                assert(false);
                return nullptr;
        }
    }

    /**
     * Get next stack_map_frame
     * @param stream Class file stream
     * @return stack_map_frame
     */
    static StackMapTable_attribute::stack_map_frame *parse_stack_map_frame(ClassFileStream &stream) {
        u1 frame_type = stream.peek_u1();
        if (frame_type >= 0 && frame_type <= 63) {
            auto *frame = new StackMapTable_attribute::same_frame;
            stream >> *frame;
            return frame;
        } else if (frame_type >= 64 && frame_type <= 127) {
            auto *frame = new StackMapTable_attribute::same_locals_1_stack_item_frame;
            stream >> *frame;
            return frame;
        } else if (frame_type == 247) {
            auto *frame = new StackMapTable_attribute::same_locals_1_stack_item_frame_extended;
            stream >> *frame;
            return frame;
        } else if (frame_type >= 248 && frame_type <= 250) {
            auto *frame = new StackMapTable_attribute::chop_frame;
            stream >> *frame;
            return frame;
        } else if (frame_type == 251) {
            auto *frame = new StackMapTable_attribute::same_frame_extended;
            stream >> *frame;
            return frame;
        } else if (frame_type >= 252 && frame_type <= 254) {
            auto *frame = new StackMapTable_attribute::append_frame;
            stream >> *frame;
            return frame;
        } else if (frame_type == 255) {
            auto *frame = new StackMapTable_attribute::full_frame;
            stream >> *frame;
            return frame;
        } else {
            assert(false);
            return nullptr;
        }
    }

    /*******************************************************************
     * attributes
     *******************************************************************/

    Code_attribute::Code_attribute() {
        code = nullptr;
        attributes = nullptr;
        exception_table = nullptr;
    }

    Code_attribute::~Code_attribute() {
        delete[] code;
        delete[] exception_table;
        AttributeParser::dealloc_attributes(&attributes, attributes_count);
    }

    void Code_attribute::init(ClassFileStream &stream, cp_info **constant_pool) {
        stream >> *((attribute_info *) this);
        max_stack = stream.get_u2();
        max_locals = stream.get_u2();
        code_length = stream.get_u4();
        code = new u1[code_length];
        stream.get_u1_bytes(code, code_length);
        exception_table_length = stream.get_u2();
        exception_table = new exception_table_t[exception_table_length];
        for (int i = 0; i < exception_table_length; ++i) {
            exception_table[i].start_pc = stream.get_u2();
            exception_table[i].end_pc = stream.get_u2();
            exception_table[i].handler_pc = stream.get_u2();
            exception_table[i].catch_type = stream.get_u2();
        }
        attributes_count = stream.get_u2();
        AttributeParser::read_attributes(&attributes, attributes_count,
                                         stream, constant_pool);
    }

    StackMapTable_attribute::append_frame::append_frame() {
        this->locals = nullptr;
    }

    StackMapTable_attribute::full_frame::full_frame() {
        this->locals = nullptr;
        this->stack = nullptr;
    }

    StackMapTable_attribute::StackMapTable_attribute() {
        this->entries = nullptr;
    }

    Exceptions_attribute::Exceptions_attribute() {
        this->exception_index_table = nullptr;
    }

    InnerClasses_attribute::InnerClasses_attribute() {
        this->classes = nullptr;
    }

    SourceDebugExtension_attribute::SourceDebugExtension_attribute() {
        this->debug_extension = nullptr;
    }

    LineNumberTable_attribute::LineNumberTable_attribute() {
        this->line_number_table = nullptr;
    }

    LocalVariableTable_attribute::LocalVariableTable_attribute() {
        this->local_variable_table = nullptr;
    }

    LocalVariableTypeTable_attribute::LocalVariableTypeTable_attribute() {
        this->local_variable_type_table = nullptr;
    }

    BootstrapMethods_attribute::BootstrapMethods_attribute() {
        this->bootstrap_methods = nullptr;
    }

    bootstrap_methods_t::bootstrap_methods_t() {
        this->bootstrap_arguments = nullptr;
    }

    MethodParameters_attribute::MethodParameters_attribute() {
        this->parameters = nullptr;
    }

    StackMapTable_attribute::same_locals_1_stack_item_frame::~same_locals_1_stack_item_frame() {
        delete stack[0];
    }

    StackMapTable_attribute::same_locals_1_stack_item_frame_extended::~same_locals_1_stack_item_frame_extended() {
        delete stack[0];
    }

    StackMapTable_attribute::full_frame::~full_frame() {
        if (locals != nullptr) {
            for (int i = 0; i < number_of_locals; i++) {
                delete locals[i];
            }
            delete[] locals;
        }
        if (stack != nullptr) {
            for (int i = 0; i < number_of_stack_items; i++) {
                delete stack[i];
            }
            delete[] stack;
        }
    }

    StackMapTable_attribute::~StackMapTable_attribute() {
        if (entries != nullptr) {
            for (int attr = 0; attr < number_of_entries; attr++) {
                delete entries[attr];
            }
            delete[] entries;
        }
    }

    Exceptions_attribute::~Exceptions_attribute() {
        delete[] exception_index_table;
    }

    InnerClasses_attribute::~InnerClasses_attribute() {
        delete[] classes;
    }

    SourceDebugExtension_attribute::~SourceDebugExtension_attribute() {
        delete[] debug_extension;
    }

    LineNumberTable_attribute::~LineNumberTable_attribute() {
        delete[] line_number_table;
    }

    LocalVariableTable_attribute::~LocalVariableTable_attribute() {
        delete[] local_variable_table;
    }

    LocalVariableTypeTable_attribute::~LocalVariableTypeTable_attribute() {
        delete[] local_variable_type_table;
    }

    bootstrap_methods_t::~bootstrap_methods_t() {
        delete[] bootstrap_arguments;
    }

    BootstrapMethods_attribute::~BootstrapMethods_attribute() {
        delete[] bootstrap_methods;
    }

    MethodParameters_attribute::~MethodParameters_attribute() {
        delete[] parameters;
    }

    /*******************************************************************
     * Attribute parser
     *******************************************************************/

    u2 AttributeParser::to_attribute_tag(u2 attribute_name_index, cp_info **constant_pool) {
        static std::unordered_map<String, u2> ATTRIBUTE_MAPPING{
                {L"ConstantValue",                        ATTRIBUTE_ConstantValue},
                {L"Code",                                 ATTRIBUTE_Code},
                {L"StackMapTable",                        ATTRIBUTE_StackMapTable},
                {L"Exceptions",                           ATTRIBUTE_Exceptions},
                {L"InnerClasses",                         ATTRIBUTE_InnerClasses},
                {L"EnclosingMethod",                      ATTRIBUTE_EnclosingMethod},
                {L"Synthetic",                            ATTRIBUTE_Synthetic},
                {L"Signature",                            ATTRIBUTE_Signature},
                {L"SourceFile",                           ATTRIBUTE_SourceFile},
                {L"SourceDebugExtension",                 ATTRIBUTE_SourceDebugExtension},
                {L"LineNumberTable",                      ATTRIBUTE_LineNumberTable},
                {L"LocalVariableTable",                   ATTRIBUTE_LocalVariableTable},
                {L"LocalVariableTypeTable",               ATTRIBUTE_LocalVariableTypeTable},
                {L"Deprecated",                           ATTRIBUTE_Deprecated},
                {L"RuntimeVisibleAnnotations",            ATTRIBUTE_RuntimeVisibleAnnotations},
                {L"RuntimeInvisibleAnnotations",          ATTRIBUTE_RuntimeInvisibleAnnotations},
                {L"RuntimeVisibleParameterAnnotations",   ATTRIBUTE_RuntimeVisibleParameterAnnotations},
                {L"RuntimeInvisibleParameterAnnotations", ATTRIBUTE_RuntimeInvisibleParameterAnnotations},
                {L"RuntimeVisibleTypeAnnotations",        ATTRIBUTE_RuntimeVisibleTypeAnnotations},
                {L"RuntimeInvisibleTypeAnnotations",      ATTRIBUTE_RuntimeInvisibleTypeAnnotations},
                {L"AnnotationDefault",                    ATTRIBUTE_AnnotationDefault},
                {L"BootstrapMethods",                     ATTRIBUTE_BootstrapMethods},
                {L"MethodParameters",                     ATTRIBUTE_MethodParameters},
        };

        cp_info *cp = constant_pool[attribute_name_index];
        if (cp->tag == CONSTANT_Utf8) {
            auto *utf8_info = (CONSTANT_Utf8_info *) cp;
            const String &name = utf8_info->get_constant();
            auto iter = ATTRIBUTE_MAPPING.find(name);
            if (iter != ATTRIBUTE_MAPPING.end()) {
                u2 tag = iter->second;
                return tag;
            }
        }
        return ATTRIBUTE_INVALID;
    }

    template<typename T>
    static T *read_attribute_entry(ClassFileStream &stream) {
        auto *result = new T;
        stream >> *result;
        return result;
    }

    attribute_info *AttributeParser::parse_attribute(ClassFileStream &stream,
                                                     cp_info **constant_pool) {
        u2 attribute_name_index = stream.peek_u2();
        u2 attribute_tag = to_attribute_tag(attribute_name_index, constant_pool);
        switch (attribute_tag) {
            case ATTRIBUTE_Code: {
                auto *result = new Code_attribute;
                result->init(stream, constant_pool);
                return result;
            }
            case ATTRIBUTE_ConstantValue:
                return read_attribute_entry<ConstantValue_attribute>(stream);

            case ATTRIBUTE_StackMapTable:
                return read_attribute_entry<StackMapTable_attribute>(stream);

            case ATTRIBUTE_Exceptions:
                return read_attribute_entry<Exceptions_attribute>(stream);

            case ATTRIBUTE_InnerClasses:
                return read_attribute_entry<InnerClasses_attribute>(stream);

            case ATTRIBUTE_EnclosingMethod:
                return read_attribute_entry<EnclosingMethod_attribute>(stream);

            case ATTRIBUTE_Synthetic:
                return read_attribute_entry<Synthetic_attribute>(stream);

            case ATTRIBUTE_Signature:
                return read_attribute_entry<Signature_attribute>(stream);

            case ATTRIBUTE_SourceFile:
                return read_attribute_entry<SourceFile_attribute>(stream);

            case ATTRIBUTE_SourceDebugExtension:
                return read_attribute_entry<SourceDebugExtension_attribute>(stream);

            case ATTRIBUTE_LineNumberTable:
                return read_attribute_entry<LineNumberTable_attribute>(stream);

            case ATTRIBUTE_LocalVariableTable:
                return read_attribute_entry<LocalVariableTable_attribute>(stream);

            case ATTRIBUTE_LocalVariableTypeTable:
                return read_attribute_entry<LocalVariableTypeTable_attribute>(stream);

            case ATTRIBUTE_Deprecated:
                return read_attribute_entry<Deprecated_attribute>(stream);

            case ATTRIBUTE_BootstrapMethods:
                return read_attribute_entry<BootstrapMethods_attribute>(stream);

            case ATTRIBUTE_MethodParameters:
                return read_attribute_entry<MethodParameters_attribute>(stream);

            case ATTRIBUTE_INVALID:
                fprintf(stderr, "Invalid attribute tag: %u\n", attribute_tag);
                exit(1);
                return nullptr;

            default:
                fprintf(stderr, "Unimplemented attribute: %u\n", attribute_tag);
                exit(1);
                return nullptr;

//            case ATTRIBUTE_RuntimeVisibleAnnotations:
//                return read_attribute_entry<RuntimeVisibleAnnotations_attribute>(stream);
//
//            case ATTRIBUTE_RuntimeInvisibleAnnotations:
//                return read_attribute_entry<RuntimeInvisibleAnnotations_attribute>(stream);
//
//            case ATTRIBUTE_RuntimeVisibleParameterAnnotations:
//                return read_attribute_entry<RuntimeVisibleParameterAnnotations_attribute>(stream);
//
//            case ATTRIBUTE_RuntimeInvisibleParameterAnnotations:
//                return read_attribute_entry<RuntimeInvisibleParameterAnnotations_attribute>(stream);
//
//            case ATTRIBUTE_RuntimeVisibleTypeAnnotations:
//                return read_attribute_entry<RuntimeVisibleTypeAnnotations_attribute>(stream);
//
//            case ATTRIBUTE_RuntimeInvisibleTypeAnnotations:
//                return read_attribute_entry<RuntimeInvisibleTypeAnnotations_attribute>(stream);
//
//            case ATTRIBUTE_AnnotationDefault:
//                return read_attribute_entry<AnnotationDefault_attribute>(stream);
        }
        return nullptr;
    }


    void AttributeParser::read_attributes(attribute_info ***p, u2 count,
                                          ClassFileStream &stream, cp_info **constant_pool) {
        auto **attributes = new attribute_info *[count];
        for (int j = 0; j < count; ++j) {
            attributes[j] = AttributeParser::parse_attribute(stream, constant_pool);
        }
        if (p != nullptr) {
            *p = attributes;
        } else {
            dealloc_attributes(&attributes, count);
        }
    }

    void AttributeParser::dealloc_attributes(attribute_info ***p, u2 count) {
        if (p == nullptr) {
            return;
        }
        auto **attributes = *p;
        for (int i = 0; i < count; i++) {
            delete attributes[i];
        }
        delete[] attributes;
        *p = nullptr;
    }
}
