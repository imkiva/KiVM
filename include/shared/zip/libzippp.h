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
#pragma once

#ifdef KIVM_JAR_CLASS_LOADING

#include <cstdio>
#include <string>
#include <vector>
#include <kivm/kivm.h>

//defined in libzip
struct zip;

#define DIRECTORY_SEPARATOR '/'
#define IS_DIRECTORY(str) ((str).length()>0 && (str)[(str).length()-1]==DIRECTORY_SEPARATOR)
#define DEFAULT_CHUNK_SIZE 524288

//standard unsigned int
typedef unsigned int uint;

#ifdef WIN32
typedef long long libzippp_int64;
        typedef unsigned long long libzippp_uint64;
        typedef unsigned short libzippp_uint16;

        //special declarations for windows to use libzippp from a DLL
#define SHARED_LIBRARY_EXPORT __declspec(dllexport)
#define SHARED_LIBRARY_IMPORT __declspec(dllimport)
#else
//standard ISO c++ does not support long long
typedef long int libzippp_int64;
typedef unsigned long int libzippp_uint64;
typedef unsigned short libzippp_uint16;

#define SHARED_LIBRARY_EXPORT
#define SHARED_LIBRARY_IMPORT
#endif

#ifdef LIBZIPPP_EXPORTS
#define LIBZIPPP_INTERNAL
#define LIBZIPPP_API SHARED_LIBRARY_EXPORT
#else
#define LIBZIPPP_API SHARED_LIBRARY_IMPORT
#endif

// special return code for libzippp
#define LIBZIPPP_OK (0)
#define LIBZIPPP_ERROR_NOT_OPEN (-1)
#define LIBZIPPP_ERROR_NOT_ALLOWED (-2)
#define LIBZIPPP_ERROR_INVALID_ENTRY (-3)
#define LIBZIPPP_ERROR_INVALID_PARAMETER (-4)
#define LIBZIPPP_ERROR_MEMORY_ALLOCATION (-16)
#define LIBZIPPP_ERROR_FOPEN_FAILURE (-25)
#define LIBZIPPP_ERROR_FREAD_FAILURE (-26)
#define LIBZIPPP_ERROR_OWRITE_FAILURE (-35)
#define LIBZIPPP_ERROR_OWRITE_INDEX_FAILURE (-36)
#define LIBZIPPP_ERROR_UNKNOWN (-99)

namespace libzippp {
    class ZipEntry;

    /**
     * Represents a ZIP archive. This class provides useful methods to handle an archive
     * content. It is simply a wrapper around libzip.
     */
    class LIBZIPPP_API ZipArchive {
    public:

        /**
         * Defines how the zip file must be open.
         * NOT_OPEN is a special mode where the file is not open.
         * READ_ONLY is the basic mode to only read the archive.
         * WRITE will append to an existing archive or create a new one if it does not exist.
         * NEW will create a new archive or erase all the data if a previous one exists.
         */
        enum OpenMode {
            NOT_OPEN,
            READ_ONLY,
            WRITE,
        };

        /**
         * Defines how the reading of the data should be made in the archive.
         * ORIGINAL will read the data of the original archive file, without any change.
         * CURRENT will read the current content of the archive.
         */
        enum State {
            ORIGINAL,
            CURRENT
        };

        /**
         * Creates a new ZipArchive with the given path. If the password is defined, it
         * will be used to read encrypted archive. It won't affect the files added
         * to the archive.
         *
         * http://nih.at/listarchive/libzip-discuss/msg00219.html
         */
        explicit ZipArchive(const kivm::String &zipPath);

        ~ZipArchive(); //commit all the changes if open

        ZipArchive(const ZipArchive &zf) = delete;

        ZipArchive &operator=(const ZipArchive &) = delete;

        /**
         * Return the path of the ZipArchive.
         */
        std::string getPath() const { return path; }

        /**
         * Open the ZipArchive with the given mode. This method will return true if the operation
         * is successful, false otherwise. If the OpenMode is NOT_OPEN an invalid_argument
         * will be thrown. If the archive is already open, this method returns true only if the
         * mode is the same.
         */
        bool open(OpenMode mode = READ_ONLY, bool checkConsistency = false);

        /**
         * Closes the ZipArchive and releases all the resources held by it. If the ZipArchive was
         * not open previously, this method does nothing. If the archive was open in modification
         * and some were done, they will be committed.
         * This method returns LIBZIPPP_OK if the archive was successfully closed, otherwise it
         * returns the value returned by the zip_close() function.
         */
        int close();

        /**
         * Returns true if the ZipArchive is currently open.
         */
        inline bool isOpen() const { return zipHandle != nullptr; }

        /**
         * Returns true if the ZipArchive is open and mutable.
         */
        inline bool isMutable() const { return isOpen() && mode != NOT_OPEN && mode != READ_ONLY; }

