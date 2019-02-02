//
// Created by kiva on 2018/4/16.
//
#include <kivm/kivm.h>
#include <kivm/jni/jniJavaVM.h>
#include <kivm/jni/jniEnv.h>
#include <kivm/runtime/abstractThread.h>
#include <kivm/memory/universe.h>
#include <kivm/classpath/classPathManager.h>
#include <kivm/bytecode/interpreter.h>
#include <kivm/memory/gcThread.h>
#include <random>
#include <kivm/bytecode/javaCall.h>

#if defined(KIVM_PLATFORM_UNIX) || defined(KIVM_PLATFORM_APPLE)
#   define PATH_SEPARATOR_CHAR L"/"
#   define PATH_DELIMITER_CHAR L":"
#elif defined(KIVM_PLATFORM_WINDOWS)
#   define PATH_SEPARATOR_CHAR L"\\"
#   define PATH_DELIMITER_CHAR L";"
#endif

namespace kivm {
    String Global::SLASH(L"/"); // NOLINT
    String Global::DOT(L"."); // NOLINT
    String Global::UNDERLINE(L"_"); // NOLINT
    String Global::PATH_SEPARATOR(PATH_SEPARATOR_CHAR); // NOLINT
    String Global::PATH_DELIMITER(PATH_DELIMITER_CHAR); // NOLINT
    String Global::CLASS_EXTENSION(L"class"); // NOLINT
    InstanceKlass *Global::java_lang_Object = nullptr;
    InstanceKlass *Global::java_lang_Cloneable = nullptr;
    InstanceKlass *Global::java_lang_Serializable = nullptr;
    InstanceKlass *Global::java_lang_NullPointerException = nullptr;
    InstanceKlass *Global::java_lang_ArrayIndexOutOfBoundsException = nullptr;
    InstanceKlass *Global::java_lang_ClassNotFoundException = nullptr;
    InstanceKlass *Global::java_lang_InternalError = nullptr;
    InstanceKlass *Global::java_io_IOException = nullptr;

    JavaObject("java/nio/charset/Charset") Global::DEFAULT_UTF8_CHARSET = nullptr;

    bool Global::jvmBooted = false;

    JavaVM *KiVM::sJavaVMInstance = nullptr;
    JNIEnv *KiVM::sJNIEnvInstance = nullptr;

    int KiVM::createVirtualMachine(JavaVM **pJavaVM, JNIEnv **pEnv, JavaVMInitArgs *initArgs) {
        if (KiVM::sJavaVMInstance != nullptr || pJavaVM == nullptr) {
            return JNI_ERR;
        }

        std::default_random_engine dre((unsigned int) time(nullptr));
        std::uniform_int_distribution<int> distribution(0xABC, INT32_MAX);
        distribution(dre);

        // initialize memory
        Universe::initialize();

        // GC
        GCThread::initialize();
        auto gc = GCThread::get();
        if (gc != nullptr) {
            gc->start();
        } else {
            WARN("createVirtualMachine: failed to init gc thread");
        }

        // initialize classpath
        ClassPathManager::initialize();

        // interpreters
        DefaultInterpreter::initialize();

        auto invokeInterface = new JNIInvokeInterface_;
        invokeInterface->AttachCurrentThread = JNI_ENTRY_NAME(AttachCurrentThread);
        invokeInterface->AttachCurrentThreadAsDaemon = JNI_ENTRY_NAME(AttachCurrentThreadAsDaemon);
        invokeInterface->DestroyJavaVM = JNI_ENTRY_NAME(DestroyJavaVM);
        invokeInterface->DetachCurrentThread = JNI_ENTRY_NAME(DetachCurrentThread);
        invokeInterface->GetEnv = JNI_ENTRY_NAME(GetEnv);

        sJavaVMInstance = new JavaVM;
        sJavaVMInstance->functions = invokeInterface;
        *pJavaVM = sJavaVMInstance;

        sJNIEnvInstance = new JNIEnv;
        auto nativeInterface = new JNINativeInterface_;
        kivm::KiVM::fillInterfaceFunctions(nativeInterface);
        sJNIEnvInstance->functions = nativeInterface;
        *pEnv = sJNIEnvInstance;

        return JNI_OK;
    }

