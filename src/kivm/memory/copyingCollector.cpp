//
// Created by kiva on 2018/6/4.
//

#include <kivm/memory/copyingHeap.h>

namespace kivm {
    void CopyingHeap::doGarbageCollection() {
        // GC-Roots include:
        // 0. InstanceKlass::_staticFieldValues
        // 1. InstanceKlass::_javaMirror
        // 2. InstanceKlass::_javaLoader
        // 3. InstanceKlass::_runtimePool's Strings
        // 4. instanceOopDesc::_instanceFieldValues
        // 5. arrayOopDesc::_elements
        // 6. JavaThread::_javaThreadObject
        // 7. JavaThread::_exceptionOop
        // 8. JavaThread::_frames[0 ~ the last frame]::_locals
        // 9. JavaThread::_frames[0 ~ the last frame]::_stack

        PANIC("TODO: CopyingHeap::doGarbageCollection()");
    }
}
