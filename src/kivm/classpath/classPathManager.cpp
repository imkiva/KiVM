//
// Created by kiva on 2018/4/21.
//

#include <kivm/classpath/classPathManager.h>
#include <shared/filesystem.h>
#include <shared/zip.h>

#include <sstream>

namespace kivm {
    void ClassPathManager::initialize() {
        ClassPathManager *cpm = ClassPathManager::get();

        const String &classPathEnv = strings::fromStdString(getenv("CLASSPATH"));
        const auto &classPathArray = strings::split(classPathEnv, Global::PATH_DELIMITER);
        for (const auto &classPathEntry : classPathArray) {
            D("ClassPathManager: adding classpath: %s",
              strings::toStdString(classPathEntry).c_str());
            cpm->addClassPath(classPathEntry);
        }
    }

    void ClassPathManager::destroy() {
        auto it = _runtimeClassPath.begin();
        while (it != _runtimeClassPath.end()) {
            const ClassPathEntry &entry = *it++;
            if (entry._source == ClassSource::JAR) {
                auto zip = (ZipArchive *) entry._cookie;
                delete zip;
            }
        }
    }

    ClassPathManager *ClassPathManager::get() {
        static ClassPathManager classPathManager;
        return &classPathManager;
    }

    ClassSearchResult ClassPathManager::searchClass(const String &className) {
        String fileName = strings::replaceAll(className, Global::SLASH, Global::PATH_SEPARATOR);
        fileName.assign(strings::replaceAll(fileName, Global::DOT, Global::PATH_SEPARATOR));

        std::wstringstream filePathBuilder;
        String tempPath;
        auto it = _runtimeClassPath.begin();

        u1 *buffer = nullptr;
        size_t bufferSize = 0;
        int fd = -1;
        ClassSource classSource = ClassSource::NOT_FOUND;
        String classFile;

        while (it != _runtimeClassPath.end()) {
            const ClassPathEntry &entry = *it++;
            filePathBuilder.clear();
            tempPath.clear();

            if (entry._source == ClassSource::DIR) {
                filePathBuilder << entry._path
                                << Global::PATH_SEPARATOR
                                << className
                                << Global::DOT
                                << Global::CLASS_EXTENSION;
            } else if (entry._source == ClassSource::JAR) {
                filePathBuilder << className
                                << Global::DOT
                                << Global::CLASS_EXTENSION;
            }

            tempPath = filePathBuilder.str();

            if (entry._source == ClassSource::DIR) {
                if (FileSystem::canRead(tempPath)) {
                    buffer = (u1 *) FileSystem::createFileMapping(tempPath, &fd, &bufferSize);
                    classSource = buffer != nullptr ? ClassSource::DIR : ClassSource::NOT_FOUND;
                    classFile = std::move(tempPath);
                    break;
                }

            } else if (entry._source == ClassSource::JAR) {
                auto zip = (ZipArchive *) entry._cookie;
                const auto &zipEntry = zip->getEntry(strings::toStdString(tempPath), false);

                if (!zipEntry.isNull() && zipEntry.isFile()) {
                    buffer = (u1 *) zipEntry.readAsBinary();
                    bufferSize = zipEntry.getSize();
                    classSource = buffer != nullptr ? ClassSource::JAR : ClassSource::NOT_FOUND;
                    classFile = entry._path;
                    break;
                }
            }
        }

        if (classSource != ClassSource::NOT_FOUND) {
            D("ClassPathManager: found class %s in file: %s",
              strings::toStdString(className).c_str(),
              strings::toStdString(classFile).c_str());
        }

        return ClassSearchResult(classFile, fd, classSource, buffer, bufferSize);
    }

    void ClassPathManager::addClassPath(const String &path) {
        if (FileSystem::isDirectory(path)) {
            _runtimeClassPath.push_back({ClassSource::DIR, path, nullptr});
            return;
        }

        if (FileSystem::canRead(path)) {
            auto zip = new ZipArchive(path);
            zip->open(ZipArchive::OpenMode::READ_ONLY);
            if (zip->isOpen()) {
                _runtimeClassPath.push_back({ClassSource::JAR, path, zip});
                return;
            }
        }

        D("ClassPathManager: ignoring unrecognized classpath: %s",
          strings::toStdString(path).c_str());
    }

    ClassSearchResult::ClassSearchResult(const String &file, int fd, ClassSource source,
                                         u1 *buffer, size_t bufferSize)
        : _file(file), _fd(fd), _source(source),
          _buffer(buffer), _bufferSize(bufferSize) {
    }

    void ClassSearchResult::closeResource() const {
        if (_source == ClassSource::DIR) {
            FileSystem::destroyFileMapping(_buffer, _fd, _bufferSize);
        } else if (_source == ClassSource::JAR) {
            delete[] _buffer;
        }
    }
}
