# - Try to find libffi
#
# Once done this will define
#  FFI_FOUND - System has libffi
#  FFI_INCLUDE_DIRS - The package include directories
#  FFI_LIBRARIES - The libraries needed to use this package

if (NOT WIN32)
    # try using pkg-config to get the directories and then use these values
    # in the find_path() and find_library() calls
    find_package(PkgConfig QUIET)
    PKG_CHECK_MODULES(PC_LIBFFI QUIET libffi)
endif ()

mark_as_advanced(GNUTLS_INCLUDE_DIR GNUTLS_LIBRARY)

find_library(FFI_LIBRARY NAMES ffi libffi
        PATHS $ENV{FFI_DIR} ${FFI_DIR} /usr /usr/local /opt/local
        PATH_SUFFIXES lib lib64 x86_64-linux-gnu lib/x86_64-linux-gnu
        HINTS ${PC_LIBFFI_LIBDIR} ${PC_LIBFFI_LIBRARY_DIRS}
        )

find_path(FFI_INCLUDE_DIR ffi.h
        PATHS $ENV{FFI_DIR} ${FFI_DIR} /usr /usr/local /opt/local
        PATH_SUFFIXES include include/ffi include/x86_64-linux-gnu x86_64-linux-gnu
        HINTS ${PC_LIBFFI_INCLUDEDIR} ${PC_LIBFFI_INCLUDE_DIRS}
        )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FFI_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(FFI  DEFAULT_MSG
        FFI_LIBRARY FFI_INCLUDE_DIR)
mark_as_advanced(FFI_INCLUDE_DIR FFI_LIBRARY)

if(FFI_FOUND)
    set(FFI_LIBRARIES ${FFI_LIBRARY})
    set(FFI_INCLUDE_DIRS ${FFI_INCLUDE_DIR})
endif()
