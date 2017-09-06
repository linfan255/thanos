cmake_minimum_required(VERSION 3.8)
project(tiny_server)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp locker.h threadpool.h Buffer.cpp Buffer.h HTTPMessage.cpp HTTPMessage.h HTTPRequest.cpp HTTPRequest.h HTTPResponse.cpp HTTPResponse.h Client.cpp Client.h)
add_executable(tiny_server ${SOURCE_FILES})

find_package(Threads REQUIRED)
target_link_libraries(tiny_server Threads::Threads)