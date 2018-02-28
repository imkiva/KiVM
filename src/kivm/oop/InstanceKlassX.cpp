//
// Created by kiva on 2018/2/27.
//

#include <kivm/oop/InstanceKlassX.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/method.h>
#include <kivm/field.h>
#include <sstream>

namespace kivm {
    InstanceKlass::InstanceKlass(ClassFile *classFile, ClassLoader *_class_loader) {
        this->set_type(ClassType::INSTANCE_CLASS);
        this->_class_file = classFile;
        this->_class_loader = _class_loader;
        this->_bm_attr = nullptr;
        this->_em_attr = nullptr;
        this->_ic_attr = nullptr;
    }

    InstanceKlass *InstanceKlass::require_instance_class(u2 class_info_index) {
        auto *class_info = require_constant<CONSTANT_Class_info>(_class_file->constant_pool,
                                                                 class_info_index);
        auto *utf8_info = require_constant<CONSTANT_Utf8_info>(_class_file->constant_pool,
                                                               class_info->name_index);

        Klass *loaded = ClassLoader::require_class(get_class_loader(), utf8_info->get_constant());
        if (loaded->get_type() != ClassType::INSTANCE_CLASS) {
            // TODO: throw VerifyError
            this->set_state(ClassState::INITIALIZATION_ERROR);
            assert(false);
            return nullptr;
        }
        return (InstanceKlass *) loaded;
    }

    void InstanceKlass::link_and_init() {
        cp_info **pool = _class_file->constant_pool;
        this->set_access_flag(_class_file->access_flags);

        auto *class_info = require_constant<CONSTANT_Class_info>(pool, _class_file->this_class);
        auto *utf8_info = require_constant<CONSTANT_Utf8_info>(pool, class_info->name_index);
        this->set_name(utf8_info->get_constant());

        link_super_class(pool);
        link_interfaces(pool);
        link_methods(pool);
        link_fields(pool);
        link_constant_pool(pool);
        link_attributes(pool);
        this->set_state(ClassState::LINKED);
        this->set_state(ClassState::BEING_INITIALIZED);
        this->set_state(ClassState::FULLY_INITIALIZED);
    }

    void InstanceKlass::link_super_class(cp_info **pool) {
        if (_class_file->super_class == 0) {
            // java.lang.Object
            if (get_name() != L"java/lang/Object") {
                // TODO: throw VerifyError
                this->set_state(ClassState::INITIALIZATION_ERROR);
                assert(false);
            }
            // java.lang.Object does not need a superclass.
            set_super_class(nullptr);
            return;
        }

        auto *super_class = require_instance_class(_class_file->super_class);
        if (super_class->is_final()) {
            // TODO: throw VerifyError
            this->set_state(ClassState::INITIALIZATION_ERROR);
            assert(false);
        }
        set_super_class(super_class);
    }

    void InstanceKlass::link_interfaces(cp_info **pool) {
        for (int i = 0; i < _class_file->interfaces_count; ++i) {
            InstanceKlass *interface_class = require_instance_class(_class_file->interfaces[i]);
            _interfaces.insert(std::make_pair(interface_class->get_name(), interface_class));
        }
    }

    void InstanceKlass::link_methods(cp_info **pool) {
        // for a easy implementation, I just copy superclass's vtable.
        if (get_super_class() != nullptr) {
            auto *sc = (InstanceKlass *) get_super_class();
            this->_vtable = sc->_vtable;
        }

        for (int i = 0; i < _class_file->methods_count; ++i) {
            auto *method = new Method(this, _class_file->methods + i);
            method->link_method(pool);
            MethodPool::add(method);

            using std::make_pair;

            if (method->is_final() || method->is_private()) {
                _pftable.insert(make_pair(Method::make_identity(method), method));

            } else if (method->is_static()) {
                _stable.insert(make_pair(Method::make_identity(method), method));

            } else {
                // Do not use _vtable.insert()
                // This may override superclass's virtual methods.
                _vtable[Method::make_identity(method)] = method;
            }
        }
    }

    static void init_field_data(std::vector<oop> &values, Field *field) {
        switch (field->get_value_type()) {
            case ValueType::INT:
                values.push_back(new intOopDesc(0));
                break;
            case ValueType::LONG:
                values.push_back(new longOopDesc(0L));
                break;
            case ValueType::FLOAT:
                values.push_back(new floatOopDesc(0.0f));
                break;
            case ValueType::DOUBLE:
                values.push_back(new doubleOopDesc(0.0));
                break;
            default:
                values.push_back(nullptr);
                break;
        }
    }

    void InstanceKlass::link_fields(cp_info **pool) {
        u2 static_field_index = 0;
        u2 instance_field_index = 0;
        for (int i = 0; i < _class_file->fields_count; ++i) {
            auto *field = new Field(this, _class_file->fields + i);
            field->link_field(pool);
            FieldPool::add(field);

            using std::make_pair;
            if (field->is_static()) {
                init_field_data(_static_field_values, field);
                _static_fields.insert(make_pair(Field::make_identity(field),
                                                make_pair(static_field_index++, field)));
            } else {
                init_field_data(_instance_field_init_values, field);
                _instance_fields.insert(make_pair(Field::make_identity(field),
                                                  make_pair(instance_field_index++, field)));
            }
        }
        this->_static_field_values.shrink_to_fit();
        this->_instance_field_init_values.shrink_to_fit();
    }

    void InstanceKlass::link_constant_pool(cp_info **constant_pool) {
        // TODO: Implement runtime constant pool
        // currently, we use cp_info as runtime constant pool
    }

    void InstanceKlass::link_attributes(cp_info **pool) {
        for (int i = 0; i < _class_file->attributes_count; ++i) {
            attribute_info *attr = _class_file->attributes[i];

            switch (AttributeParser::to_attribute_tag(attr->attribute_name_index, pool)) {
                case ATTRIBUTE_InnerClasses:
                    _ic_attr = (InnerClasses_attribute *) attr;
                    break;
                case ATTRIBUTE_EnclosingMethod:
                    _em_attr = (EnclosingMethod_attribute *) attr;
                    break;
                case ATTRIBUTE_Signature: {
                    auto *sig_attr = (Signature_attribute *) attr;
                    auto *utf8 = require_constant<CONSTANT_Utf8_info>(pool, sig_attr->signature_index);
                    _signature = utf8->get_constant();
                    break;
                }
                case ATTRIBUTE_SourceFile: {
                    auto *s = (SourceFile_attribute *) attr;
                    auto *utf8 = require_constant<CONSTANT_Utf8_info>(pool, s->sourcefile_index);
                    _source_file = utf8->get_constant();
                    break;
                }
                case ATTRIBUTE_BootstrapMethods:
                    _bm_attr = (BootstrapMethods_attribute *) attr;
                    break;
                case ATTRIBUTE_RuntimeVisibleAnnotations:
                case ATTRIBUTE_RuntimeVisibleTypeAnnotations:
                default: {
                    // TODO
                    break;
                }
            }
        }
    }
}
