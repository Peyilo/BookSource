cmake_minimum_required(VERSION 4.0)
project(booksource LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(third_party/quickjs)

add_executable(booksource src/main.cpp
        src/booksource.h
        src/booksource_parser.h
        src/js_utils.cpp
        src/js_utils.h)

find_package(Threads REQUIRED)
target_link_libraries(booksource PRIVATE Threads::Threads)

target_link_libraries(booksource PRIVATE quickjs)

target_include_directories(booksource
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/json/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/quickjs
)
