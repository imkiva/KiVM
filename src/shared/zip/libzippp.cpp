/*
  libzippp.h -- exported declarations.
  Copyright (C) 2013 Cédric Tabin

  This file is part of libzippp, a library that wraps libzip for manipulating easily
  ZIP files in C++.
  The author can be contacted on http://www.astorm.ch/blog/index.php?contact

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
  3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.

  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <compileTimeConfig.h>

#ifdef KIVM_JAR_CLASS_LOADING

#include <zip.h>
#include <cerrno>
#include <fstream>
#include <memory>

#include <shared/zip/libzippp.h>
#include <shared/string.h>

#if HAVE_ZIP_FLAGS_T
#define ZIPPP_ZIP_FLAGS_T zip_flags_t
#else
#define ZIPPP_ZIP_FLAGS_T unsigned int
#endif

#if HAVE_ZIP_FL_ENC_GUESS
#define ZIPPP_ZIP_INITIAL_FLAGS ZIP_FL_ENC_GUESS
#else
#define ZIPPP_ZIP_INITIAL_FLAGS 0
#endif

using namespace libzippp;
using namespace std;

string ZipEntry::readAsText(ZipArchive::State state, libzippp_uint64 size) const {
    auto *content = (char *) zipFile->readEntry(*this, true, state, size);
    if (content == nullptr) { return string(); } //happen if the ZipArchive has been closed

    libzippp_uint64 maxSize = getSize();
    string str(content, (size == 0 || size > maxSize ? maxSize : size));
    delete[] content;
    return str;
}

void *ZipEntry::readAsBinary(ZipArchive::State state, libzippp_uint64 size) const {
    return zipFile->readEntry(*this, false, state, size);
}

int ZipEntry::readContent(std::ofstream &ofOutput, ZipArchive::State state, libzippp_uint64 chunksize) const {
    return zipFile->readEntry(*this, ofOutput, state, chunksize);
}

ZipArchive::ZipArchive(const kivm::String &zipPath)
    : path(kivm::strings::toStdString(zipPath)),
      zipHandle(nullptr),
      mode(NOT_OPEN) {
}

ZipArchive::~ZipArchive() {
    close(); /* discard ??? */
}

bool ZipArchive::open(OpenMode om, bool checkConsistency) {
    if (isOpen()) { return om == mode; }

    int zipFlag = 0;
    if (om == READ_ONLY) { zipFlag = 0; }
    else if (om == WRITE) { zipFlag = ZIP_CREATE; }
    else { return false; }

    if (checkConsistency) {
        zipFlag = zipFlag | ZIP_CHECKCONS;
    }

    int errorFlag = 0;
    zipHandle = zip_open(path.c_str(), zipFlag, &errorFlag);

    //error during opening of the file
    if (errorFlag != ZIP_ER_OK) {
        zipHandle = nullptr;
        return false;
    }

    if (zipHandle != nullptr) {
        mode = om;
        return true;
    }

    return false;
}

int ZipArchive::close() {
    if (isOpen()) {
        int result = zip_close(zipHandle);
        zipHandle = nullptr;
        mode = NOT_OPEN;

        if (result != 0) { return result; }
    }

    return LIBZIPPP_OK;
}

libzippp_int64 ZipArchive::getNbEntries(State state) const {
    if (!isOpen()) { return LIBZIPPP_ERROR_NOT_OPEN; }

    unsigned int flag = state == ORIGINAL ? ZIP_FL_UNCHANGED : 0;
    return zip_get_num_entries(zipHandle, flag);
}

ZipEntry ZipArchive::createEntry(struct zip_stat *stat) const {
    string name(stat->name);
    libzippp_uint64 index = stat->index;
    libzippp_uint64 size = stat->size;
    libzippp_uint16 compMethod = stat->comp_method;
    libzippp_uint16 encMethod = stat->encryption_method;
    libzippp_uint64 sizeComp = stat->comp_size;
    int crc = stat->crc;
    time_t time = stat->mtime;

    return ZipEntry(this, name, index, time, compMethod, encMethod, size, sizeComp, crc);
}

