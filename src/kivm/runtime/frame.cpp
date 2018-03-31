//
// Created by kiva on 2018/3/23.
//
#include <kivm/runtime/frame.h>

namespace kivm {
    Frame::Frame(int maxLocals, int maxStacks)
            : _locals(maxLocals), _stack(maxStacks) {
    }

    FrameList::FrameList(int maxFrames)
            : _max_frames(maxFrames), _size(0), _current(nullptr) {
    }
}