    int KiVM::destroyJavaVM(JavaVM *vm) {
        if (vm == nullptr || vm != sJavaVMInstance) {
            return JNI_ERR;
        }

        auto gc = GCThread::get();
        if (gc != nullptr) {
            gc->stop();
        }

        ClassPathManager::get()->destroy();
        Universe::destroy();

        delete sJNIEnvInstance->functions;
        delete sJNIEnvInstance;
        delete sJavaVMInstance->functions;
        delete sJavaVMInstance;
        sJavaVMInstance = nullptr;
        sJNIEnvInstance = nullptr;

        // TODO: notify JavaVM destroyed
        return JNI_OK;
    }

    int KiVM::getJavaVM(JavaVM **pJavaVM) {
        if (pJavaVM == nullptr) {
            return JNI_ERR;
        }

        *pJavaVM = sJavaVMInstance;
        return JNI_OK;
    }

    int KiVM::getEnv(JavaVM *vm, JNIEnv **pEnv, int version) {
        if (vm == nullptr || vm != sJavaVMInstance || pEnv == nullptr) {
            return JNI_ERR;
        }

        if (!checkJavaVersion(version)) {
            return JNI_EVERSION;
        }

        *pEnv = sJNIEnvInstance;
        return JNI_OK;
    }

    void KiVM::fillInterfaceFunctions(JNINativeInterface_ *nativeInterface) {
        if (nativeInterface == nullptr) {
            return;
        }

        nativeInterface->GetVersion = JNI_ENTRY_NAME(GetVersion);
        nativeInterface->DefineClass = JNI_ENTRY_NAME(DefineClass);
        nativeInterface->FindClass = JNI_ENTRY_NAME(FindClass);
        nativeInterface->FromReflectedMethod = JNI_ENTRY_NAME(FromReflectedMethod);
        nativeInterface->FromReflectedField = JNI_ENTRY_NAME(FromReflectedField);
        nativeInterface->ToReflectedMethod = JNI_ENTRY_NAME(ToReflectedMethod);
        nativeInterface->GetSuperclass = JNI_ENTRY_NAME(GetSuperclass);
        nativeInterface->IsAssignableFrom = JNI_ENTRY_NAME(IsAssignableFrom);
        nativeInterface->ToReflectedField = JNI_ENTRY_NAME(ToReflectedField);
        nativeInterface->Throw = JNI_ENTRY_NAME(Throw);
        nativeInterface->ThrowNew = JNI_ENTRY_NAME(ThrowNew);
        nativeInterface->ExceptionOccurred = JNI_ENTRY_NAME(ExceptionOccurred);
        nativeInterface->ExceptionDescribe = JNI_ENTRY_NAME(ExceptionDescribe);
        nativeInterface->ExceptionClear = JNI_ENTRY_NAME(ExceptionClear);
        nativeInterface->FatalError = JNI_ENTRY_NAME(FatalError);
        nativeInterface->PushLocalFrame = JNI_ENTRY_NAME(PushLocalFrame);
        nativeInterface->PopLocalFrame = JNI_ENTRY_NAME(PopLocalFrame);
        nativeInterface->NewGlobalRef = JNI_ENTRY_NAME(NewGlobalRef);
        nativeInterface->DeleteGlobalRef = JNI_ENTRY_NAME(DeleteGlobalRef);
        nativeInterface->DeleteLocalRef = JNI_ENTRY_NAME(DeleteLocalRef);
        nativeInterface->IsSameObject = JNI_ENTRY_NAME(IsSameObject);
        nativeInterface->NewLocalRef = JNI_ENTRY_NAME(NewLocalRef);
        nativeInterface->EnsureLocalCapacity = JNI_ENTRY_NAME(EnsureLocalCapacity);
        nativeInterface->AllocObject = JNI_ENTRY_NAME(AllocObject);
        nativeInterface->NewObject = JNI_ENTRY_NAME(NewObject);
        nativeInterface->NewObjectV = JNI_ENTRY_NAME(NewObjectV);
        nativeInterface->NewObjectA = JNI_ENTRY_NAME(NewObjectA);
        nativeInterface->GetObjectClass = JNI_ENTRY_NAME(GetObjectClass);
        nativeInterface->IsInstanceOf = JNI_ENTRY_NAME(IsInstanceOf);
        nativeInterface->GetMethodID = JNI_ENTRY_NAME(GetMethodID);
        nativeInterface->CallObjectMethod = JNI_ENTRY_NAME(CallObjectMethod);
        nativeInterface->CallObjectMethodV = JNI_ENTRY_NAME(CallObjectMethodV);
        nativeInterface->CallObjectMethodA = JNI_ENTRY_NAME(CallObjectMethodA);
        nativeInterface->CallBooleanMethod = JNI_ENTRY_NAME(CallBooleanMethod);
        nativeInterface->CallBooleanMethodV = JNI_ENTRY_NAME(CallBooleanMethodV);
        nativeInterface->CallBooleanMethodA = JNI_ENTRY_NAME(CallBooleanMethodA);
        nativeInterface->CallByteMethod = JNI_ENTRY_NAME(CallByteMethod);
        nativeInterface->CallByteMethodV = JNI_ENTRY_NAME(CallByteMethodV);
        nativeInterface->CallByteMethodA = JNI_ENTRY_NAME(CallByteMethodA);
        nativeInterface->CallCharMethod = JNI_ENTRY_NAME(CallCharMethod);
        nativeInterface->CallCharMethodV = JNI_ENTRY_NAME(CallCharMethodV);
        nativeInterface->CallCharMethodA = JNI_ENTRY_NAME(CallCharMethodA);
        nativeInterface->CallShortMethod = JNI_ENTRY_NAME(CallShortMethod);
        nativeInterface->CallShortMethodV = JNI_ENTRY_NAME(CallShortMethodV);
        nativeInterface->CallShortMethodA = JNI_ENTRY_NAME(CallShortMethodA);
        nativeInterface->CallIntMethod = JNI_ENTRY_NAME(CallIntMethod);
        nativeInterface->CallIntMethodV = JNI_ENTRY_NAME(CallIntMethodV);
        nativeInterface->CallIntMethodA = JNI_ENTRY_NAME(CallIntMethodA);
        nativeInterface->CallLongMethod = JNI_ENTRY_NAME(CallLongMethod);
        nativeInterface->CallLongMethodV = JNI_ENTRY_NAME(CallLongMethodV);
        nativeInterface->CallLongMethodA = JNI_ENTRY_NAME(CallLongMethodA);
        nativeInterface->CallFloatMethod = JNI_ENTRY_NAME(CallFloatMethod);
        nativeInterface->CallFloatMethodV = JNI_ENTRY_NAME(CallFloatMethodV);
        nativeInterface->CallFloatMethodA = JNI_ENTRY_NAME(CallFloatMethodA);
        nativeInterface->CallDoubleMethod = JNI_ENTRY_NAME(CallDoubleMethod);
        nativeInterface->CallDoubleMethodV = JNI_ENTRY_NAME(CallDoubleMethodV);
        nativeInterface->CallDoubleMethodA = JNI_ENTRY_NAME(CallDoubleMethodA);
        nativeInterface->CallVoidMethod = JNI_ENTRY_NAME(CallVoidMethod);
        nativeInterface->CallVoidMethodV = JNI_ENTRY_NAME(CallVoidMethodV);
        nativeInterface->CallVoidMethodA = JNI_ENTRY_NAME(CallVoidMethodA);
        nativeInterface->CallNonvirtualObjectMethod = JNI_ENTRY_NAME(CallNonvirtualObjectMethod);
        nativeInterface->CallNonvirtualObjectMethodV = JNI_ENTRY_NAME(CallNonvirtualObjectMethodV);
        nativeInterface->CallNonvirtualObjectMethodA = JNI_ENTRY_NAME(CallNonvirtualObjectMethodA);
        nativeInterface->CallNonvirtualBooleanMethod = JNI_ENTRY_NAME(CallNonvirtualBooleanMethod);
        nativeInterface->CallNonvirtualBooleanMethodV = JNI_ENTRY_NAME(CallNonvirtualBooleanMethodV);
        nativeInterface->CallNonvirtualBooleanMethodA = JNI_ENTRY_NAME(CallNonvirtualBooleanMethodA);
        nativeInterface->CallNonvirtualByteMethod = JNI_ENTRY_NAME(CallNonvirtualByteMethod);
        nativeInterface->CallNonvirtualByteMethodV = JNI_ENTRY_NAME(CallNonvirtualByteMethodV);
        nativeInterface->CallNonvirtualByteMethodA = JNI_ENTRY_NAME(CallNonvirtualByteMethodA);
        nativeInterface->CallNonvirtualCharMethod = JNI_ENTRY_NAME(CallNonvirtualCharMethod);
        nativeInterface->CallNonvirtualCharMethodV = JNI_ENTRY_NAME(CallNonvirtualCharMethodV);
        nativeInterface->CallNonvirtualCharMethodA = JNI_ENTRY_NAME(CallNonvirtualCharMethodA);
        nativeInterface->CallNonvirtualShortMethod = JNI_ENTRY_NAME(CallNonvirtualShortMethod);
        nativeInterface->CallNonvirtualShortMethodV = JNI_ENTRY_NAME(CallNonvirtualShortMethodV);
        nativeInterface->CallNonvirtualShortMethodA = JNI_ENTRY_NAME(CallNonvirtualShortMethodA);
        nativeInterface->CallNonvirtualIntMethod = JNI_ENTRY_NAME(CallNonvirtualIntMethod);
        nativeInterface->CallNonvirtualIntMethodV = JNI_ENTRY_NAME(CallNonvirtualIntMethodV);
        nativeInterface->CallNonvirtualIntMethodA = JNI_ENTRY_NAME(CallNonvirtualIntMethodA);
        nativeInterface->CallNonvirtualLongMethod = JNI_ENTRY_NAME(CallNonvirtualLongMethod);
        nativeInterface->CallNonvirtualLongMethodV = JNI_ENTRY_NAME(CallNonvirtualLongMethodV);
        nativeInterface->CallNonvirtualLongMethodA = JNI_ENTRY_NAME(CallNonvirtualLongMethodA);
        nativeInterface->CallNonvirtualFloatMethod = JNI_ENTRY_NAME(CallNonvirtualFloatMethod);
        nativeInterface->CallNonvirtualFloatMethodV = JNI_ENTRY_NAME(CallNonvirtualFloatMethodV);
        nativeInterface->CallNonvirtualFloatMethodA = JNI_ENTRY_NAME(CallNonvirtualFloatMethodA);
        nativeInterface->CallNonvirtualDoubleMethod = JNI_ENTRY_NAME(CallNonvirtualDoubleMethod);
        nativeInterface->CallNonvirtualDoubleMethodV = JNI_ENTRY_NAME(CallNonvirtualDoubleMethodV);
        nativeInterface->CallNonvirtualDoubleMethodA = JNI_ENTRY_NAME(CallNonvirtualDoubleMethodA);
        nativeInterface->CallNonvirtualVoidMethod = JNI_ENTRY_NAME(CallNonvirtualVoidMethod);
        nativeInterface->CallNonvirtualVoidMethodV = JNI_ENTRY_NAME(CallNonvirtualVoidMethodV);
        nativeInterface->CallNonvirtualVoidMethodA = JNI_ENTRY_NAME(CallNonvirtualVoidMethodA);
        nativeInterface->GetFieldID = JNI_ENTRY_NAME(GetFieldID);
        nativeInterface->GetObjectField = JNI_ENTRY_NAME(GetObjectField);
        nativeInterface->GetBooleanField = JNI_ENTRY_NAME(GetBooleanField);
        nativeInterface->GetByteField = JNI_ENTRY_NAME(GetByteField);
        nativeInterface->GetCharField = JNI_ENTRY_NAME(GetCharField);
        nativeInterface->GetShortField = JNI_ENTRY_NAME(GetShortField);
        nativeInterface->GetIntField = JNI_ENTRY_NAME(GetIntField);
        nativeInterface->GetLongField = JNI_ENTRY_NAME(GetLongField);
        nativeInterface->GetFloatField = JNI_ENTRY_NAME(GetFloatField);
        nativeInterface->GetDoubleField = JNI_ENTRY_NAME(GetDoubleField);
        nativeInterface->SetObjectField = JNI_ENTRY_NAME(SetObjectField);
        nativeInterface->SetBooleanField = JNI_ENTRY_NAME(SetBooleanField);
        nativeInterface->SetByteField = JNI_ENTRY_NAME(SetByteField);
        nativeInterface->SetCharField = JNI_ENTRY_NAME(SetCharField);
        nativeInterface->SetShortField = JNI_ENTRY_NAME(SetShortField);
        nativeInterface->SetIntField = JNI_ENTRY_NAME(SetIntField);
        nativeInterface->SetLongField = JNI_ENTRY_NAME(SetLongField);
        nativeInterface->SetFloatField = JNI_ENTRY_NAME(SetFloatField);
        nativeInterface->SetDoubleField = JNI_ENTRY_NAME(SetDoubleField);
        nativeInterface->GetStaticMethodID = JNI_ENTRY_NAME(GetStaticMethodID);
        nativeInterface->CallStaticObjectMethod = JNI_ENTRY_NAME(CallStaticObjectMethod);
        nativeInterface->CallStaticObjectMethodV = JNI_ENTRY_NAME(CallStaticObjectMethodV);
        nativeInterface->CallStaticObjectMethodA = JNI_ENTRY_NAME(CallStaticObjectMethodA);
        nativeInterface->CallStaticBooleanMethod = JNI_ENTRY_NAME(CallStaticBooleanMethod);
        nativeInterface->CallStaticBooleanMethodV = JNI_ENTRY_NAME(CallStaticBooleanMethodV);
        nativeInterface->CallStaticBooleanMethodA = JNI_ENTRY_NAME(CallStaticBooleanMethodA);
        nativeInterface->CallStaticByteMethod = JNI_ENTRY_NAME(CallStaticByteMethod);
        nativeInterface->CallStaticByteMethodV = JNI_ENTRY_NAME(CallStaticByteMethodV);
        nativeInterface->CallStaticByteMethodA = JNI_ENTRY_NAME(CallStaticByteMethodA);
        nativeInterface->CallStaticCharMethod = JNI_ENTRY_NAME(CallStaticCharMethod);
        nativeInterface->CallStaticCharMethodV = JNI_ENTRY_NAME(CallStaticCharMethodV);
        nativeInterface->CallStaticCharMethodA = JNI_ENTRY_NAME(CallStaticCharMethodA);
        nativeInterface->CallStaticShortMethod = JNI_ENTRY_NAME(CallStaticShortMethod);
        nativeInterface->CallStaticShortMethodV = JNI_ENTRY_NAME(CallStaticShortMethodV);
        nativeInterface->CallStaticShortMethodA = JNI_ENTRY_NAME(CallStaticShortMethodA);
        nativeInterface->CallStaticIntMethod = JNI_ENTRY_NAME(CallStaticIntMethod);
        nativeInterface->CallStaticIntMethodV = JNI_ENTRY_NAME(CallStaticIntMethodV);
        nativeInterface->CallStaticIntMethodA = JNI_ENTRY_NAME(CallStaticIntMethodA);
        nativeInterface->CallStaticLongMethod = JNI_ENTRY_NAME(CallStaticLongMethod);
        nativeInterface->CallStaticLongMethodV = JNI_ENTRY_NAME(CallStaticLongMethodV);
        nativeInterface->CallStaticLongMethodA = JNI_ENTRY_NAME(CallStaticLongMethodA);
        nativeInterface->CallStaticFloatMethod = JNI_ENTRY_NAME(CallStaticFloatMethod);
        nativeInterface->CallStaticFloatMethodV = JNI_ENTRY_NAME(CallStaticFloatMethodV);
        nativeInterface->CallStaticFloatMethodA = JNI_ENTRY_NAME(CallStaticFloatMethodA);
        nativeInterface->CallStaticDoubleMethod = JNI_ENTRY_NAME(CallStaticDoubleMethod);
        nativeInterface->CallStaticDoubleMethodV = JNI_ENTRY_NAME(CallStaticDoubleMethodV);
        nativeInterface->CallStaticDoubleMethodA = JNI_ENTRY_NAME(CallStaticDoubleMethodA);
        nativeInterface->CallStaticVoidMethod = JNI_ENTRY_NAME(CallStaticVoidMethod);
        nativeInterface->CallStaticVoidMethodV = JNI_ENTRY_NAME(CallStaticVoidMethodV);
        nativeInterface->CallStaticVoidMethodA = JNI_ENTRY_NAME(CallStaticVoidMethodA);
        nativeInterface->GetStaticFieldID = JNI_ENTRY_NAME(GetStaticFieldID);
        nativeInterface->GetStaticObjectField = JNI_ENTRY_NAME(GetStaticObjectField);
        nativeInterface->GetStaticBooleanField = JNI_ENTRY_NAME(GetStaticBooleanField);
        nativeInterface->GetStaticByteField = JNI_ENTRY_NAME(GetStaticByteField);
        nativeInterface->GetStaticCharField = JNI_ENTRY_NAME(GetStaticCharField);
        nativeInterface->GetStaticShortField = JNI_ENTRY_NAME(GetStaticShortField);
        nativeInterface->GetStaticIntField = JNI_ENTRY_NAME(GetStaticIntField);
        nativeInterface->GetStaticLongField = JNI_ENTRY_NAME(GetStaticLongField);
        nativeInterface->GetStaticFloatField = JNI_ENTRY_NAME(GetStaticFloatField);
        nativeInterface->GetStaticDoubleField = JNI_ENTRY_NAME(GetStaticDoubleField);
        nativeInterface->SetStaticObjectField = JNI_ENTRY_NAME(SetStaticObjectField);
        nativeInterface->SetStaticBooleanField = JNI_ENTRY_NAME(SetStaticBooleanField);
        nativeInterface->SetStaticByteField = JNI_ENTRY_NAME(SetStaticByteField);
        nativeInterface->SetStaticCharField = JNI_ENTRY_NAME(SetStaticCharField);
        nativeInterface->SetStaticShortField = JNI_ENTRY_NAME(SetStaticShortField);
        nativeInterface->SetStaticIntField = JNI_ENTRY_NAME(SetStaticIntField);
        nativeInterface->SetStaticLongField = JNI_ENTRY_NAME(SetStaticLongField);
        nativeInterface->SetStaticFloatField = JNI_ENTRY_NAME(SetStaticFloatField);
        nativeInterface->SetStaticDoubleField = JNI_ENTRY_NAME(SetStaticDoubleField);
        nativeInterface->NewString = JNI_ENTRY_NAME(NewString);
        nativeInterface->GetStringLength = JNI_ENTRY_NAME(GetStringLength);
        nativeInterface->GetStringChars = JNI_ENTRY_NAME(GetStringChars);
        nativeInterface->ReleaseStringChars = JNI_ENTRY_NAME(ReleaseStringChars);
        nativeInterface->NewStringUTF = JNI_ENTRY_NAME(NewStringUTF);
        nativeInterface->GetStringUTFLength = JNI_ENTRY_NAME(GetStringUTFLength);
        nativeInterface->GetStringUTFChars = JNI_ENTRY_NAME(GetStringUTFChars);
        nativeInterface->ReleaseStringUTFChars = JNI_ENTRY_NAME(ReleaseStringUTFChars);
        nativeInterface->GetArrayLength = JNI_ENTRY_NAME(GetArrayLength);
        nativeInterface->NewObjectArray = JNI_ENTRY_NAME(NewObjectArray);
        nativeInterface->GetObjectArrayElement = JNI_ENTRY_NAME(GetObjectArrayElement);
        nativeInterface->SetObjectArrayElement = JNI_ENTRY_NAME(SetObjectArrayElement);
        nativeInterface->NewBooleanArray = JNI_ENTRY_NAME(NewBooleanArray);
        nativeInterface->NewByteArray = JNI_ENTRY_NAME(NewByteArray);
        nativeInterface->NewCharArray = JNI_ENTRY_NAME(NewCharArray);
        nativeInterface->NewShortArray = JNI_ENTRY_NAME(NewShortArray);
        nativeInterface->NewIntArray = JNI_ENTRY_NAME(NewIntArray);
        nativeInterface->NewLongArray = JNI_ENTRY_NAME(NewLongArray);
        nativeInterface->NewFloatArray = JNI_ENTRY_NAME(NewFloatArray);
        nativeInterface->NewDoubleArray = JNI_ENTRY_NAME(NewDoubleArray);
        nativeInterface->GetBooleanArrayElements = JNI_ENTRY_NAME(GetBooleanArrayElements);
        nativeInterface->GetByteArrayElements = JNI_ENTRY_NAME(GetByteArrayElements);
        nativeInterface->GetCharArrayElements = JNI_ENTRY_NAME(GetCharArrayElements);
        nativeInterface->GetShortArrayElements = JNI_ENTRY_NAME(GetShortArrayElements);
        nativeInterface->GetIntArrayElements = JNI_ENTRY_NAME(GetIntArrayElements);
        nativeInterface->GetLongArrayElements = JNI_ENTRY_NAME(GetLongArrayElements);
        nativeInterface->GetFloatArrayElements = JNI_ENTRY_NAME(GetFloatArrayElements);
        nativeInterface->GetDoubleArrayElements = JNI_ENTRY_NAME(GetDoubleArrayElements);
        nativeInterface->ReleaseBooleanArrayElements = JNI_ENTRY_NAME(ReleaseBooleanArrayElements);
        nativeInterface->ReleaseByteArrayElements = JNI_ENTRY_NAME(ReleaseByteArrayElements);
        nativeInterface->ReleaseCharArrayElements = JNI_ENTRY_NAME(ReleaseCharArrayElements);
        nativeInterface->ReleaseShortArrayElements = JNI_ENTRY_NAME(ReleaseShortArrayElements);
        nativeInterface->ReleaseIntArrayElements = JNI_ENTRY_NAME(ReleaseIntArrayElements);
        nativeInterface->ReleaseLongArrayElements = JNI_ENTRY_NAME(ReleaseLongArrayElements);
        nativeInterface->ReleaseFloatArrayElements = JNI_ENTRY_NAME(ReleaseFloatArrayElements);
        nativeInterface->ReleaseDoubleArrayElements = JNI_ENTRY_NAME(ReleaseDoubleArrayElements);
        nativeInterface->GetBooleanArrayRegion = JNI_ENTRY_NAME(GetBooleanArrayRegion);
        nativeInterface->GetByteArrayRegion = JNI_ENTRY_NAME(GetByteArrayRegion);
        nativeInterface->GetCharArrayRegion = JNI_ENTRY_NAME(GetCharArrayRegion);
        nativeInterface->GetShortArrayRegion = JNI_ENTRY_NAME(GetShortArrayRegion);
        nativeInterface->GetIntArrayRegion = JNI_ENTRY_NAME(GetIntArrayRegion);
        nativeInterface->GetLongArrayRegion = JNI_ENTRY_NAME(GetLongArrayRegion);
        nativeInterface->GetFloatArrayRegion = JNI_ENTRY_NAME(GetFloatArrayRegion);
        nativeInterface->GetDoubleArrayRegion = JNI_ENTRY_NAME(GetDoubleArrayRegion);
        nativeInterface->SetBooleanArrayRegion = JNI_ENTRY_NAME(SetBooleanArrayRegion);
        nativeInterface->SetByteArrayRegion = JNI_ENTRY_NAME(SetByteArrayRegion);
        nativeInterface->SetCharArrayRegion = JNI_ENTRY_NAME(SetCharArrayRegion);
        nativeInterface->SetShortArrayRegion = JNI_ENTRY_NAME(SetShortArrayRegion);
        nativeInterface->SetIntArrayRegion = JNI_ENTRY_NAME(SetIntArrayRegion);
        nativeInterface->SetLongArrayRegion = JNI_ENTRY_NAME(SetLongArrayRegion);
        nativeInterface->SetFloatArrayRegion = JNI_ENTRY_NAME(SetFloatArrayRegion);
        nativeInterface->SetDoubleArrayRegion = JNI_ENTRY_NAME(SetDoubleArrayRegion);
        nativeInterface->RegisterNatives = JNI_ENTRY_NAME(RegisterNatives);
        nativeInterface->UnregisterNatives = JNI_ENTRY_NAME(UnregisterNatives);
        nativeInterface->MonitorEnter = JNI_ENTRY_NAME(MonitorEnter);
        nativeInterface->MonitorExit = JNI_ENTRY_NAME(MonitorExit);
        nativeInterface->GetJavaVM = JNI_ENTRY_NAME(GetJavaVM);
        nativeInterface->GetStringRegion = JNI_ENTRY_NAME(GetStringRegion);
        nativeInterface->GetStringUTFRegion = JNI_ENTRY_NAME(GetStringUTFRegion);
        nativeInterface->GetPrimitiveArrayCritical = JNI_ENTRY_NAME(GetPrimitiveArrayCritical);
        nativeInterface->ReleasePrimitiveArrayCritical = JNI_ENTRY_NAME(ReleasePrimitiveArrayCritical);
        nativeInterface->GetStringCritical = JNI_ENTRY_NAME(GetStringCritical);
        nativeInterface->ReleaseStringCritical = JNI_ENTRY_NAME(ReleaseStringCritical);
        nativeInterface->NewWeakGlobalRef = JNI_ENTRY_NAME(NewWeakGlobalRef);
        nativeInterface->DeleteWeakGlobalRef = JNI_ENTRY_NAME(DeleteWeakGlobalRef);
        nativeInterface->ExceptionCheck = JNI_ENTRY_NAME(ExceptionCheck);
        nativeInterface->NewDirectByteBuffer = JNI_ENTRY_NAME(NewDirectByteBuffer);
        nativeInterface->GetDirectBufferAddress = JNI_ENTRY_NAME(GetDirectBufferAddress);
        nativeInterface->GetDirectBufferCapacity = JNI_ENTRY_NAME(GetDirectBufferCapacity);
    }

