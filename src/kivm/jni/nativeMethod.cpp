//
// Created by kiva on 2018/11/13.
//
#include <kivm/jni/nativeMethod.h>
#include <kivm/oop/instanceKlass.h>
#include <shared/lock.h>
#include <shared/dl.h>

#include <sstream>

namespace kivm {
    class NativeMethodPool {
    private:
        HashMap<Method *, JavaNativeMethod *> _nativeMethods;

    public:
        static NativeMethodPool *get() {
            static NativeMethodPool pool;
            return &pool;
        }

        static Lock &nativeMethodPoolLock() {
            static Lock lock;
            return lock;
        }

        void set(Method *method, JavaNativeMethod *nativeMethod) {
            if (!method->isNative()) {
                return;
            }

            LockGuard lg(nativeMethodPoolLock());
            _nativeMethods[method] = nativeMethod;
        }

        JavaNativeMethod *resolve(Method *method) {
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
    };

    JavaNativeMethod *JavaNativeMethod::resolve(Method *method) {
        return NativeMethodPool::get()->resolve(method);
    }

    JavaNativeMethod::JavaNativeMethod(Method *method, JNIMethodPointer target)
        : _javaMethod(method), _invocationTarget(target) {
    }

    JavaNativeMethod::~JavaNativeMethod() = default;
}
