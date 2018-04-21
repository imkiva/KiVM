//
// Created by kiva on 2018/4/14.
//
#include <kivm/runtime/nativeMethodPool.h>
#include <kivm/oop/instanceKlass.h>
#include <shared/lock.h>
#include <shared/dl.h>

#include <sstream>

namespace kivm {
    static Lock &nativeMethodPoolLock() {
        static Lock lock;
        return lock;
    }

    void *NativeMethodPool::resolve(Method *method) {
        LockGuard lg(nativeMethodPoolLock());
        auto it = _nativeMethods.find(method);
        if (it != _nativeMethods.end()) {
            return it->second;
        }

        D("Native method not registered, looking up in loaded libraries");

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
            _nativeMethods.insert(std::make_pair(method, nativeSymbol));
        }
        return nativeSymbol;
    }

    NativeMethodPool *NativeMethodPool::get() {
        static NativeMethodPool pool;
        return &pool;
    }
}
