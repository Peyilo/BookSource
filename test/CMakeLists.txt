add_executable(test_jsengine EXCLUDE_FROM_ALL test_jsengine.cpp)
target_link_libraries(test_jsengine PRIVATE booksource)

add_executable(test_object_bind EXCLUDE_FROM_ALL test_object_bind.cpp)
target_link_libraries(test_object_bind PRIVATE booksource)

add_executable(test_quickjs EXCLUDE_FROM_ALL test_quickjs.cpp)
target_link_libraries(test_quickjs PRIVATE booksource)

# 生成一个头文件，用于确定当前项目的路径
set(PROJECT_ROOT_DIR "${CMAKE_SOURCE_DIR}")
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/config/project_root.h.in
        ${CMAKE_CURRENT_SOURCE_DIR}/project_root.h
)

add_test(NAME TestJsEngine COMMAND test_jsengine)
add_test(NAME TestObjectBind COMMAND test_object_bind)
add_test(NAME TestQuickJS COMMAND test_quickjs)
