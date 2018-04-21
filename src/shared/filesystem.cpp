//
// Created by kiva on 2018/4/21.
//
#include <shared/filesystem.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace kivm {
    bool FileSystem::isDirectory(const String &path) {
        struct stat s{};
        if (stat(strings::toStdString(path).c_str(), &s) == 0) {
            return S_ISDIR(s.st_mode);
        }
        return false;
    }

    bool FileSystem::canRead(const String &path) {
        return access(strings::toStdString(path).c_str(), R_OK) == 0;
    }
}
