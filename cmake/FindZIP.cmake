# - Try to find libzip
#
# Once done this will define
#  ZIP_FOUND - System has libzip
#  ZIP_INCLUDE_DIRS - The package include directories
#  ZIP_LIBRARIES - The libraries needed to use this package

if (NOT WIN32)
    # try using pkg-config to get the directories and then use these values
    # in the find_path() and find_library() calls
    find_package(PkgConfig QUIET)
    PKG_CHECK_MODULES(PC_LIBZIP QUIET libzip)
endif ()

mark_as_advanced(GNUTLS_INCLUDE_DIR GNUTLS_LIBRARY)

find_library(ZIP_LIBRARY NAMES zip libzip
        PATHS $ENV{ZIP_DIR} ${ZIP_DIR} $ENV{WIN32_ZIP_DIR} ${WIN32_ZIP_DIR} /usr /usr/local /opt/local
        PATH_SUZIPXES lib lib64 x86_64-linux-gnu lib/x86_64-linux-gnu
        HINTS ${PC_LIBZIP_LIBDIR} ${PC_LIBZIP_LIBRARY_DIRS}
        )

find_path(ZIP_INCLUDE_DIR zip.h
        PATHS $ENV{ZIP_DIR} ${ZIP_DIR} $ENV{WIN32_ZIP_DIR} ${WIN32_ZIP_DIR} /usr /usr/local /opt/local
        PATH_SUZIPXES include include/zip include/x86_64-linux-gnu x86_64-linux-gnu
        HINTS ${PC_LIBZIP_INCLUDEDIR} ${PC_LIBZIP_INCLUDE_DIRS}
        )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set ZIP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(ZIP  DEFAULT_MSG
        ZIP_LIBRARY ZIP_INCLUDE_DIR)
mark_as_advanced(ZIP_INCLUDE_DIR ZIP_LIBRARY)

if(ZIP_FOUND)
    set(ZIP_LIBRARIES ${ZIP_LIBRARY})
    set(ZIP_INCLUDE_DIRS ${ZIP_INCLUDE_DIR})
endif()
