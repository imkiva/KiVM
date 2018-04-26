cmake_minimum_required(VERSION 3.2)
project(KivaVM)

set(CMAKE_MODULE_PATH "${CMCMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake")

include(CheckIncludeFiles)
include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)
include(CheckCSourceCompiles)

#### Check c++11
check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if (COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif (COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else ()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif ()
set(CMAKE_CXX_STANDARD 11)

#### Check c11
check_c_compiler_flag("-std=c11" COMPILER_SUPPORTS_C11)
if (COMPILER_SUPPORTS_CXX11)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
else ()
    message(STATUS "The compiler ${CMAKE_C_COMPILER} has no C11 support. Please use a different C compiler.")
endif ()
set(CMAKE_C_STANDARD 11)

if (DEFINED ENV{KIVM_DEBUG})
    add_definitions(-DKIVM_DEBUG)
endif ()

#### Check platform
if (WIN32)
    add_definitions(-DKIVM_PLATFORM_WINDOWS)
    aux_source_directory(src/shared/platform/windows KIVM_PLATFORM_SRC)
elseif (UNIX)
    add_definitions(-DKIVM_PLATFORM_UNIX)
    aux_source_directory(src/shared/platform/unix KIVM_PLATFORM_SRC)
else ()
    message(STATUS "Cannot detect current system, disabling platform related features.")
endif ()

#### libffi
find_package(FFI REQUIRED)
include_directories(${FFI_INCLUDE_DIRS})
link_directories(${FFI_LIBRARIES})

#### zlib
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR})
set(CMAKE_REQUIRED_INCLUDES ${ZLIB_INCLUDE_DIR})
if (ZLIB_VERSION_STRING VERSION_LESS "1.1.2")
    message(FATAL_ERROR "-- zlib version too old, please install at least v1.1.2")
endif (ZLIB_VERSION_STRING VERSION_LESS "1.1.2")

