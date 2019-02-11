//
// Created by kiva on 2018/5/1.
//
#include <kivm/native/classNames.h>
#include <kivm/oop/reflection.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/oop/arrayKlass.h>
#include <kivm/oop/arrayOop.h>
#include <kivm/native/java_lang_Class.h>
#include <kivm/native/java_lang_reflect_Constructor.h>
#include <kivm/native/java_lang_reflect_Method.h>

namespace kivm {
    instanceOop newJavaFieldObject(FieldID *fieldID) {
        static auto fieldClass = (InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"java/lang/reflect/Field");

        static auto classField = fieldClass->getInstanceFieldInfo(J_FIELD,
            L"clazz", L"Ljava/lang/Class;");
        static auto typeField = fieldClass->getInstanceFieldInfo(J_FIELD,
            L"type", L"Ljava/lang/Class;");
        static auto slotField = fieldClass->getInstanceFieldInfo(J_FIELD,
            L"slot", L"I");
        static auto nameField = fieldClass->getInstanceFieldInfo(J_FIELD,
            L"name", L"Ljava/lang/String;");
        static auto signatureField = fieldClass->getInstanceFieldInfo(J_FIELD,
            L"signature", L"Ljava/lang/String;");
        static auto modifiersField = fieldClass->getInstanceFieldInfo(J_FIELD,
            L"modifiers", L"I");
        static auto overrideField = fieldClass->getInstanceFieldInfo(J_ACCESSIBLE_OBJECT,
            L"override", L"Z");

        auto fieldOop = fieldClass->newInstance();
        auto fieldInfo = fieldID->_field;

        auto fieldType = fieldInfo->getValueTypeClass();
        mirrorOop fieldTypeMirror = nullptr;
        if (fieldType == nullptr) {
            fieldTypeMirror = java::lang::Class::findPrimitiveTypeMirror(fieldInfo->getDescriptor());
        } else {
            fieldTypeMirror = fieldType->getJavaMirror();
        }

        fieldOop->setFieldValue(modifiersField, new intOopDesc(fieldInfo->getAccessFlag()));
        fieldOop->setFieldValue(classField, fieldInfo->getClass()->getJavaMirror());
        fieldOop->setFieldValue(slotField, new intOopDesc(fieldID->_offset));
        fieldOop->setFieldValue(nameField, java::lang::String::intern(fieldInfo->getName()));
        fieldOop->setFieldValue(typeField, fieldTypeMirror);
        fieldOop->setFieldValue(overrideField, new intOopDesc(JNI_FALSE));

        const auto &signature = fieldInfo->getSignature();
        if (!signature.empty()) {
            fieldOop->setFieldValue(signatureField, java::lang::String::intern(signature));
        }
        return fieldOop;
    }

    static void fillMethodParameterTypes(MethodID *methodID, instanceOop methodOop, FieldID *field) {
        static auto classArrayClass = (ObjectArrayKlass *) BootstrapClassLoader::get()
            ->loadClass(L"[Ljava/lang/Class;");

        const auto &argList = methodID->_method->getArgumentClassTypes();
        objectArrayOop parameterTypes = classArrayClass->newInstance((int) argList.size());
        for (int i = 0; i < parameterTypes->getLength(); ++i) {
            parameterTypes->setElementAt(i, argList[i]);
        }

        methodOop->setFieldValue(field, parameterTypes);
    }

    static void fillMethodBasicFields(MethodID *methodID, instanceOop methodOop,
                                      FieldID *classField, FieldID *slotField,
                                      FieldID *modifiersField, FieldID *signatureField,
                                      FieldID *overrideField) {
        methodOop->setFieldValue(modifiersField, new intOopDesc(
            methodID->_method->getAccessFlag() & (~ACC_ANNOTATION)));
        methodOop->setFieldValue(classField, methodID->_method->getClass()->getJavaMirror());
        methodOop->setFieldValue(slotField, new intOopDesc(methodID->_offset));
        methodOop->setFieldValue(overrideField, new intOopDesc(JNI_FALSE));
        const auto &signature = methodID->_method->getSignature();
        if (!signature.empty()) {
            methodOop->setFieldValue(signatureField, java::lang::String::intern(signature));
        }
    }

    instanceOop newJavaMethodObject(MethodID *method) {
        static auto methodClass = java::lang::reflect::Method::CLASS;

        static auto nameField = methodClass->getInstanceFieldInfo(J_METHOD,
            L"name", L"Ljava/lang/String;");
        static auto returnTypeField = methodClass->getInstanceFieldInfo(J_METHOD,
            L"returnType", L"Ljava/lang/Class;");
        static auto parameterTypesField = methodClass->getInstanceFieldInfo(J_METHOD,
            L"parameterTypes", L"[Ljava/lang/Class;");
        static auto exceptionTypesField = methodClass->getInstanceFieldInfo(J_METHOD,
            L"exceptionTypes", L"[Ljava/lang/Class;");
        static auto modifiersField = methodClass->getInstanceFieldInfo(J_METHOD,
            L"modifiers", L"I");
        static auto signatureField = methodClass->getInstanceFieldInfo(J_METHOD,
            L"signature", L"Ljava/lang/String;");
        static auto overrideField = methodClass->getInstanceFieldInfo(J_ACCESSIBLE_OBJECT,
            L"override", L"Z");

        instanceOop methodOop = methodClass->newInstance();
        return methodOop;
    }

    instanceOop newJavaConstructorObject(MethodID *method) {
        auto constructorClass = java::lang::reflect::Constructor::CLASS;

        static auto parameterTypesField = constructorClass->getInstanceFieldInfo(J_CTOR,
            L"parameterTypes", L"[Ljava/lang/Class;");
        static auto exceptionTypesField = constructorClass->getInstanceFieldInfo(J_CTOR,
            L"exceptionTypes", L"[Ljava/lang/Class;");
        static auto modifiersField = constructorClass->getInstanceFieldInfo(J_CTOR,
            L"modifiers", L"I");
        static auto signatureField = constructorClass->getInstanceFieldInfo(J_CTOR,
            L"signature", L"Ljava/lang/String;");
        static auto overrideField = constructorClass->getInstanceFieldInfo(J_ACCESSIBLE_OBJECT,
            L"override", L"Z");

        instanceOop methodOop = constructorClass->newInstance();

        fillMethodBasicFields(method, methodOop,
            java::lang::reflect::Constructor::FIELD_CLAZZ,
            java::lang::reflect::Constructor::FIELD_SLOT,
            modifiersField, signatureField, overrideField);
        fillMethodParameterTypes(method, methodOop, parameterTypesField);

        // TODO: fill exceptionTypes and annotations
        return methodOop;
    }

    mirrorOop getClassFromConstructor(instanceOop ctorOop) {
        mirrorOop r = nullptr;
        return java::lang::reflect::Constructor::CLASS->getInstanceFieldValue(
            ctorOop, java::lang::reflect::Constructor::FIELD_CLAZZ, (oop *) &r)
               ? r
               : nullptr;
    }

    jint getSlotFromConstructor(instanceOop ctorOop) {
        intOop r = nullptr;
        return java::lang::reflect::Constructor::CLASS->getInstanceFieldValue(
            ctorOop, java::lang::reflect::Constructor::FIELD_SLOT, (oop *) &r)
               ? r->getValue()
               : -1;
    }
}
