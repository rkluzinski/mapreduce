cmake_minimum_required(VERSION 3.1...3.15)

if (${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# project settings
set(CMAKE_BUILD_TYPE Debug)
project("MapReduce" LANGUAGES C)

# threadpool library
find_package(Threads REQUIRED)
add_library(threadpool STATIC src/threadpool.c src/threadpool.h)

# work queue tests
add_executable(test_queue test/queue.c)
target_link_libraries(test_queue PRIVATE threadpool pthread)

# threadpool tests
add_executable(test_threadpool test/threadpool.c)
target_link_libraries(test_threadpool PRIVATE threadpool pthread)

# mapreduce library
add_library(mapreduce STATIC src/mapreduce.c src/mapreduce.h)
target_link_libraries(mapreduce PRIVATE threadpool pthread)

# vector library
add_library(vector STATIC src/vector.c src/vector.h)

# vector tests
add_executable(test_vector test/vector.c)
target_link_libraries(test_vector PRIVATE vector)

# hash table library

# hash table tests

# wordcount executable
add_executable(wordcount wordcount/distw.c)
target_link_libraries(wordcount PRIVATE mapreduce)