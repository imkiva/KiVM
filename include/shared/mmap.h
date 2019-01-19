//
// Created by kiva on 2019-01-19.
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
#define MAP_FAILED ((void*) -1)

#define MAP_SHARED	0x01		/* Share changes.  */
#define MAP_PRIVATE	0x02		/* Changes are private.  */
#define	MAP_ANON	0x1000	    /* allocated from memory, swap space */
#define	MAP_ANONYMOUS	MAP_ANON

void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset);

int munmap(void *start, size_t length);

#else

#include <sys/mman.h>
#include <fcntl.h>

#endif
