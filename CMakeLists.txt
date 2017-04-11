cmake_minimum_required(VERSION 3.7)
project(ChatServer)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c ThreadPool.h ThreadPool.c)
add_executable(ChatServer ${SOURCE_FILES})
SET(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pthread")