set(CONFIGURE_TEST_SOURCE 
        ${PROJECT_SOURCE_DIR}/src/configure.cpp
        configure_test.cpp)
set(BUFFER_TEST_SOURCE
        ${PROJECT_SOURCE_DIR}/src/easylogging++.cc
        ${PROJECT_SOURCE_DIR}/src/buffer.cpp
        buffer_test.cpp)

set(THREADPOOL_TEST_SOURCE
        ${PROJECT_SOURCE_DIR}/src/easylogging++.cc
        ${PROJECT_SOURCE_DIR}/src/locker.cpp
        threadpool_test.cpp)

set(SERVER_TEST_SOURCE
        ${PROJECT_SOURCE_DIR}/src/easylogging++.cc
        ${PROJECT_SOURCE_DIR}/src/configure.cpp
        ${PROJECT_SOURCE_DIR}/src/buffer.cpp
        ${PROJECT_SOURCE_DIR}/src/fd_handler.cpp
        ${PROJECT_SOURCE_DIR}/src/connection.cpp
        ${PROJECT_SOURCE_DIR}/src/locker.cpp
        ${PROJECT_SOURCE_DIR}/src/server.cpp
        server_test.cpp
        )

set(CLIENT_TEST_SOURCE
        ${PROJECT_SOURCE_DIR}/src/easylogging++.cc
        client_test.cpp
        )

add_executable(configure_test ${CONFIGURE_TEST_SOURCE})
target_link_libraries(configure_test gtest gtest_main)

add_executable(buffer_test ${BUFFER_TEST_SOURCE})
target_link_libraries(buffer_test gtest gtest_main)

add_executable(threadpool_test ${THREADPOOL_TEST_SOURCE})

add_executable(server_test ${SERVER_TEST_SOURCE})
add_executable(client_test ${CLIENT_TEST_SOURCE})
