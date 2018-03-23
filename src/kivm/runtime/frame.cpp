//
// Created by kiva on 2018/3/23.
//
#include <kivm/runtime/frame.h>

namespace kivm {
    Frame::Frame(int max_locals, int max_stacks)
            : _locals(max_locals), _stack(max_stacks) {
    }

    FrameList::FrameList(int max_frames)
            : _max_frames(max_frames), _size(0), _current(nullptr) {
    }
}