vector<ZipEntry> ZipArchive::getEntries(State state) const {
    if (!isOpen()) { return vector<ZipEntry>(); }

    struct zip_stat stat{};
    zip_stat_init(&stat);

    vector<ZipEntry> entries;
    unsigned int flag = state == ORIGINAL ? ZIP_FL_UNCHANGED : 0;
    libzippp_int64 nbEntries = getNbEntries(state);
    for (libzippp_int64 i = 0; i < nbEntries; ++i) {
        int result = zip_stat_index(zipHandle, (zip_uint64_t) i, flag, &stat);
        if (result == 0) {
            ZipEntry entry = createEntry(&stat);
            entries.push_back(entry);
        } else {
            // Skip
        }
    }
    return entries;
}

bool ZipArchive::hasEntry(const string &name, bool excludeDirectories, bool caseSensitive, State state) const {
    if (!isOpen()) { return false; }

    ZIPPP_ZIP_FLAGS_T flags = ZIPPP_ZIP_INITIAL_FLAGS;
    if (excludeDirectories) { flags = flags | ZIP_FL_NODIR; }
    if (!caseSensitive) { flags = flags | ZIP_FL_NOCASE; }
    if (state == ORIGINAL) { flags = flags | ZIP_FL_UNCHANGED; }

    libzippp_int64 index = zip_name_locate(zipHandle, name.c_str(), flags);
    return index >= 0;
}

ZipEntry ZipArchive::getEntry(const string &name, bool excludeDirectoryPart, bool caseSensitive, State state) const {
    if (isOpen()) {
        ZIPPP_ZIP_FLAGS_T flags = ZIPPP_ZIP_INITIAL_FLAGS;
        if (excludeDirectoryPart) { flags = flags | ZIP_FL_NODIR; }
        if (!caseSensitive) { flags = flags | ZIP_FL_NOCASE; }
        if (state == ORIGINAL) { flags = flags | ZIP_FL_UNCHANGED; }

        libzippp_int64 index = zip_name_locate(zipHandle, name.c_str(), flags);
        if (index >= 0) {
            return getEntry(index);
        }
    }

    // name not found
    return ZipEntry();
}

ZipEntry ZipArchive::getEntry(libzippp_int64 index, State state) const {
    if (isOpen()) {
        struct zip_stat stat{};
        zip_stat_init(&stat);
        unsigned int flag = state == ORIGINAL ? ZIP_FL_UNCHANGED : 0;
        int result = zip_stat_index(zipHandle, (zip_uint64_t) index, flag, &stat);
        if (result == 0) {
            return createEntry(&stat);
        }
    }

    // index not found / invalid index
    return ZipEntry();
}

void *ZipArchive::readEntry(const ZipEntry &zipEntry, bool asText, State state, libzippp_uint64 size) const {
    if (!isOpen()) { return nullptr; }
    if (zipEntry.zipFile != this) { return nullptr; }

    unsigned int flag = state == ORIGINAL ? ZIP_FL_UNCHANGED : 0;
    struct zip_file *zipFile = zip_fopen_index(zipHandle, zipEntry.getIndex(), flag);
    if (zipFile) {
        libzippp_uint64 maxSize = zipEntry.getSize();
        libzippp_uint64 uisize = size == 0 || size > maxSize ? maxSize : size;

        auto *data = new char[uisize + (asText ? 1 : 0)];

        libzippp_int64 result = zip_fread(zipFile, data, uisize);
        zip_fclose(zipFile);

        //avoid buffer copy
        if (asText) { data[uisize] = '\0'; }

        auto isize = (libzippp_int64) uisize;
        if (result == isize) {
            return data;
        } else { //unexpected number of bytes read => crash ?
            delete[] data;
        }
    } else {
        //unable to read the entry => crash ?
    }

    return nullptr;
}

