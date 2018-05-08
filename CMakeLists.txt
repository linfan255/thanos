cmake_minimum_required(VERSION 3.8)
project(tiny_server)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp include/locker.h include/threadpool.h src/Buffer.cpp include/Buffer.h src/HTTPMessage.cpp include/HTTPMessage.h src/HTTPRequest.cpp include/HTTPRequest.h src/HTTPResponse.cpp include/HTTPResponse.h src/Client.cpp include/Client.h src/Configure.cpp include/Configure.h)
add_executable(tiny_server ${SOURCE_FILES})

set()

find_package(Threads REQUIRED)
target_link_libraries(tiny_server Threads::Threads)