//
// Created by kiva on 2018/11/11.
//

#include <kivm/kivm.h>
#include <kivm/jni/nativeLibrariy.h>

namespace kivm {
    using JNIOnLoadFunction = jint(*)(JavaVM *, void *);
    using JNIOnUnloadFunction = void (*)(JavaVM *, void *);

    JavaNativeLibrary::JavaNativeLibrary(const String &libraryName)
        : _libraryName(libraryName), _linked(false) {
    }

    bool JavaNativeLibrary::prepare() {
        const String &path = findLibrary(_libraryName);
        int javaVersion = JNI_VERSION_UNKNOWN;
        if (_sharedLibrary.open(strings::toStdString(path))) {
            auto onLoadFunction = (JNIOnLoadFunction) _sharedLibrary.findSymbol("JNI_OnLoad");
            if (onLoadFunction) {
                javaVersion = onLoadFunction(KiVM::getJavaVMQuick(), nullptr);
            }
        }
        _linked = KiVM::checkJavaVersion(javaVersion);
        return _linked;
    }

    void JavaNativeLibrary::dispose() {
        if (_linked) {
            auto onUnloadFunction = (JNIOnUnloadFunction) _sharedLibrary.findSymbol("JNI_OnUnload");
            if (onUnloadFunction) {
                onUnloadFunction(KiVM::getJavaVMQuick(), nullptr);
            }
            _linked = false;
        }
    }

    String JavaNativeLibrary::findLibrary(const String &libraryName) {
        return kivm::String();
    }

    JavaNativeLibrary::~JavaNativeLibrary() {
        dispose();
    }
}