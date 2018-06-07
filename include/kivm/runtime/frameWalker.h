//
// Created by kiva on 2018/6/7.
//
#pragma once

#include <kivm/runtime/frame.h>
#include <kivm/runtime/javaThread.h>

namespace kivm {
    class FrameIterator : public std::iterator<std::input_iterator_tag, Frame *> {
    private:
        Frame *_start;
        Frame *_current;

    public:
        explicit FrameIterator(Frame *start)
            : _start(start), _current(start) {
        }

        FrameIterator &operator=(const FrameIterator &rhs) {
            this->_start = rhs._start;
            this->_current = rhs._current;
            return *this;
        }

        bool operator==(const FrameIterator &rhs) const {
            return _current == rhs._current;
        }

        bool operator!=(const FrameIterator &rhs) const {
            return !(rhs == *this);
        }

        FrameIterator &operator++() {
            if (_current != nullptr) {
                _current = _current->getPrevious();
            }
            return *this;
        }

        FrameIterator &operator--() {
            return *this;
        }

        Frame *operator*() const {
            return _current;
        }

        Frame *operator->() const {
            return _current;
        }
    };

    class FrameWalker {
    private:
        FrameIterator _iterator;
        int _size;

    public:
        explicit FrameWalker(JavaThread *thread)
            : _iterator(thread->_frames.getCurrentFrame()),
              _size(thread->_frames.getSize()) {
        }

        int getSize() const {
            return _size;
        }

        FrameIterator begin() {
            return _iterator;
        }
    };
}
