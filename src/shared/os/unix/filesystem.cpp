//
// Created by kiva on 2018/4/21.
//
#include <shared/filesystem.h>

#if defined(KIVM_PLATFORM_UNIX) || defined(KIVM_PLATFORM_APPLE)

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <cstring>
#include <cerrno>
#include <fcntl.h>

namespace kivm {
    bool UnixFileSystem::isDirectory(const String &path) {
        struct stat s{};
        if (stat(strings::toStdString(path).c_str(), &s) == 0) {
            return S_ISDIR(s.st_mode);
        }
        return false;
    }

    bool UnixFileSystem::canRead(const String &path) {
        int r = access(strings::toStdString(path).c_str(), R_OK);
        return r == 0;
    }

    void *UnixFileSystem::createFileMapping(const String &path, int *pFd, size_t *pSize) {
        int fd = open(strings::toStdString(path).c_str(), O_RDONLY);
        if (fd < 0) {
            return nullptr;
        }
        size_t size = UnixFileSystem::getFileSize(path);
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

    void UnixFileSystem::destroyFileMapping(void *memory, int fd, size_t size) {
        if (memory != nullptr) {
            munmap(memory, size);
            close(fd);
        }
    }

    size_t UnixFileSystem::getFileSize(const String &path) {
        struct stat s{};
        if (stat(strings::toStdString(path).c_str(), &s) == 0) {
            return (size_t) s.st_size;
        }
        return 0;
    }
}

#endif
