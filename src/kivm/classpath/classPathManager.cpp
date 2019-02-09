//
// Created by kiva on 2018/4/21.
//

#include <compileTimeConfig.h>
#include <kivm/classpath/classPathManager.h>
#include <shared/filesystem.h>
#include <shared/zip.h>

#include <sstream>

#ifndef KIVM_CLASSPATH_DEBUG
#undef D
#define D(...)
#endif

namespace kivm {
    void ClassPathManager::initialize() {
        ClassPathManager *cpm = ClassPathManager::get();

        const char *classpathEnv = getenv("CLASSPATH");
        if (classpathEnv != nullptr) {
            const String &classpath = strings::fromStdString(classpathEnv);
            cpm->addClassPaths(classpath);
        }
    }

    void ClassPathManager::destroy() {
#ifdef KIVM_JAR_CLASS_LOADING
        auto it = _runtimeClassPath.begin();
        while (it != _runtimeClassPath.end()) {
            const ClassPathEntry &entry = *it++;
            if (entry._source == ClassSource::JAR) {
                auto zip = (ZipArchive *) entry._cookie;
                delete zip;
            }
        }
#endif
    }

    ClassPathManager *ClassPathManager::get() {
        static ClassPathManager classPathManager;
        return &classPathManager;
    }

    ClassSearchResult ClassPathManager::searchClass(const String &className) {
        String fileName = strings::replaceAll(className, Global::SLASH, Global::PATH_SEPARATOR);
        fileName.assign(strings::replaceAll(fileName, Global::DOT, Global::PATH_SEPARATOR));

        String tempPath;
        auto it = _runtimeClassPath.begin();

        u1 *buffer = nullptr;
        size_t bufferSize = 0;
        int fd = -1;
        ClassSource classSource = ClassSource::NOT_FOUND;
        String classFile;

        while (it != _runtimeClassPath.end()) {
            const ClassPathEntry &entry = *it++;
            std::wstringstream filePathBuilder;
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
#ifdef KIVM_JAR_CLASS_LOADING
                auto zip = (ZipArchive *) entry._cookie;
                const auto &zipEntry = zip->getEntry(strings::toStdString(tempPath), false);

                if (!zipEntry.isNull() && zipEntry.isFile()) {
                    buffer = (u1 *) zipEntry.readAsBinary();
                    bufferSize = zipEntry.getSize();
                    classSource = buffer != nullptr ? ClassSource::JAR : ClassSource::NOT_FOUND;
                    classFile = entry._path;
                    break;
                }
#else
                D("jar class loading disabled, skipping jars in class path");
#endif
            }
        }

        if (classSource != ClassSource::NOT_FOUND) {
            D("ClassPathManager: found class %S in file: %S",
                (className).c_str(),
                (classFile).c_str());
        }

        return ClassSearchResult(classFile, fd, classSource, buffer, bufferSize);
    }

    void ClassPathManager::addClassPath(const String &path) {
        if (FileSystem::isDirectory(path)) {
            _runtimeClassPath.push_back({ClassSource::DIR, path, nullptr});
            return;
        }

        if (FileSystem::canRead(path)) {
#ifdef KIVM_JAR_CLASS_LOADING
            auto zip = new ZipArchive(path);
            zip->open(ZipArchive::OpenMode::READ_ONLY);
            if (zip->isOpen()) {
                _runtimeClassPath.push_back({ClassSource::JAR, path, zip});
                return;
            }
#else
            D("jar class loading disabled, skipping jar files");
#endif
        }

        D("ClassPathManager: ignoring unrecognized classpath: %S",
            (path).c_str());
    }

    void ClassPathManager::addClassPaths(const String &classpath) {
        const auto &classPathArray = strings::split(classpath, Global::PATH_DELIMITER);
        for (const auto &classPathEntry : classPathArray) {
            D("ClassPathManager: adding classpath: %S",
                (classPathEntry).c_str());
            addClassPath(classPathEntry);
        }
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
#ifdef KIVM_JAR_CLASS_LOADING
            delete[] _buffer;
#endif
        }
    }
}
