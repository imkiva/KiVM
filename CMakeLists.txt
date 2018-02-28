cmake_minimum_required(VERSION 3.8)
project(KivaVM)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

set(SOURCE_FILES
        include/kivm/classfile/classFile.h
        include/kivm/kivm.h
        include/kivm/interpreter.h
        src/kivm/classfile/classFileStream.cpp
        include/kivm/oop/oop.h
        include/kivm/oop/klass.h
        include/kivm/oop/instanceKlass.h
        include/shared/monitor.h
        include/shared/lock.h
        src/kivm/oop/oopBase.cpp
        src/kivm/oop/oop.cpp
        src/kivm/classfile/constantPool.cpp
        include/shared/string.h
        include/kivm/classfile/constantPool.h
        include/shared/types.h
        src/shared/string.cpp
        include/kivm/classfile/classFileStream.h
        include/kivm/classfile/classFileParser.h
        src/kivm/classfile/classFileParser.cpp
        src/kivm/classfile/classFile.cpp
        include/kivm/classfile/attributeInfo.h
        src/kivm/classfile/attributeInfo.cpp src/kivm/oop/klass.cpp include/kivm/classLoader.h src/kivm/classLoader.cpp include/kivm/method.h src/kivm/oop/instanceKlass.cpp src/kivm/method.cpp include/kivm/field.h src/kivm/field.cpp include/kivm/oop/mirrorKlass.h include/kivm/oop/oopfwd.h src/kivm/oop/mirrorKlass.cpp include/kivm/oop/instanceOop.h src/kivm/oop/instanceOop.cpp include/kivm/oop/primitiveOop.h src/kivm/oop/primitiveOop.cpp include/kivm/oop/mirrorOop.h src/kivm/oop/mirrorOop.cpp include/kivm/oop/arrayKlass.h include/kivm/oop/arrayOop.h src/kivm/oop/arrayOop.cpp src/kivm/oop/arrayKlass.cpp)

add_executable(KivaVM ${SOURCE_FILES} src/bin/main.cpp)
add_executable(javap ${SOURCE_FILES} src/bin/javap.cpp)