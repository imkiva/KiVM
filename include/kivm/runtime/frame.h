//
// Created by kiva on 2018/3/23.
//
#pragma once

#include <kivm/runtime/stack.h>
#include <cassert>

namespace kivm {
    struct Frame {
        Frame *_previous;
        Locals _locals;
        Stack _stack;

        Frame(int max_locals, int max_stacks);
    };

    struct FrameList {
        int _max_frames;
        int _size;
        Frame *_current;

        explicit FrameList(int max_frames);

        inline void push(Frame *frame) {
            if (_size >= _max_frames) {
                // TODO: throw java.lang.StackOverflowException
                assert(false);
            }

            frame->_previous = _current;
            _current = frame;
            ++_size;
        }

        inline Frame *pop() {
            if (_size == 0 || _current == nullptr) {
                // TODO: throw java.lang.StackOverflowException
                assert(false);
            }

            Frame *current = _current;
            _current = _current->_previous;
            --_size;

            // this frame is not longer belong to this list.
            current->_previous = nullptr;
            return current;
        }

        inline Frame *current() const {
            if (_size == 0 || _current == nullptr) {
                // TODO: panic()
                assert(false);
            }

            return _current;
        }
    };
}
