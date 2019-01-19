//
// Created by kiva on 2019/1/19.
//

#include "mmap.h"

#if defined(KIVM_PLATFORM_WINDOWS)

void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset) {
    size_t len;
    struct stat st{};
    uint64_t o = offset;
    uint32_t l = o & 0xFFFFFFFF;
    uint32_t h = (o >> 32) & 0xFFFFFFFF;

    if (!fstat(fd, &st)) {
        len = (size_t) st.st_size;
    } else {
        fprintf(stderr,"mmap: could not determine filesize");
        exit(1);
    }

    if ((length + offset) > len) {
        length = len - offset;
    }

    if (!(flags & MAP_PRIVATE)) {
        fprintf(stderr,"Invalid usage of mmap when built with USE_WIN32_MMAP");
        exit(1);
    }

    HANDLE hmap = CreateFileMapping((HANDLE)_get_osfhandle(fd), 0, PAGE_WRITECOPY, 0, 0, 0);

    if (!hmap) {
        return MAP_FAILED;
    }

    void *temp = MapViewOfFileEx(hmap, FILE_MAP_COPY, h, l, length, start);

    if (!CloseHandle(hmap))
        fprintf(stderr,"unable to close file mapping handle\n");
    return temp ? temp : MAP_FAILED;
}


int munmap(void *start, size_t length) {
    return !UnmapViewOfFile(start);
}

#endif