        /**
         * Returns the number of entries in this zip file (folders are included).
         * The zip file must be open otherwise LIBZIPPP_ERROR_NOT_OPEN will be returned.
         * If the state is ORIGINAL, then the number entries of the original archive are returned.
         * Any change will not be considered.
         * Note also that the deleted entries does not affect the result of this method
         * with the CURRENT state. For instance, if there are 3 entries and you delete one,
         * this method will still return 3. However, if you add one entry, it will return
         * 4 with the state CURRENT and 3 with the state ORIGINAL.
         * If you wanna know the "real" entries effectively in the archive, you might use
         * the getEntries method.
         */
        libzippp_int64 getNbEntries(State state = CURRENT) const;

        inline libzippp_int64 getEntriesCount(State state = CURRENT) const { return getNbEntries(state); }

        /**
         * Returns all the entries of the ZipArchive. If the state is ORIGINAL, then
         * returns the entries in the original archive, any change will not be considered.
         * The zip file must be open otherwise an empty vector will be returned.
         */
        std::vector<ZipEntry> getEntries(State state = CURRENT) const;

        /**
         * Return true if an entry with the specified name exists. If no such entry exists,
         * then false will be returned. If a directory is searched, the name must end with a '/' !
         * The zip file must be open otherwise false will be returned.
         */
        bool hasEntry(const std::string &name, bool excludeDirectories = false, bool caseSensitive = true,
                      State state = CURRENT) const;

        /**
         * Return the ZipEntry for the specified entry name. If no such entry exists,
         * then a null-ZiPEntry will be returned. If a directory is searched, the name
         * must end with a '/' !
         * The zip file must be open otherwise a null-ZipEntry will be returned.
         */
        ZipEntry getEntry(const std::string &name, bool excludeDirectoryPart = false, bool caseSensitive = true,
                          State state = CURRENT) const;

        /**
         * Return the ZipEntry for the specified index. If the index is out of range,
         * then a null-ZipEntry will be returned.
         * The zip file must be open otherwise a null-ZipEntry will be returned.
         */
        ZipEntry getEntry(libzippp_int64 index, State state = CURRENT) const;

        /**
         * Read the specified ZipEntry of the ZipArchive and returns its content within
         * a char array. If there is an error while reading the entry, then null will be returned.
         * The data must be deleted by the developer once not used anymore. If the asText
         * is set to true, then the returned void* will be ended by a \0 (hence the size of
         * the returned array will be zipEntry.getSize()+1 or size+1 if the latter is specified).
         * The zip file must be open otherwise null will be returned. If the ZipEntry was not
         * created by this ZipArchive, null will be returned.
         */
        void *
        readEntry(const ZipEntry &zipEntry, bool asText = false, State state = CURRENT, libzippp_uint64 size = 0) const;

        /**
         * Read the specified ZipEntry of the ZipArchive and returns its content within
         * a char array. If there is an error while reading the entry, then null will be returned.
         * The data must be deleted by the developer once not used anymore. If the asText
         * is set to true, then the returned void* will be ended by a \0 (hence the size of
         * the returned array will be zipEntry.getSize()+1 or size+1 if the latter is specified).
         * The zip file must be open otherwise null will be returned. If the ZipEntry was not
         * created by this ZipArchive, null will be returned. If the zipEntry does not exist,
         * this method returns NULL:
         */
        void *readEntry(const std::string &zipEntry, bool asText = false, State state = CURRENT,
                        libzippp_uint64 size = 0) const;

        /**
         * Read the specified ZipEntry of the ZipArchive and inserts its content in the provided reference to an already
         * opened std::ofstream, gradually, with chunks of size "chunksize" to reduce memory usage when dealing with big files.
         * The method returns LIBZIPPP_OK if the extraction has succeeded with no problems, LIBZIPPP_ERROR_INVALID_PARAMETER if the
         * ofstream is not opened, LIBZIPPP_ERROR_NOT_OPEN if the archive is not opened, LIBZIPPP_ERROR_INVALID_ENTRY if the zipEntry
         * doesn't belong to the archive, LIBZIPPP_ERROR_FOPEN_FAILURE if zip_fopen_index() has failed, LIBZIPPP_ERROR_MEMORY_ALLOCATION if
         * a memory allocation has failed, LIBZIPPP_ERROR_FREAD_FAILURE if zip_fread() didn't succeed to read data,
         * LIBZIPPP_ERROR_OWRITE_INDEX_FAILURE if the last ofstream operation has failed, LIBZIPPP_ERROR_OWRITE_FAILURE if fread() didn't
         * return the exact amount of requested bytes and -9 if the amount of extracted bytes didn't match the size of the file (unknown error).
         * If the provided chunk size is zero, it will be defaulted to DEFAULT_CHUNK_SIZE (512KB).
         * The method doesn't close the ofstream after the extraction.
         */
        int readEntry(const ZipEntry &zipEntry, std::ofstream &ofOutput, State state = CURRENT,
                      libzippp_uint64 chunkSize = DEFAULT_CHUNK_SIZE) const;

