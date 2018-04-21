//
// Created by kiva on 2018/4/21.
//
#include <kivm/kivm.h>

namespace kivm {
    class FileSystem {
    public:
        static bool isDirectory(const String &path);

        static bool canRead(const String &path);
    };
}
