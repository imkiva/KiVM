//
// Created by kiva on 2018/2/27.
//

#include <kivm/oop/instanceKlass.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/helper.h>
#include <kivm/method.h>
#include <kivm/field.h>
#include <sstream>

namespace kivm {
    InstanceKlass::InstanceKlass(ClassFile *class_file, ClassLoader *class_loader,
                                 mirrorOop java_loader, ClassType class_type)
            : _java_loader(java_loader),
            _class_file(class_file),
            _class_loader(class_loader),
            _n_instance_fields(0),
            _n_static_fields(0),
            _em_attr(nullptr),
            _bm_attr(nullptr),
            _ic_attr(nullptr) {
        this->set_type(class_type);
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

    void InstanceKlass::link_fields(cp_info **pool) {
        using std::make_pair;

        // for a easy implementation, I just copy superclass's instance fields layout.
        int instance_field_index = 0;

        // instance fields in superclass
        if (this->_super_class != nullptr) {
            auto *super = (InstanceKlass *) this->_super_class;
            for (auto &e : super->_instance_fields) {
                D("%s: Extended instance field: #%-d %s",
                  strings::to_std_string(get_name()).c_str(),
                  instance_field_index,
                  strings::to_std_string(e.first).c_str());
                this->_instance_fields.insert(
                        make_pair(e.first,
                                  FieldID(instance_field_index++, e.second._field)));
            }
        }

        // instance fields in interfaces
        for (auto &interface : this->_interfaces) {
            for (auto &field : interface.second->_instance_fields) {
                D("%s: Extended interface instance field: #%-d %s",
                  strings::to_std_string(get_name()).c_str(),
                  instance_field_index,
                  strings::to_std_string(field.first).c_str());

                this->_instance_fields.insert(
                        make_pair(field.first,
                                  FieldID(instance_field_index++, field.second._field)));
            }
        }

        // link our fields
        int static_field_index = 0;
        for (int i = 0; i < _class_file->fields_count; ++i) {
            auto *field = new Field(this, _class_file->fields + i);
            field->link_field(pool);
            FieldPool::add(field);

            if (field->is_static()) {
                // static final fields should be initialized with constant values in constant pool.
                // static non-final fields should be initialized with specified values.
                if (!field->is_final()) {
                    helper_init_field(_static_field_values, field);

                } else if (!helper_init_static_final_field(_static_field_values, pool, field)) {
                    // TODO: throw VerifyError: static final fields must be initialized.
                    assert(false);
                }

                D("%s: New static field: #%-d %s",
                  strings::to_std_string(get_name()).c_str(),
                  static_field_index,
                  strings::to_std_string(Field::make_identity(this, field)).c_str());

                _static_fields.insert(make_pair(Field::make_identity(this, field),
                                                FieldID(static_field_index++, field)));
            } else {
                D("%s: New instance field: #%-d %s",
                  strings::to_std_string(get_name()).c_str(),
                  instance_field_index,
                  strings::to_std_string(Field::make_identity(this, field)).c_str());

                _instance_fields.insert(make_pair(Field::make_identity(this, field),
                                                  FieldID(instance_field_index++, field)));
            }
        }
        this->_static_field_values.shrink_to_fit();
        this->_n_static_fields = static_field_index;
        this->_n_instance_fields = instance_field_index;
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

#define KEY_MAKER(CLASS, NAME, DESC) \
        ((CLASS) + L" " + (NAME) + L" " + (DESC))

#define ND_KEY_MAKER(NAME, DESC) \
        ((NAME) + L" " + (DESC))

#define RETURN_IF(ITER, COLLECTION, KEY, SUCCESS, FAIL) \
    const auto &ITER = (COLLECTION).find(KEY); \
    return (ITER) != (COLLECTION).end() ? (SUCCESS) : (FAIL);

    FieldID InstanceKlass::get_static_field_info(const String &className,
                                                 const String &name,
                                                 const String &descriptor) const {
        RETURN_IF(iter, this->_static_fields,
                  KEY_MAKER(className, name, descriptor),
                  iter->second,
                  FieldID(-1, nullptr));
    }

    FieldID InstanceKlass::get_instance_field_info(const String &className,
                                                   const String &name,
                                                   const String &descriptor) const {
        RETURN_IF(iter, this->_instance_fields,
                  KEY_MAKER(className, name, descriptor),
                  iter->second,
                  FieldID(-1, nullptr));
    }

    int InstanceKlass::get_static_field_offset(const String &className,
                                               const String &name,
                                               const String &descriptor) const {
        return this->get_static_field_info(className, name, descriptor)._offset;
    }

    int InstanceKlass::get_instance_field_offset(const String &className,
                                                 const String &name,
                                                 const String &descriptor) const {
        return this->get_instance_field_info(className, name, descriptor)._offset;
    }

    Method *InstanceKlass::find_virtual_method(const String &name, const String &descriptor) const {
        RETURN_IF(iter, this->_vtable,
                  ND_KEY_MAKER(name, descriptor),
                  iter->second, nullptr);
    }

    Method *InstanceKlass::find_non_virtual_method(const String &name, const String &descriptor) const {
        RETURN_IF(iter, this->_pftable,
                  ND_KEY_MAKER(name, descriptor),
                  iter->second, nullptr);
    }

    Method *InstanceKlass::find_static_method(const String &name, const String &descriptor) const {
        RETURN_IF(iter, this->_stable,
                  ND_KEY_MAKER(name, descriptor),
                  iter->second, nullptr);
    }

    InstanceKlass *InstanceKlass::find_interface(const String &interface_class_name) const {
        RETURN_IF(iter, this->_interfaces,
                  interface_class_name,
                  iter->second, nullptr);
    }

    void InstanceKlass::set_static_field_value(const String &className,
                                               const String &name,
                                               const String &descriptor,
                                               oop value) {
        const auto &fieldID = get_static_field_info(className, name, descriptor);
        set_static_field_value(fieldID, value);
    }

    void InstanceKlass::set_static_field_value(const FieldID &fieldID, oop value) {
        if (fieldID._field == nullptr) {
            // throw java.lang.NoSuchFieldError
            assert(!"java.lang.NoSuchFieldError");
        }
        if (fieldID._field->is_final()) {
            // throw java.lang.IllegalAccessError
            assert(!"java.lang.IllegalAccessError");
        }

        D("Set field %s::%s(%s) to %p\n",
          strings::to_std_string(fieldID._field->get_class()->get_name()).c_str(),
          strings::to_std_string(fieldID._field->get_name()).c_str(),
          strings::to_std_string(fieldID._field->get_descriptor()).c_str(),
          value);
        this->_static_field_values[fieldID._offset] = value;
    }

    bool InstanceKlass::get_static_field_value(const String &className,
                                               const String &name,
                                               const String &descriptor,
                                               oop *result) {
        const auto &fieldID = get_static_field_info(className, name, descriptor);
        return get_static_field_value(fieldID, result);
    }

    bool InstanceKlass::get_static_field_value(const FieldID &fieldID, oop *result) {
        if (fieldID._field == nullptr) {
            return false;
        }

        *result = this->_static_field_values[fieldID._offset];
        return true;
    }

    void InstanceKlass::set_instance_field_value(instanceOop receiver,
                                                 const String &className,
                                                 const String &name,
                                                 const String &descriptor,
                                                 oop value) {
        const auto &fieldID = get_instance_field_info(className, name, descriptor);
        set_instance_field_value(receiver, fieldID, value);
    }

    void InstanceKlass::set_instance_field_value(instanceOop receiver, const FieldID &fieldID, oop value) {
        if (fieldID._field == nullptr) {
            // throw java.lang.NoSuchFieldError
            assert(!"java.lang.NoSuchFieldError");
        }
        if (fieldID._field->is_final()) {
            // throw java.lang.IllegalAccessError
            assert(!"java.lang.IllegalAccessError");
        }

        D("Set field %s::%s(%s) to %p\n",
          strings::to_std_string(fieldID._field->get_class()->get_name()).c_str(),
          strings::to_std_string(fieldID._field->get_name()).c_str(),
          strings::to_std_string(fieldID._field->get_descriptor()).c_str(),
          value);
        receiver->_instance_field_values[fieldID._offset] = value;
    }

    bool InstanceKlass::get_instance_field_value(instanceOop receiver, const String &className,
                                                 const String &name,
                                                 const String &descriptor,
                                                 oop *result) {
        const auto &fieldID = get_instance_field_info(className, name, descriptor);
        return get_instance_field_value(receiver, fieldID, result);
    }

    bool InstanceKlass::get_instance_field_value(instanceOop receiver, const FieldID &fieldID,
                                                 oop *result) {
        if (fieldID._field == nullptr) {
            return false;
        }

        *result = receiver->_instance_field_values[fieldID._offset];
        return true;
    }
}
