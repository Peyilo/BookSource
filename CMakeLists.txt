cmake_minimum_required(VERSION 4.0)
project(booksource LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(test)
add_subdirectory(third_party/quickjs)

file(GLOB_RECURSE SRC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

file(GLOB_RECURSE INCLUDE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/booksource/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/booksource/*.h"
)

add_library(booksource ${SRC_FILES} ${INCLUDE_FILES})

# js引擎: quickjs
target_link_libraries(booksource PUBLIC quickjs)

target_include_directories(booksource
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(booksource
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/json/include
)
