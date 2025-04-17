cmake_minimum_required(VERSION 3.10)

project(chunked-list VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(../include include)

add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-error=aggressive-loop-optimizations)

add_executable(Chunk-UT entry_files/ChunkedList/Chunk_ut.cpp)

add_executable(ChunkedList-IT entry_files/ChunkedList/ChunkedList_it.cpp)

add_executable(ChunkedList-benchmark entry_files/ChunkedList/benchmark.cpp)
