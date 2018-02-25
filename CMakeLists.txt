cmake_minimum_required(VERSION 3.8)
project(KivaVM)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

set(SOURCE_FILES src/main.cpp
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
        src/kivm/classFileParser.cpp)
add_executable(KivaVM ${SOURCE_FILES})