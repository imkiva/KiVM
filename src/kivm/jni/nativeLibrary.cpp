//
// Created by kiva on 2018/11/11.
//

#include <kivm/jni/nativeLibrariy.h>

namespace kivm {
    using JNIOnLoadFunction = jint(*)(JavaVM *, void *);
    using JNIOnUnloadFunction = void (*)(JavaVM *, void *);

    JavaNativeLibrary::JavaNativeLibrary(const String &libraryName)
        : libraryName(libraryName), linked(false) {
    }

    bool JavaNativeLibrary::openAndLink() {
        const String &path = findLibrary(libraryName);
        int javaVersion = JNI_VERSION_UNKNOWN;
        if (sharedLibrary.open(strings::toStdString(path))) {
            auto onLoadFunction = (JNIOnLoadFunction) sharedLibrary.findSymbol("JNI_OnLoad");
            if (onLoadFunction) {
                javaVersion = onLoadFunction(KiVM::getJavaVMQuick(), nullptr);
            }
        }
        linked = KiVM::checkJavaVersion(javaVersion);
        return linked;
    }

    void JavaNativeLibrary::unlinkAndClose() {
        if (linked) {
            auto onUnloadFunction = (JNIOnUnloadFunction) sharedLibrary.findSymbol("JNI_OnUnload");
            if (onUnloadFunction) {
                onUnloadFunction(KiVM::getJavaVMQuick(), nullptr);
            }
            linked = false;
        }
    }

    String JavaNativeLibrary::findLibrary(const String &libraryName) {
        return kivm::String();
    }

    JavaNativeLibrary::~JavaNativeLibrary() {
        unlinkAndClose();
    }
}