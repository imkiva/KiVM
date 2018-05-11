//
// Created by kiva on 2018/5/1.
//
#include <kivm/native/classNames.h>
#include <kivm/oop/reflection.h>
#include <kivm/oop/instanceKlass.h>
#include <kivm/oop/instanceOop.h>
#include <kivm/oop/mirrorOop.h>
#include <kivm/oop/primitiveOop.h>
#include <kivm/native/java_lang_Class.h>

namespace kivm {
    instanceOop newJavaLangReflectField(FieldID *fieldID) {
        static auto fieldClass = (InstanceKlass *) BootstrapClassLoader::get()
            ->loadClass(L"java/lang/reflect/Field");

        static auto classFieldId = fieldClass->getInstanceFieldInfo(J_FIELD, L"clazz", L"Ljava/lang/Class;");
        static auto typeFieldId = fieldClass->getInstanceFieldInfo(J_FIELD, L"type", L"Ljava/lang/Class;");
        static auto slotField = fieldClass->getInstanceFieldInfo(J_FIELD, L"slot", L"I");
        static auto nameField = fieldClass->getInstanceFieldInfo(J_FIELD, L"name", L"Ljava/lang/String;");
        static auto signatureField = fieldClass->getInstanceFieldInfo(J_FIELD, L"signature", L"Ljava/lang/String;");
        static auto modifiersField = fieldClass->getInstanceFieldInfo(J_FIELD, L"modifiers", L"I");
        static auto overrideField = fieldClass->getInstanceFieldInfo(J_ACCESSIBLE_OBJECT, L"override", L"Z");

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
        fieldOop->setFieldValue(classFieldId, fieldInfo->getClass()->getJavaMirror());
        fieldOop->setFieldValue(slotField, new intOopDesc(fieldID->_offset));
        fieldOop->setFieldValue(nameField, java::lang::String::intern(fieldInfo->getName()));
        fieldOop->setFieldValue(typeFieldId, fieldTypeMirror);
        fieldOop->setFieldValue(overrideField, new intOopDesc(JNI_FALSE));

        const auto &signature = fieldInfo->getSignature();
        if (!signature.empty()) {
            fieldOop->setFieldValue(signatureField, java::lang::String::intern(signature));
        }
        return fieldOop;
    }
}