void *ZipArchive::readEntry(const string &zipEntry, bool asText, State state, libzippp_uint64 size) const {
    ZipEntry entry = getEntry(zipEntry);
    if (entry.isNull()) { return nullptr; }
    return readEntry(entry, asText, state, size);
}

int
ZipArchive::readEntry(const ZipEntry &zipEntry, std::ofstream &ofOutput, State state, libzippp_uint64 chunkSize) const {
    if (!ofOutput.is_open()) { return LIBZIPPP_ERROR_INVALID_PARAMETER; }
    if (!isOpen()) { return LIBZIPPP_ERROR_NOT_OPEN; }
    if (zipEntry.zipFile != this) { return LIBZIPPP_ERROR_INVALID_ENTRY; }

    int iRes = LIBZIPPP_OK;
    unsigned int flag = state == ORIGINAL ? ZIP_FL_UNCHANGED : 0;
    struct zip_file *zipFile = zip_fopen_index(zipHandle, zipEntry.getIndex(), flag);
    if (zipFile) {
        libzippp_uint64 maxSize = zipEntry.getSize();
        if (!chunkSize) { chunkSize = DEFAULT_CHUNK_SIZE; } // use the default chunk size (512K) if not specified by the user

        if (maxSize < chunkSize) {
            auto *data = new char[maxSize];
            libzippp_int64 result = zip_fread(zipFile, data, maxSize);
            if (result > 0) {
                if (result != static_cast<libzippp_int64>(maxSize)) {
                    iRes = LIBZIPPP_ERROR_OWRITE_INDEX_FAILURE;
                } else {
                    ofOutput.write(data, maxSize);
                    if (!ofOutput) { iRes = LIBZIPPP_ERROR_OWRITE_FAILURE; }
                }
            } else {
                iRes = LIBZIPPP_ERROR_FREAD_FAILURE;
            }
            delete[] data;
        } else {
            libzippp_uint64 uWrittenBytes = 0;
            libzippp_int64 result = 0;
            auto *outerData = new char[chunkSize];
            libzippp_uint64 nbChunks = maxSize / chunkSize;
            for (int uiChunk = 0; uiChunk < nbChunks; ++uiChunk) {
                result = zip_fread(zipFile, outerData, chunkSize);
                if (result > 0) {
                    if (result != static_cast<libzippp_int64>(chunkSize)) {
                        iRes = LIBZIPPP_ERROR_OWRITE_INDEX_FAILURE;
                        break;
                    } else {
                        ofOutput.write(outerData, chunkSize);
                        if (!ofOutput) {
                            iRes = LIBZIPPP_ERROR_OWRITE_FAILURE;
                            break;
                        }
                        uWrittenBytes += result;
                    }
                } else {
                    iRes = LIBZIPPP_ERROR_FREAD_FAILURE;
                    break;
                }
            }
            delete[] outerData;

            libzippp_uint64 leftOver = maxSize % chunkSize;
            if (iRes == 0 && leftOver > 0) {
                auto *leftOverData = new char[leftOver];
                result = zip_fread(zipFile, leftOverData, leftOver);
                if (result > 0) {
                    if (result != static_cast<libzippp_int64>(leftOver)) {
                        iRes = LIBZIPPP_ERROR_OWRITE_INDEX_FAILURE;
                    } else {
                        ofOutput.write(leftOverData, leftOver);
                        if (!ofOutput) {
                            iRes = LIBZIPPP_ERROR_OWRITE_FAILURE;
                        } else {
                            uWrittenBytes += result;
                            if (uWrittenBytes != maxSize) {
                                iRes = LIBZIPPP_ERROR_UNKNOWN; // shouldn't occur but let's be careful
                            }
                        }
                    }
                } else {
                    iRes = LIBZIPPP_ERROR_FREAD_FAILURE;
                }
                delete[] leftOverData;
            }
        }
        zip_fclose(zipFile);
    } else {
        iRes = LIBZIPPP_ERROR_FOPEN_FAILURE;
    }
    return iRes;
}

#endif
