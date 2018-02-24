cmake_minimum_required(VERSION 3.8)
project(KivaVM)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp include/kivm/class_file.h include/kivm/kivm.h include/kivm/interpreter.h)
add_executable(KivaVM ${SOURCE_FILES})