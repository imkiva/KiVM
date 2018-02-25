cmake_minimum_required(VERSION 3.8)
project(KivaVM)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

set(SOURCE_FILES src/main.cpp include/kivm/classFile.h include/kivm/kivm.h include/kivm/interpreter.h src/classFileStream.cpp)
add_executable(KivaVM ${SOURCE_FILES})