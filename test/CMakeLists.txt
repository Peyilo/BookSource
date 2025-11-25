add_executable(test_jsengine test_jsengine.cpp)

target_link_libraries(test_jsengine PRIVATE booksource)

add_executable(test_object_bind test_object_bind.cpp)

target_link_libraries(test_object_bind PRIVATE booksource)

add_executable(test_quickjs test_quickjs.cpp)

target_link_libraries(test_quickjs PRIVATE booksource)

