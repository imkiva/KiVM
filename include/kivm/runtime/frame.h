//
// Created by kiva on 2018/3/23.
//
#pragma once

#include <kivm/runtime/stack.h>
#include <cassert>

namespace kivm {
    class Method;

    class Frame {
        friend class FrameList;

    private:
        Frame *_previous;
        Method *_method;

        bool _native_frame;
        bool _exception_occurred;
        u4 _return_pc;

        Locals _locals;
        Stack _stack;

    public:
        Frame(int maxLocals, int maxStacks);

        Method *getMethod() {
            return _method;
        }

        bool isNativeFrame() const {
            return _native_frame;
        }

        bool isExceptionOccurred() const {
            return _exception_occurred;
        }

        Locals &getLocals() {
            return _locals;
        }

        Stack &getStack() {
            return _stack;
        }

        u4 getReturnPc() const {
            return this->_return_pc;
        }

        void setMethod(Method *_method) {
            this->_method = _method;
        }

        void setNativeFrame(bool _native_frame) {
            this->_native_frame = _native_frame;
        }

        void setExceptionOccurred(bool _exception_occurred) {
            this->_exception_occurred = _exception_occurred;
        }

        void setReturnPc(u4 _return_pc) {
            this->_return_pc = _return_pc;
        }
    };

    struct FrameList {
    private:
        int _max_frames;
        int _size;
        Frame *_current;

    public:
        explicit FrameList(int maxFrames);

        inline void push(Frame *frame) {
            if (_size >= _max_frames) {
                // TODO: throw java.lang.StackOverflowException
                PANIC("java.lang.StackOverflowException");
            }

            frame->_previous = _current;
            _current = frame;
            ++_size;
        }

        inline Frame *pop() {
            if (_size == 0 || _current == nullptr) {
                // TODO: throw java.lang.StackOverflowException
                PANIC("java.lang.StackOverflowException");
            }

            Frame *current = _current;
            _current = _current->_previous;
            --_size;

            // this frame is not longer belong to this list.
            current->_previous = nullptr;
            return current;
        }

        inline Frame *getCurrentFrame() const {
            if (_size == 0 || _current == nullptr) {
                PANIC("FrameList is empty");
            }

            return _current;
        }

        inline int getSize() const {
            return this->_size;
        }
    };
}