    void KiVM::uncaughtException(JavaThread *exceptionThread) {
        static bool FIRST_TIME_DISPATCH = true;
        auto ex = exceptionThread->_exceptionOop;

        // clear exception object because we have handled it
        exceptionThread->_exceptionOop = nullptr;

        auto javaThreadOop = exceptionThread->getJavaThreadObject();

        if (javaThreadOop == nullptr) {
            KiVM::uncaughtExceptionJVMInternal(ex);
            return;
        }

        auto threadClass = javaThreadOop->getInstanceClass();
        auto dispatcher = threadClass->getThisClassMethod(L"dispatchUncaughtException",
            L"(Ljava/lang/Throwable;)V");
        if (dispatcher == nullptr) {
            KiVM::uncaughtExceptionJVMInternal(ex);
            return;
        }

        if (FIRST_TIME_DISPATCH) {
            FIRST_TIME_DISPATCH = false;
            JavaCall::withArgs(exceptionThread, dispatcher, {javaThreadOop, ex});
        } else {
            KiVM::uncaughtExceptionJVMInternal(ex);
        }
    }

    void KiVM::uncaughtExceptionJVMInternal(instanceOop exceptionOop) {
        oop messageOop = nullptr;

        if (exceptionOop->getFieldValue(L"java/lang/Throwable", L"detailMessage", L"Ljava/lang/String;", &messageOop)) {
            if (messageOop == nullptr) {
                PANIC("(JVM) UncaughtException: %s",
                    strings::toStdString(exceptionOop->getInstanceClass()->getName()).c_str());

            } else if (messageOop->getClass()->getClassType() == ClassType::INSTANCE_CLASS) {
                auto instance = (instanceOop) messageOop;
                PANIC("(JVM) UncaughtException: %s: %s",
                    strings::toStdString(exceptionOop->getInstanceClass()->getName()).c_str(),
                    strings::toStdString(java::lang::String::toNativeString(instance)).c_str());

            } else {
                PANIC("(JVM) UncaughtException: %s (failed to convert message oop)",
                    strings::toStdString(exceptionOop->getInstanceClass()->getName()).c_str());
            }

        } else {
            PANIC("(JVM) UncaughtException: %s (failed to obtain message)",
                strings::toStdString(exceptionOop->getInstanceClass()->getName()).c_str());
        }
    }

    bool KiVM::checkJavaVersion(int javaVersion) {
        switch (javaVersion) {
            case JNI_VERSION_1_1:
            case JNI_VERSION_1_2:
            case JNI_VERSION_1_4:
            case JNI_VERSION_1_6:
            case JNI_VERSION_1_8:
                return true;
            default:
                return false;
        }
    }
}
