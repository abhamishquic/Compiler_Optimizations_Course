cmake_minimum_required(VERSION 3.13)
project(MyPass LANGUAGES CXX)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "LLVM CMAKE DIR: ${LLVM_CMAKE_DIR}")
message(STATUS "LLVM_INCLUDE_DIRS: ${LLVM_INCLUDE_DIRS}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)
include_directories(${LLVM_INCLUDE_DIRS})

add_llvm_pass_plugin(MyPass passes.cpp)
