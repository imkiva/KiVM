//
// Created by kiva on 2018/4/21.
//

#include <kivm/classpath/classPath.h>
#include <shared/filesystem.h>
#include <shared/zip.h>

namespace kivm {
    ClassPathManager *ClassPathManager::get() {
        static ClassPathManager classPathManager;
        return &classPathManager;
    }

    ClassSearchResult ClassPathManager::searchClass(const String &className) {
        String fileName = strings::replaceAll(className, Global::SLASH, Global::PATH_SEPARATOR);
        fileName.assign(strings::replaceAll(fileName, Global::DOT, Global::PATH_SEPARATOR));

        return {ClassSource::NOT_FOUND, nullptr, 0};
    }

    void ClassPathManager::addClassPath(const String &path) {
        if (FileSystem::isDirectory(path)) {
            _runtimeClassPath.push_back({ClassSource::DIR, path, nullptr});
            return;
        }

        if (FileSystem::canRead(path)) {
            auto zip = new ZipArchive(path);
            zip->open(ZipArchive::OpenMode::READ_ONLY);
            _runtimeClassPath.push_back({ClassSource::JAR, path, zip});
        }

        D("ClassPathManager: ignoring unrecognized classpath: %s",
          strings::toStdString(path).c_str());
    }
}
