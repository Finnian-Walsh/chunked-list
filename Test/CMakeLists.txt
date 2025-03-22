cmake_minimum_required(VERSION 3.10)

project(ChunkedListTests VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_FILES)

include_directories(../include include)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_executable(ChunkedListTest
        ${SRC_FILES}
        test_files/ChunkedListTest.cpp
)

add_executable(ChunkedListSnakeTest
        ${SRC_FILES}
        test_files/ChunkedListSnakeTest.cpp
)

add_executable(UnitTest
        ${SRC_FILES}
        test_files/UnitTests.cpp
)
