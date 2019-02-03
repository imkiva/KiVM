//
// Created by kiva on 2018/3/23.
//
#pragma once

#include <kivm/runtime/stack.h>
#include <cassert>

namespace kivm {
    class Method;

    class Frame final {
        friend class FrameList;

        friend class CopyingHeap;

    private:
        Frame *_previous = nullptr;
        Method *_method = nullptr;

        bool _nativeFrame = false;
        volatile bool _exceptionThrownHere = false;
        u4 _returnPc;

        Locals _locals;
        Stack _stack;

    public:
        Frame(int maxLocals, int maxStacks);

        inline Frame *getPrevious() {
            return _previous;
        }

        inline Method *getMethod() {
            return _method;
        }

        inline bool isNativeFrame() const {
            return _nativeFrame;
        }

        inline Locals &getLocals() {
            return _locals;
        }

        inline Stack &getStack() {
            return _stack;
        }

        inline u4 getReturnPc() const {
            return this->_returnPc;
        }

        inline bool isExceptionThrownHere() const {
            return this->_exceptionThrownHere;
        }

        inline void setMethod(Method *method) {
            this->_method = method;
        }

        inline void setNativeFrame(bool isNativeFrame) {
            this->_nativeFrame = isNativeFrame;
        }

        inline void setReturnPc(u4 returnPc) {
            this->_returnPc = returnPc;
        }

        inline void setExceptionThrownHere(bool here) {
            this->_exceptionThrownHere = here;
        }
    };

    struct FrameList {
        friend class CopyingHeap;

    private:
        int _max_frames;
        int _size;
        Frame *_current;

    public:
        explicit FrameList(int maxFrames);

        inline void push(Frame *frame) {
            assert(_size >= 0 && _size < _max_frames);

            frame->_previous = _current;
            _current = frame;
            ++_size;
        }

        inline Frame *pop() {
            assert(_size > 0 && _current != nullptr);

            Frame *current = _current;
            _current = _current->_previous;
            --_size;

            // this frame is not longer belong to this list.
            current->_previous = nullptr;
            return current;
        }

        inline Frame *getCurrentFrame() const {
            assert(_size > 0 && _current != nullptr);
            return _current;
        }

        inline int getSize() const {
            return this->_size;
        }

        inline int getMaxFrames() const {
            return _max_frames;
        }
    };
}
