cmake_minimum_required(VERSION 3.1...3.15)

if (${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(CMAKE_BUILD_TYPE Debug)
project("MapReduce" LANGUAGES C)

find_package(Threads REQUIRED)
add_library(threadpool STATIC src/threadpool.c src/threadpool.h)

add_executable(test_queue test/test_queue.c)
target_link_libraries(test_queue PRIVATE threadpool pthread)

add_executable(test_threadpool test/test_threadpool.c)
target_link_libraries(test_threadpool PRIVATE threadpool pthread)