        /**
         * Returns the mode in which the file has been open.
         * If the archive is not open, then NOT_OPEN will be returned.
         */
        inline OpenMode getMode() const { return mode; }

    private:
        std::string path;
        zip *zipHandle;
        OpenMode mode;

        //generic method to create ZipEntry
        ZipEntry createEntry(struct zip_stat *stat) const;
    };

    /**
     * Represents an entry in a zip file.
     * This class is meant to be used by the ZipArchive class.
     */
    class LIBZIPPP_API ZipEntry {
        friend class ZipArchive;

    public:
        /**
         * Creates a new null-ZipEntry. Only a ZipArchive will create a valid ZipEntry
         * usable to read and modify an archive.
         */
        explicit ZipEntry() : zipFile(nullptr), index(0), time(0),
                              size(0), sizeComp(0), crc(0) {}

        ~ZipEntry() = default;

        /**
         * Returns the name of the entry.
         */
        inline const std::string &getName() const { return name; }

        /**
         * Returns the index of the file in the zip.
         */
        inline libzippp_uint64 getIndex() const { return index; }

        /**
         * Returns the timestamp of the entry.
         */
        inline time_t getDate() const { return time; }

        /**
         * Returns the size of the file (not deflated).
         */
        inline libzippp_uint64 getSize() const { return size; }

        /**
         * Returns the size of the inflated file.
         */
        inline libzippp_uint64 getInflatedSize() const { return sizeComp; }

        /**
         * Returns the CRC of the file.
         */
        inline int getCRC() const { return crc; }

        /**
         * Returns true if the entry is a directory.
         */
        inline bool isDirectory() const { return IS_DIRECTORY(name); }

        /**
         * Returns true if the entry is a file.
         */
        inline bool isFile() const { return !isDirectory(); }

        /**
         * Returns true if this entry is null (means no more entry is available).
         */
        inline bool isNull() const { return zipFile == nullptr; }

        /**
         * Read the content of this ZipEntry as text.
         * The returned string will be of size getSize() if the latter is not specified or too big.
         * If the ZipArchive is not open, this method returns an
         * empty string. This method is a wrapper around ZipArchive::readEntry(...).
         */
        std::string readAsText(ZipArchive::State state = ZipArchive::CURRENT, libzippp_uint64 size = 0) const;

        /**
         * Read the content of this ZipEntry as binary.
         * The returned void* will be of size getSize() if the latter is not specified or too big.
         * If the ZipArchive is not open, this method returns NULL.
         * The data must be deleted by the developer once not used anymore.
         * This method is a wrapper around ZipArchive::readEntry(...).
         */
        void *readAsBinary(ZipArchive::State state = ZipArchive::CURRENT, libzippp_uint64 size = 0) const;

        /**
         * Read the specified ZipEntry of the ZipArchive and inserts its content in the provided reference to an already
         * opened std::ofstream, gradually, with chunks of size "chunksize" to reduce memory usage when dealing with big files.
         * The method returns LIBZIPPP_OK if the extraction has succeeded with no problems, LIBZIPPP_ERROR_INVALID_PARAMETER if the
         * ofstream is not opened, LIBZIPPP_ERROR_NOT_OPEN if the archive is not opened, LIBZIPPP_ERROR_INVALID_ENTRY if the zipEntry
         * doesn't belong to the archive, LIBZIPPP_ERROR_FOPEN_FAILURE if zip_fopen_index() has failed, LIBZIPPP_ERROR_MEMORY_ALLOCATION if
         * a memory allocation has failed, LIBZIPPP_ERROR_FREAD_FAILURE if zip_fread() didn't succeed to read data,
         * LIBZIPPP_ERROR_OWRITE_INDEX_FAILURE if the last ofstream operation has failed, LIBZIPPP_ERROR_OWRITE_FAILURE if fread() didn't
         * return the exact amount of requested bytes and -9 if the amount of extracted bytes didn't match the size of the file (unknown error).
         * If the provided chunk size is zero, it will be defaulted to DEFAULT_CHUNK_SIZE (512KB).
         * The method doesn't close the ofstream after the extraction.
         */
        int readContent(std::ofstream &ofOutput, ZipArchive::State state = ZipArchive::CURRENT,
                        libzippp_uint64 chunksize = DEFAULT_CHUNK_SIZE) const;

    private:
        const ZipArchive *zipFile;
        std::string name;
        libzippp_uint64 index;
        time_t time;
        libzippp_uint64 size;
        libzippp_uint64 sizeComp;
        int crc;

        ZipEntry(const ZipArchive *zipFile, const std::string &name, libzippp_uint64 index, time_t time,
                 libzippp_uint16 compMethod, libzippp_uint16 encMethod, libzippp_uint64 size, libzippp_uint64 sizeComp,
                 int crc) :
            zipFile(zipFile), name(name), index(index), time(time), size(size), sizeComp(sizeComp), crc(crc) {}
    };
}

#endif
