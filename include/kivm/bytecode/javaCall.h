//
// Created by kiva on 2018/4/13.
//
#pragma once

#include <kivm/runtime/javaThread.h>
#include <list>

namespace kivm {
    class JavaCall final {
    private:
        JavaThread *_thread;
        Method *_method;
        Stack *_stack;
        std::list<oop> _args;
        bool _obtainArgsFromStack;

        InstanceKlass *_instanceKlass;

    private:
        static Method *resolveVirtualMethod(oop thisObject, Method *tagMethod);

    private:
        bool prepareFrame(Frame *frame);

        bool prepareEnvironment();

        void prepareSynchronized(oop thisObject);

        bool fillArguments(const std::vector<ValueType> &argTypes, bool hasThis);

        void finishSynchronized(oop thisObject);

        oop invokeNative(bool hasThis, bool resolveTwice);

        oop invokeJava(bool hasThis, bool resolveTwice);

        oop invokeDynamic(instanceOop MH, const String &descriptor);

        oop callInterpreter();

        inline oop invokeSimple(bool forceNoResolve) {
            if (!prepareEnvironment()) {
                return nullptr;
            }

            bool hasThis = !_method->isStatic();
            bool resolveTwice = forceNoResolve ? false :
                                _method->isAbstract()
                                || (_method->isPublic() && !_method->isFinal());


            if (_method->isNative()) {
                return this->invokeNative(hasThis, resolveTwice);

            } else {
                return this->invokeJava(hasThis, resolveTwice);
            }
        }

        JavaCall(JavaThread *thread, Method *method, Stack *stack);

        JavaCall(JavaThread *thread, Method *method, const std::list<oop> &args);

    public:
        static inline oop withArgs(JavaThread *thread, Method *method,
                                   const std::list<oop> &args, bool forceNoResolve = false) {
            return JavaCall(thread, method, args).invokeSimple(forceNoResolve);
        }

        static inline oop withStack(JavaThread *thread, Method *method,
                                    Stack *stack, bool forceNoResolve = false) {
            return JavaCall(thread, method, stack).invokeSimple(forceNoResolve);
        }

        static inline oop withMethodHandle(JavaThread *thread, Method *invokeExact,
                                           Stack *stack, instanceOop MH,
                                           const String &descriptor) {
            return JavaCall(thread, invokeExact, stack).invokeDynamic(MH, descriptor);
        }
    };
}
