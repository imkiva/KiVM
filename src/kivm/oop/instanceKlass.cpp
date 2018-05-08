//
// Created by kiva on 2018/2/27.
//

#include <kivm/oop/instanceKlass.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/helper.h>
#include <kivm/oop/method.h>
#include <kivm/oop/field.h>
#include <sstream>
#include <kivm/native/java_lang_Class.h>

namespace kivm {
    InstanceKlass::InstanceKlass(ClassFile *classFile, ClassLoader *classLoader,
                                 mirrorOop javaLoader, ClassType classType)
        : _runtimePool(this),
          _javaLoader(javaLoader),
          _classFile(classFile),
          _classLoader(classLoader),
          _nInstanceFields(0),
          _nStaticFields(0),
          _enclosingMethodAttr(nullptr),
          _bootstrapMethodAttr(nullptr),
          _innerClassAttr(nullptr) {
        this->setClassType(classType);
        this->setJavaMirror(nullptr);
    }

    InstanceKlass *InstanceKlass::requireInstanceClass(u2 classInfoIndex) {
        auto *class_info = requireConstant<CONSTANT_Class_info>(_classFile->constant_pool,
                                                                classInfoIndex);
        auto *utf8_info = requireConstant<CONSTANT_Utf8_info>(_classFile->constant_pool,
                                                              class_info->name_index);

        Klass *loaded = ClassLoader::requireClass(getClassLoader(), utf8_info->getConstant());
        if (loaded->getClassType() != ClassType::INSTANCE_CLASS) {
            // TODO: throw VerifyError
            this->setClassState(ClassState::INITIALIZATION_ERROR);
            assert(false);
            return nullptr;
        }
        return (InstanceKlass *) loaded;
    }

    void InstanceKlass::initClass() {
        D("%s: initializing static fields",
          strings::toStdString(getName()).c_str());

        for (auto &e : this->_staticFields) {
            auto field = e.second->_field;
            // static final fields should be initialized with constant values in constant pool.
            // static non-final fields should be initialized with specified values.
            if (field->isFinal()) {
                if (!helperInitConstantField(_staticFieldValues, e.second->_offset,
                                             _classFile->constant_pool, field)) {
                    // TODO: throw VerifyError
                    PANIC("java.lang.VerifyError: static final fields must be initialized");
                }
            } else {
                helperInitField(_staticFieldValues, e.second->_offset, field);
            }
        }
        D("%s: class inited",
          strings::toStdString(getName()).c_str());
    }

    void InstanceKlass::linkClass() {
        cp_info **pool = _classFile->constant_pool;
        this->setAccessFlag(_classFile->access_flags);

        auto *class_info = requireConstant<CONSTANT_Class_info>(pool, _classFile->this_class);
        auto *utf8_info = requireConstant<CONSTANT_Utf8_info>(pool, class_info->name_index);
        this->setName(utf8_info->getConstant());

        // Must call this once class name is linked
        java::lang::Class::createMirror(this, _javaLoader);

        linkSuperClass(pool);
        linkFields(pool);

        linkInterfaces(pool);
        linkMethods(pool);

        linkConstantPool(pool);
        linkAttributes(pool);

        this->setClassState(ClassState::LINKED);
        D("%s: class linked",
          strings::toStdString(getName()).c_str());
    }

    void InstanceKlass::linkSuperClass(cp_info **pool) {
        if (_classFile->super_class == 0) {
            // java.lang.Object
            if (getName() != L"java/lang/Object") {
                // TODO: throw VerifyError
                this->setClassState(ClassState::INITIALIZATION_ERROR);
                assert(false);
            }
            // java.lang.Object does not need a superclass.
            setSuperClass(nullptr);
            return;
        }

        auto *super_class = requireInstanceClass(_classFile->super_class);
        if (super_class->isFinal()) {
            // TODO: throw VerifyError
            this->setClassState(ClassState::INITIALIZATION_ERROR);
            assert(false);
        }
        setSuperClass(super_class);
    }

    void InstanceKlass::linkInterfaces(cp_info **pool) {
        for (int i = 0; i < _classFile->interfaces_count; ++i) {
            InstanceKlass *interface_class = requireInstanceClass(_classFile->interfaces[i]);
            _interfaces.insert(std::make_pair(interface_class->getName(), interface_class));
        }
    }

    void InstanceKlass::linkMethods(cp_info **pool) {
        using std::make_pair;

        // for a easy implementation, I just copy superclass's vtable.
        if (getSuperClass() != nullptr) {
            auto *sc = getSuperClass();
            this->_vtable = sc->_vtable;
        }

        for (int i = 0; i < _classFile->methods_count; ++i) {
            auto *method = new Method(this, _classFile->methods + i);
            method->linkMethod(pool);
            MethodPool::add(method);

            const auto &id = Method::makeIdentity(method);
            const auto &pair = make_pair(id, method);
            _allMethods.insert(pair);

            if (method->isStatic()) {
                _stable.insert(pair);

            } else if (method->isFinal() || method->isPrivate()) {
                _pftable.insert(pair);

            } else {
                // Do not use _vtable.insert()
                // This may override superclass's virtual methods.
                _vtable[id] = method;
            }
        }
    }

