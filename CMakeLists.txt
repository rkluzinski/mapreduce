cmake_minimum_required(VERSION 3.1...3.15)

if (${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# project settings
set(CMAKE_BUILD_TYPE Debug)
project("MapReduce" LANGUAGES C CXX)

# threadpool library
add_library(threadpool STATIC src/threadpool.c src/threadpool.h)
target_link_libraries(threadpool PRIVATE pthread)

# work queue tests
add_executable(test_queue test/queue.c)
target_link_libraries(test_queue PRIVATE threadpool)

# threadpool tests
add_executable(test_threadpool test/threadpool.c)
target_link_libraries(test_threadpool PRIVATE threadpool )

# mapreduce library
add_library(mapreduce STATIC src/mapreduce.cpp src/mapreduce.h)
target_link_libraries(mapreduce PRIVATE threadpool)

# wordcount executable
add_executable(wordcount src/distw.c)
target_link_libraries(wordcount PRIVATE mapreduce)