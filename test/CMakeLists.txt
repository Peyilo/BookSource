add_executable(test_jsengine test_jsengine.cpp)

target_link_libraries(test_jsengine PRIVATE booksource)

add_executable(test_object_bind test_object_bind.cpp)

target_link_libraries(test_object_bind PRIVATE booksource)

add_executable(test_rttr test_rttr.cpp)

target_link_libraries(test_rttr PRIVATE booksource)