    void InstanceKlass::linkFields(cp_info **pool) {
        using std::make_pair;

        // for a easy implementation, I just copy superclass's instance fields layout.
        int instanceFieldIndex = 0;

        // instance fields in superclass
        if (this->_superClass != nullptr) {
            auto *super = this->_superClass;
            this->_instanceFields = super->_instanceFields;
            instanceFieldIndex += (int) this->_instanceFields.size();
        }

        D("%s: Extended instance field count: %d",
          strings::toStdString(getName()).c_str(),
          instanceFieldIndex);

        // link our fields
        int staticFieldIndex = 0;
        for (int i = 0; i < _classFile->fields_count; ++i) {
            auto *field = new Field(this, _classFile->fields + i);
            field->linkField(pool);
            FieldPool::add(field);

            bool isStatic = field->isStatic();
            D("%s: New %s field (final: %s): #%-d %s",
              strings::toStdString(getName()).c_str(),
              isStatic ? "static" : "instance",
              field->isFinal() ? "true" : "false",
              isStatic ? staticFieldIndex : instanceFieldIndex,
              strings::toStdString(Field::makeIdentity(this, field)).c_str());

            if (isStatic) {
                _staticFields.insert(make_pair(Field::makeIdentity(this, field),
                                               new FieldID(staticFieldIndex++, field)));
            } else {
                _instanceFields.insert(make_pair(Field::makeIdentity(this, field),
                                                 new FieldID(instanceFieldIndex++, field)));
            }
        }

        this->_nStaticFields = staticFieldIndex;
        this->_nInstanceFields = instanceFieldIndex;

        // We need to allocate memory
        // because before initClass(), there might be field access
        this->_staticFieldValues.resize(this->_staticFields.size(), nullptr);
    }

    void InstanceKlass::linkConstantPool(cp_info **pool) {
        getRuntimeConstantPool()->attachConstantPool(pool);
    }

