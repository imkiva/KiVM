//
// Created by kiva on 2018/3/25.
//

#include <kivm/method.h>
#include <kivm/runtime/thread.h>
#include <kivm/runtime_config.h>
#include <kivm/bytecode/interpreter.h>

namespace kivm {
    Thread::Thread(Method *method, const std::list<oop> &args)
            : _frames(RuntimeConfig::get().threadInitialStackSize),
              _method(method), _args(args),
              _java_thread_object(nullptr), _native_thread(nullptr),
              _pc(nullptr), _state(ThreadState::RUNNING) {
    }

    void Thread::create(instanceOop java_thread) {
        this->_java_thread_object = java_thread;
        this->_native_thread = new std::thread([this] {
            this->start();
        });
        this->thread_lunched();
    }

    void Thread::thread_lunched() {
        // Do nothing.
    }

    Thread::~Thread() = default;


    JavaMainThread::JavaMainThread()
            : JavaThread(nullptr, {}) {
    }

    void JavaMainThread::start() {
        // TODO: find main(String[]) method and build arg list

        // OK, call main() with args
//        this->_method = main_method;
//        this->_args = main_args;
        JavaThread::start();
    }

    void JavaMainThread::thread_lunched() {
        // Start the first app thread to run main(String[])
        this->_native_thread->join();

        // Then, let's wait for all app threads to finish
        for (;;) {
            int threads = Threads::get_app_thread_count_locked();
            assert(threads >= 0);

            if (threads == 0) {
                break;
            }

            sched_yield();
        }
    }

    JavaThread::JavaThread(Method *method, const std::list<oop> &args)
            : Thread(method, args) {
    }

    void JavaThread::start() {
        // Just call it
        run_method(_method, _args);
    }

    oop JavaThread::run_method(Method *method, const std::list<oop> &args) {
        Frame method_frame(method->get_max_locals(), method->get_max_stack());
        method_frame._method = method;

        u8 *pc_before_call = this->_pc;
        this->_frames.push(&method_frame);
        oop result = ByteCodeInterpreter::interp(this);
        this->_frames.pop();

        this->_pc = pc_before_call;
        return result;
    }
}
