//
// Created by kiva on 2018/4/21.
//

#include <compileTimeConfig.h>
#include <shared/string.h>

namespace kivm {
    class FileSystem {
    public:
        static bool isDirectory(const String &path);

        static bool canRead(const String &path);

        static size_t getFileSize(const String &path);

        static void *createFileMapping(const String &path, int *pFd, size_t *pSize);

        static void destroyFileMapping(void *memory, int fd, size_t size);
    };
}
