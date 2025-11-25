add_library(quickjs
        src/cutils.c
        src/quickjs.c
        src/libregexp.c
        src/libunicode.c
        src/dtoa.c
)

# 读取 VERSION 文件内容
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" QJS_VERSION_RAW)
string(STRIP "${QJS_VERSION_RAW}" QJS_VERSION)
message(STATUS "QuickJS version = ${QJS_VERSION}")
# 把 VERSION 写入宏 CONFIG_VERSION
target_compile_definitions(quickjs PRIVATE
        CONFIG_VERSION=\"${QJS_VERSION}\"
        CONFIG_BIGNUM
        _GNU_SOURCE
)

target_include_directories(quickjs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (WIN32)
    target_link_libraries(quickjs
            PRIVATE
            pthread
            winmm
    )
endif()
