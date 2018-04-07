//
// Created by kiva on 2018/3/25.
//

#include <kivm/method.h>
#include <kivm/runtime/thread.h>
#include <kivm/runtime/runtimeConfig.h>
#include <kivm/bytecode/interpreter.h>

namespace kivm {
    Thread::Thread(Method *method, const std::list<oop> &args)
        : _frames(RuntimeConfig::get().threadMaxStackSize),
          _method(method), _args(args),
          _java_thread_object(nullptr), _native_thread(nullptr),
          _pc(0), _state(ThreadState::RUNNING) {
    }

    void Thread::create(instanceOop java_thread) {
        this->_java_thread_object = java_thread;
        this->_native_thread = new std::thread([this] {
            if (this->shouldRecordInThreadTable()) {
                D("should_record_in_thread_table == true, recording.");
                Threads::add(this);
            }
            this->start();
        });
        this->onThreadLaunched();
    }

    void Thread::onThreadLaunched() {
        // Do nothing.
    }

    bool Thread::shouldRecordInThreadTable() {
        return true;
    }

    long Thread::getEetop() const {
        return (long) this->_native_thread->native_handle();
    }

    Thread::~Thread() = default;

    JavaThread::JavaThread(Method *method, const std::list<oop> &args)
        : Thread(method, args) {
    }

    void JavaThread::start() {
        // No other threads will join this thread.
        // So it is OK to detach()
        this->_native_thread->detach();

        // A thread must start with an empty frame
        assert(_frames.getSize() == 0);

        // Only one argument(this) in java.lang.Thread#run()
        assert(_args.size() == 1);

        runMethod(_method, _args);

        Threads::threadStateChangeLock().lock();
        this->setThreadState(ThreadState::DIED);
        Threads::threadStateChangeLock().unlock();

        if (this->shouldRecordInThreadTable()) {
            Threads::decAppThreadCountLocked();
        }
    }

    oop JavaThread::runMethod(Method *method, const std::list<oop> &args) {
        D("### JavaThread::runMethod()");
        Frame frame(method->getMaxLocals(), method->getMaxStack());
        D("### Stack is at %p, locals is at %p", &frame.getStack(), &frame.getLocals());

        frame.setMethod(method);
        frame.setReturnPc(this->_pc);
        frame.setNativeFrame(method->isNative());

        this->_frames.push(&frame);
        oop result = ByteCodeInterpreter::interp(this);
        this->_frames.pop();

        this->_pc = frame.getReturnPc();
        return result;
    }
}
