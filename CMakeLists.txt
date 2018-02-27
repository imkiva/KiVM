cmake_minimum_required(VERSION 3.8)
project(KivaVM)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

set(SOURCE_FILES
        include/kivm/classFile.h
        include/kivm/kivm.h
        include/kivm/interpreter.h
        src/kivm/classFileStream.cpp
        include/kivm/oop.h
        include/kivm/klass.h
        include/kivm/instanceKlass.h
        include/shared/monitor.h
        include/shared/lock.h
        src/kivm/oopBase.cpp
        src/kivm/oop.cpp
        src/kivm/constantPool.cpp
        include/shared/string.h
        include/kivm/constantPool.h
        include/shared/types.h
        src/shared/string.cpp
        include/kivm/classFileStream.h
        include/kivm/classFileParser.h
        src/kivm/classFileParser.cpp
        src/kivm/classFile.cpp
        include/kivm/attributeInfo.h
        src/kivm/attributeInfo.cpp src/kivm/klass.cpp include/kivm/classLoader.h src/kivm/classLoader.cpp include/kivm/method.h src/kivm/InstanceKlass.cpp src/kivm/method.cpp)

add_executable(KivaVM ${SOURCE_FILES} src/bin/main.cpp)
add_executable(javap ${SOURCE_FILES} src/bin/javap.cpp)