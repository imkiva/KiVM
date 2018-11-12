//
// Created by kiva on 2018/4/14.
//
#include <kivm/jni/nativeMethodPool.h>
#include <kivm/jni/nativeMethod.h>
#include <kivm/oop/instanceKlass.h>
#include <shared/lock.h>
#include <shared/dl.h>

#include <sstream>

namespace kivm {
    static Lock &nativeMethodPoolLock() {
        static Lock lock;
        return lock;
    }

    JavaNativeMethod *NativeMethodPool::resolve(Method *method) {
        LockGuard lg(nativeMethodPoolLock());
        auto it = _nativeMethods.find(method);
        if (it != _nativeMethods.end()) {
            return it->second;
        }

        std::wstringstream nativeSymbolBuilder;

        nativeSymbolBuilder
            << L"Java_"
            << strings::replaceAll(method->getClass()->getName(), Global::SLASH, Global::UNDERLINE)
            << Global::UNDERLINE
            << strings::replaceAll(method->getName(), Global::SLASH, Global::UNDERLINE);

        const String &nativeSymbolName = nativeSymbolBuilder.str();
        D("Looking up symbol: %s", strings::toStdString(nativeSymbolName).c_str());
        dl::DLInterface dlInterface;
        dl::DLSymbol nativeSymbol = dlInterface.findSymbol(strings::toStdString(nativeSymbolName));
        if (nativeSymbol != nullptr) {
            JavaNativeMethod *nativeMethod = new JavaNativeMethod(method, nativeSymbol);
            _nativeMethods.insert(std::make_pair(method, nativeMethod));
            return nativeMethod;
        }
        return nullptr;
    }

    NativeMethodPool *NativeMethodPool::get() {
        static NativeMethodPool pool;
        return &pool;
    }

    void NativeMethodPool::set(Method *method, JavaNativeMethod *nativePointer) {
        if (!method->isNative()) {
            return;
        }

        LockGuard lg(nativeMethodPoolLock());
        _nativeMethods[method] = nativePointer;
    }
}