    void InstanceKlass::linkAttributes(cp_info **pool) {
        for (int i = 0; i < _classFile->attributes_count; ++i) {
            attribute_info *attr = _classFile->attributes[i];

            switch (AttributeParser::toAttributeTag(attr->attribute_name_index, pool)) {
                case ATTRIBUTE_InnerClasses:
                    _innerClassAttr = (InnerClasses_attribute *) attr;
                    break;
                case ATTRIBUTE_EnclosingMethod:
                    _enclosingMethodAttr = (EnclosingMethod_attribute *) attr;
                    break;
                case ATTRIBUTE_Signature: {
                    auto *sig_attr = (Signature_attribute *) attr;
                    auto *utf8 = requireConstant<CONSTANT_Utf8_info>(pool, sig_attr->signature_index);
                    _signature = utf8->getConstant();
                    break;
                }
                case ATTRIBUTE_SourceFile: {
                    auto *s = (SourceFile_attribute *) attr;
                    auto *utf8 = requireConstant<CONSTANT_Utf8_info>(pool, s->sourcefile_index);
                    _sourceFile = utf8->getConstant();
                    break;
                }
                case ATTRIBUTE_BootstrapMethods:
                    _bootstrapMethodAttr = (BootstrapMethods_attribute *) attr;
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

    int InstanceKlass::getStaticFieldOffset(const String &className,
                                            const String &name,
                                            const String &descriptor) const {
        return this->getStaticFieldInfo(className, name, descriptor)->_offset;
    }

    FieldID *InstanceKlass::getThisClassField(const String &name, const String &descriptor) const {
        auto id = getInstanceFieldInfo(getName(), name, descriptor);
        if (id != nullptr && id->_field != nullptr) {
            return id;
        }

        return getStaticFieldInfo(getName(), name, descriptor);
    }

    FieldID *InstanceKlass::getStaticFieldInfo(const String &className,
                                               const String &name,
                                               const String &descriptor) const {
        RETURN_IF(iter, this->_staticFields,
                  KEY_MAKER(className, name, descriptor),
                  iter->second,
                  nullptr);
    }

    FieldID *InstanceKlass::getInstanceFieldInfo(const String &className,
                                                 const String &name,
                                                 const String &descriptor) const {
        RETURN_IF(iter, this->_instanceFields,
                          KEY_MAKER(className, name, descriptor),
                          iter->second,
                          nullptr);
    }

    Method *InstanceKlass::getThisClassMethod(const String &name, const String &descriptor) const {
        RETURN_IF(iter, this->_allMethods,
                  ND_KEY_MAKER(name, descriptor),
                  iter->second, nullptr);
    }

    Method *InstanceKlass::getVirtualMethod(const String &name, const String &descriptor) const {
        RETURN_IF(iter, this->_vtable,
                  ND_KEY_MAKER(name, descriptor),
                  iter->second, nullptr);
    }

    Method *InstanceKlass::getNonVirtualMethod(const String &name, const String &descriptor) const {
        RETURN_IF(iter, this->_pftable,
                  ND_KEY_MAKER(name, descriptor),
                  iter->second, nullptr);
    }

    Method *InstanceKlass::getStaticMethod(const String &name, const String &descriptor) const {
        RETURN_IF(iter, this->_stable,
                  ND_KEY_MAKER(name, descriptor),
                  iter->second, nullptr);
    }

    InstanceKlass *InstanceKlass::getInterface(const String &interfaceClassName) const {
        RETURN_IF(iter, this->_interfaces,
                  interfaceClassName,
                  iter->second, nullptr);
    }

    void InstanceKlass::setStaticFieldValue(const String &className,
                                            const String &name,
                                            const String &descriptor,
                                            oop value) {
        auto fieldID = getStaticFieldInfo(className, name, descriptor);
        setStaticFieldValue(fieldID, value);
    }

    void InstanceKlass::setStaticFieldValue(FieldID *fieldID, oop value) {
        if (fieldID == nullptr || fieldID->_field == nullptr) {
            // TODO: throw java.lang.NoSuchFieldError
            PANIC("java.lang.NoSuchFieldError");
        }

        D("Set field %s::%s(%s) (slot: %d, max: %zd) to %p in %s",
          strings::toStdString(fieldID->_field->getClass()->getName()).c_str(),
          strings::toStdString(fieldID->_field->getName()).c_str(),
          strings::toStdString(fieldID->_field->getDescriptor()).c_str(),
          fieldID->_offset,
          this->_staticFieldValues.size(),
          value,
          strings::toStdString(this->getName()).c_str());
        this->_staticFieldValues[fieldID->_offset] = value;
    }

    bool InstanceKlass::getStaticFieldValue(const String &className,
                                            const String &name,
                                            const String &descriptor,
                                            oop *result) {
        auto fieldID = getStaticFieldInfo(className, name, descriptor);
        return getStaticFieldValue(fieldID, result);
    }

    bool InstanceKlass::getStaticFieldValue(FieldID *fieldID, oop *result) {
        if (fieldID == nullptr || fieldID->_field == nullptr) {
            return false;
        }

        *result = this->_staticFieldValues[fieldID->_offset];
        return true;
    }

    void InstanceKlass::setInstanceFieldValue(instanceOop receiver,
                                              const String &className,
                                              const String &name,
                                              const String &descriptor,
                                              oop value) {
        const auto &fieldID = getInstanceFieldInfo(className, name, descriptor);
        setInstanceFieldValue(receiver, fieldID, value);
    }

    void InstanceKlass::setInstanceFieldValue(instanceOop receiver, FieldID *fieldID, oop value) {
        if (fieldID == nullptr || fieldID->_field == nullptr) {
            // throw java.lang.NoSuchFieldError
            PANIC("java.lang.NoSuchFieldError");
        }

        D("Set field %s::%s(%s) to %p",
          strings::toStdString(fieldID->_field->getClass()->getName()).c_str(),
          strings::toStdString(fieldID->_field->getName()).c_str(),
          strings::toStdString(fieldID->_field->getDescriptor()).c_str(),
          value);
        receiver->_instanceFieldValues[fieldID->_offset] = value;
    }

    bool InstanceKlass::getInstanceFieldValue(instanceOop receiver, const String &className,
                                              const String &name,
                                              const String &descriptor,
                                              oop *result) {
        auto fieldID = getInstanceFieldInfo(className, name, descriptor);
        return getInstanceFieldValue(receiver, fieldID, result);
    }

    bool InstanceKlass::getInstanceFieldValue(instanceOop receiver, FieldID *fieldID,
                                              oop *result) {
        if (fieldID == nullptr || fieldID->_field == nullptr) {
            return false;
        }

        *result = receiver->_instanceFieldValues[fieldID->_offset];
        return true;
    }

    instanceOop InstanceKlass::newInstance() {
        return new instanceOopDesc(this);
    }

    bool InstanceKlass::checkInterface(InstanceKlass *interfaceClass) {
        for (const auto &e : this->_interfaces) {
            if (e.second == interfaceClass) {
                return true;
            }

            // check interfaces' interfaces
            if (e.second->checkInterface(interfaceClass)) {
                return true;
            }
        }

        // check parent's interfaces
        InstanceKlass *superClass = getSuperClass();
        if (superClass != nullptr) {
            return superClass->checkInterface(interfaceClass);
        }
        return false;
    }
}
