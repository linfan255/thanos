cmake_minimum_required(VERSION 3.8)
project(tiny_server)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp inc/locker.h inc/threadpool.h src/Buffer.cpp inc/Buffer.h src/HTTPMessage.cpp inc/HTTPMessage.h src/HTTPRequest.cpp inc/HTTPRequest.h src/HTTPResponse.cpp inc/HTTPResponse.h src/Client.cpp inc/Client.h)
add_executable(tiny_server ${SOURCE_FILES})

find_package(Threads REQUIRED)
target_link_libraries(tiny_server Threads::Threads)