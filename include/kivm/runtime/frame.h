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

        friend class CopyingHeap;

    private:
        Frame *_previous;
        Method *_method;

        bool _nativeFrame;
        u4 _returnPc;
        u4 _currentPc;

        Locals _locals;
        Stack _stack;

    public:
        Frame(int maxLocals, int maxStacks);

        Frame *getPrevious() {
            return _previous;
        }

        Method *getMethod() {
            return _method;
        }

        bool isNativeFrame() const {
            return _nativeFrame;
        }

        Locals &getLocals() {
            return _locals;
        }

        Stack &getStack() {
            return _stack;
        }

        u4 getReturnPc() const {
            return this->_returnPc;
        }

        u4 getCurrentPc() const {
            return this->_currentPc;
        }

        void setMethod(Method *method) {
            this->_method = method;
        }

        void setNativeFrame(bool isNativeFrame) {
            this->_nativeFrame = isNativeFrame;
        }

        void setReturnPc(u4 returnPc) {
            this->_returnPc = returnPc;
        }

        void setCurrentPc(u4 currentPc) {
            this->_currentPc = currentPc;
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