#### libzip
check_include_files(zip.h HAVE_ZIP_H)
check_c_source_compiles("#include <zip.h>
int main(int argc, char *argv[]) { zip_flags_t flag = 0; }" HAVE_ZIP_FLAGS_T)
check_c_source_compiles("#include <zip.h>
int main(int argc, char *argv[]) { unsigned int flag = ZIP_FL_ENC_GUESS; }" HAVE_ZIP_FL_ENC_GUESS)

if (NOT HAVE_ZIP_H)
    message(FATAL_ERROR "-- libzip not found")
endif ()


#### post configuration
ADD_DEFINITIONS("-DHAVE_CONFIG_H")
configure_file(${CMAKE_SOURCE_DIR}/cmake/cmake-config.h.in ${CMAKE_BINARY_DIR}/compileTimeConfig.h)

#### kivm
include_directories(${CMAKE_BINARY_DIR})
include_directories(include)

set(SOURCE_FILES
        include/kivm/classfile/classFile.h
        include/kivm/kivm.h
        include/kivm/bytecode/interpreter.h
        include/kivm/oop/oop.h
        include/kivm/oop/klass.h
        include/kivm/oop/instanceKlass.h
        include/shared/monitor.h
        include/shared/lock.h
        include/shared/string.h
        include/kivm/classfile/constantPool.h
        include/shared/types.h
        include/kivm/classfile/classFileStream.h
        include/kivm/classfile/classFileParser.h
        include/kivm/classfile/attributeInfo.h
        include/kivm/classpath/classLoader.h
        include/kivm/oop/method.h
        include/kivm/oop/field.h
        include/kivm/oop/mirrorKlass.h
        include/kivm/oop/oopfwd.h
        include/kivm/oop/instanceOop.h
        include/kivm/oop/primitiveOop.h
        include/kivm/oop/mirrorOop.h
        include/kivm/oop/arrayKlass.h
        include/kivm/oop/arrayOop.h
        include/kivm/runtime/slot.h
        include/kivm/runtime/stack.h
        include/kivm/runtime/frame.h
        include/kivm/oop/reflectionSupport.h
        include/kivm/runtime/thread.h
        include/kivm/oop/helper.h
        include/kivm/runtime/runtimeConfig.h
        include/kivm/bytecode/bytecodes.h
        include/kivm/bytecode/execution.h
        include/kivm/native/java_lang_Class.h
        include/kivm/native/classNames.h
        include/kivm/native/java_lang_Thread.h
        include/kivm/native/java_lang_String.h
        include/kivm/classpath/system.h
        include/kivm/bytecode/codeBlob.h
        include/kivm/runtime/constantPool.h
        include/kivm/bytecode/invocationContext.h
        include/kivm/runtime/nativeMethodPool.h
        include/kivm/jni/jni_md.h
        include/kivm/jni/jni.h
        include/kivm/jni/jniJavaVM.h
        include/kivm/jni/jniEnv.h
        include/kivm/memory/collectedHeap.h
        include/kivm/memory/markSweep.h
        include/kivm/memory/universe.h
        include/kivm/memory/heapRegion.h
        include/shared/zip/libzippp.h
        include/kivm/classpath/classPathManager.h
        include/shared/filesystem.h
        include/shared/zip.h
        src/kivm/oop/oopBase.cpp
        src/kivm/classfile/classFileStream.cpp
        src/kivm/oop/oop.cpp
        src/kivm/classfile/constantPool.cpp
        src/shared/string.cpp
        src/kivm/classfile/classFileParser.cpp
        src/kivm/classfile/classFile.cpp
        src/kivm/classfile/attributeInfo.cpp
        src/kivm/oop/klass.cpp
        src/kivm/classpath/classLoader.cpp
        src/kivm/oop/instanceKlass.cpp
        src/kivm/oop/method.cpp
        src/kivm/bytecode/interpreter.cpp
        src/kivm/oop/field.cpp
        src/kivm/classpath/baseClassLoader.cpp
        src/kivm/oop/mirrorKlass.cpp
        src/kivm/oop/instanceOop.cpp
        src/kivm/oop/primitiveOop.cpp
        src/kivm/oop/mirrorOop.cpp
        src/kivm/oop/arrayOop.cpp
        src/kivm/oop/arrayKlass.cpp
        src/kivm/runtime/stack.cpp
        src/kivm/runtime/frame.cpp
        src/kivm/runtime/thread.cpp
        src/kivm/runtime/runtimeConfig.cpp
        src/kivm/bytecode/execution.cpp
        src/kivm/native/java_lang_Class.cpp
        src/kivm/runtime/init.cpp
        src/kivm/classpath/system.cpp
        src/kivm/native/java_lang_String.cpp
        src/kivm/runtime/constantPool.cpp
        src/kivm/bytecode/resolver.cpp
        src/kivm/bytecode/invocationContext.cpp
        src/kivm/bytecode/nativeInvocationContext.cpp
        src/kivm/runtime/nativeMethodPool.cpp
        src/kivm/native/java_lang_Thread.cpp
        src/kivm/jni/jniGlobal.cpp
        src/kivm/jni/jniJavaVM.cpp
        src/kivm/kivm.cpp
        src/kivm/jni/jniEnv.cpp
        src/kivm/bytecode/executionInvoke.cpp
        src/kivm/bytecode/javaInvocationContext.cpp
        src/kivm/memory/markSweep.cpp
        src/kivm/memory/universe.cpp
        src/kivm/native/java_lang_System.cpp
        src/shared/zip/libzippp.cpp
        src/kivm/classpath/classPathManager.cpp
        src/shared/filesystem.cpp
        src/kivm/native/java_lang_Object.cpp
        src/kivm/native/java_lang_Float.cpp
        src/kivm/native/java_lang_Double.cpp
        src/kivm/native/sun_misc_VM.cpp
        src/kivm/bytecode/virtualMethodResolver.cpp)


#### libkivm
add_library(kivm SHARED ${SOURCE_FILES} ${KIVM_PLATFORM_SRC})
target_link_libraries(kivm ffi)
IF (UNIX)
    target_link_libraries(kivm pthread)
    target_link_libraries(kivm dl)
    target_link_libraries(kivm z)
    target_link_libraries(kivm zip)
ENDIF ()

#### Executables
add_executable(java src/bin/java.cpp)
target_link_libraries(java kivm)

#### Tests
add_executable(test_stack-and-frame tests/stack-and-locals.cpp)
target_link_libraries(test_stack-and-frame kivm)

add_executable(bench-allocation tests/bench-allocation.cpp)
target_link_libraries(bench-allocation kivm)

#### CovScript extension
if (DEFINED ENV{CS_SRC})
    set(CS_SRC $ENV{CS_SRC})
    include_directories(${CS_SRC}/include)
    add_library(covscript-kivm SHARED
            ext/covscript-kivm.cpp
            ext/extension-helper.h)
    target_link_libraries(covscript-kivm kivm)
    set_target_properties(covscript-kivm PROPERTIES OUTPUT_NAME "kivm")
    set_target_properties(covscript-kivm PROPERTIES PREFIX "")
    set_target_properties(covscript-kivm PROPERTIES SUFFIX ".cse")
endif ()

