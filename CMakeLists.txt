cmake_minimum_required(VERSION 3.8)
project(KivaVM)

set(CMAKE_CXX_STANDARD 11)
if (DEFINED ENV{CS_SRC})
    set(CMAKE_CXX_FLAGS -DKIVM_DEBUG)
endif ()

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
        include/kivm/classLoader.h
        include/kivm/method.h
        include/kivm/field.h
        include/kivm/oop/mirrorKlass.h
        include/kivm/oop/oopfwd.h
        include/kivm/oop/instanceOop.h
        include/kivm/oop/primitiveOop.h
        include/kivm/oop/mirrorOop.h
        include/kivm/oop/arrayKlass.h
        include/kivm/oop/arrayOop.h
        include/kivm/runtime/slot.h
        include/kivm/runtime/stack.h
        src/kivm/oop/oopBase.cpp
        src/kivm/classfile/classFileStream.cpp
        src/kivm/oop/oop.cpp
        src/kivm/classfile/constantPool.cpp
        src/shared/string.cpp
        src/kivm/classfile/classFileParser.cpp
        src/kivm/classfile/classFile.cpp
        src/kivm/classfile/attributeInfo.cpp
        src/kivm/oop/klass.cpp
        src/kivm/classLoader.cpp
        src/kivm/oop/instanceKlass.cpp
        src/kivm/method.cpp
        src/kivm/bytecode/interpreter.cpp
        src/kivm/field.cpp
        src/kivm/baseClassLoader.cpp
        src/kivm/oop/mirrorKlass.cpp
        src/kivm/oop/instanceOop.cpp
        src/kivm/oop/primitiveOop.cpp
        src/kivm/oop/mirrorOop.cpp
        src/kivm/oop/arrayOop.cpp
        src/kivm/oop/arrayKlass.cpp
        src/kivm/runtime/stack.cpp
        include/kivm/runtime/frame.h
        src/kivm/runtime/frame.cpp
        include/kivm/runtime/thread.h
        include/kivm/oop/helper.h include/kivm/oop/reflection_support.h src/kivm/runtime/thread.cpp include/kivm/runtime_config.h src/kivm/runtime_config.cpp include/kivm/bytecode/bytecodes.h include/kivm/bytecode/execution.h src/kivm/bytecode/execution.cpp include/kivm/native/java_lang_Class.h src/kivm/native/java_lang_Class.cpp src/kivm/runtime/init.cpp include/kivm/system.h src/kivm/system.cpp include/kivm/native/class_names.h include/kivm/native/java_lang_Thread.h include/kivm/native/java_lang_String.h src/kivm/native/java_lang_String.cpp)

add_library(kivm STATIC ${SOURCE_FILES})
add_executable(KivaVM src/bin/main.cpp)
add_executable(javap src/bin/javap.cpp)

target_link_libraries(KivaVM kivm)
target_link_libraries(javap kivm)

if (DEFINED ENV{CS_SRC})
    set(CS_SRC $ENV{CS_SRC})
    include_directories(${CS_SRC}/include)
    add_library(covscript-kivm SHARED
            ext/covscript-kivm.cpp
            ext/extension_helper.h)
    target_link_libraries(covscript-kivm kivm)
    set_target_properties(covscript-kivm PROPERTIES OUTPUT_NAME "kivm")
    set_target_properties(covscript-kivm PROPERTIES PREFIX "")
    set_target_properties(covscript-kivm PROPERTIES SUFFIX ".cse")
endif ()

add_executable(test_stack-and-frame tests/stack-and-locals.cpp)
target_link_libraries(test_stack-and-frame kivm)
