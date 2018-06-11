//
// Created by kiva on 2018/2/25.
//
#pragma once

#include <shared/hashMap.h>
#include <kivm/kivm.h>
#include <kivm/classfile/classFile.h>
#include <kivm/classpath/classLoader.h>
#include <kivm/oop/oopfwd.h>

namespace kivm {
    enum ClassType {
        INSTANCE_CLASS,
        OBJECT_ARRAY_CLASS,
        TYPE_ARRAY_CLASS,
    };

    enum ClassState {
        ALLOCATED,
        LOADED,
        LINKED,
        BEING_INITIALIZED,
        FULLY_INITIALIZED,
        INITIALIZATION_ERROR,
    };

    class Klass {
        friend class CopyingHeap;

    private:
        ClassState _state;
        u2 _accessFlag;

    protected:
        String _name;
        ClassType _type;

        mirrorOop _javaMirror;
        InstanceKlass *_superClass;

    public:
        mirrorOop getJavaMirror() {
            return _javaMirror;
        }

        void setJavaMirror(mirrorOop javaMirror) {
            this->_javaMirror = javaMirror;
        }

        ClassState getClassState() const {
            return _state;
        }

        void setClassState(ClassState classState) {
            this->_state = classState;
        }

        u2 getAccessFlag() const {
            return _accessFlag;
        }

        void setAccessFlag(u2 accessFlag) {
            Klass::_accessFlag = accessFlag;
        }

        const String &getName() const {
            return _name;
        }

        void setName(const String &name) {
            this->_name = name;
        }

        ClassType getClassType() const {
            return _type;
        }

        void setClassType(ClassType classType) {
            this->_type = classType;
        }

        InstanceKlass *getSuperClass() const {
            return _superClass;
        }

        void setSuperClass(InstanceKlass *superClass) {
            this->_superClass = superClass;
        }

        bool isPublic() const {
            return (getAccessFlag() & ACC_PUBLIC) == ACC_PUBLIC;
        }

        bool isPrivate() const {
            return (getAccessFlag() & ACC_PRIVATE) == ACC_PRIVATE;
        }

        bool isProtected() const {
            return (getAccessFlag() & ACC_PROTECTED) == ACC_PROTECTED;
        }

        bool isFinal() const {
            return (getAccessFlag() & ACC_FINAL) == ACC_FINAL;
        }

        bool isStatic() {
            return (getAccessFlag() & ACC_STATIC) == ACC_STATIC;
        }

        bool isAbstract() {
            return (getAccessFlag() & ACC_ABSTRACT) == ACC_ABSTRACT;
        }

        bool isInterface() {
            return (getAccessFlag() & ACC_INTERFACE) == ACC_INTERFACE;
        }

    public:
        Klass();

        virtual ~Klass() = default;

        virtual void linkClass() = 0;

        virtual void initClass() = 0;
    };
}

