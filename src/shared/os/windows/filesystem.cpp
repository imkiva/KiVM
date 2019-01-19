//
// Created by kiva on 2018/4/21.
//
#include <shared/filesystem.h>

#ifdef KIVM_PLATFORM_WINDOWS

#include <windows.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <tchar.h>
#include "mmap.h"

namespace kivm {
    bool WindowsFileSystem::isDirectory(const String &path) {
        struct stat s{};
        if (stat(strings::toStdString(path).c_str(), &s) == 0) {
            return S_ISDIR(s.st_mode);
        }
        return false;
    }

    bool WindowsFileSystem::canRead(const String &path) {
        int r = access(strings::toStdString(path).c_str(), R_OK);
        return r == 0;
    }

    void *WindowsFileSystem::createFileMapping(const String &path, int *pFd, size_t *pSize) {
        int fd = open(strings::toStdString(path).c_str(), O_RDONLY);
        if (fd < 0) {
            return nullptr;
        }
        size_t size = WindowsFileSystem::getFileSize(path);
        auto m = (jbyte *) mmap(nullptr, size,
                                PROT_READ,
                                MAP_SHARED, fd, 0);
        if (m == MAP_FAILED) {
            return nullptr;
        }
        *pFd = fd;
        *pSize = size;
        return m;
    }

    void WindowsFileSystem::destroyFileMapping(void *memory, int fd, size_t size) {
        if (memory != nullptr) {
            munmap(memory, size);
            close(fd);
        }
    }

    size_t WindowsFileSystem::getFileSize(const String &path) {
        struct stat s{};
        if (stat(strings::toStdString(path).c_str(), &s) == 0) {
            return (size_t) s.st_size;
        }
        return 0;
    }
}

#endif
