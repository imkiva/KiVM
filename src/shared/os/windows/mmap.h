//
// Created by kiva on 2019/1/19.
//
// This file provides an equivalent of the POSIX mmap() function for use in
// Windows; the Windows API lacks this function.
//
//

#pragma once

#include <compileTimeConfig.h>

#if defined(KIVM_PLATFORM_WINDOWS)

#include <windows.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>
#include <io.h>

/* copied from sys/mman.h */
#define PROT_READ  1
#define PROT_WRITE 2
#define MAP_FAILED ((void*)-1)

#define MAP_SHARED	0x01		/* Share changes.  */
#define MAP_PRIVATE	0x02		/* Changes are private.  */

void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset);

int munmap(void *start, size_t length);

#endif
