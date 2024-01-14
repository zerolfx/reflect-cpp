project(reflect-cpp-msgpack-tests)

file(GLOB_RECURSE SOURCES "*.cpp")

add_executable(reflect-cpp-msgpack-tests ${SOURCES})

target_link_libraries(reflect-cpp-msgpack-tests PRIVATE reflectcpp)
