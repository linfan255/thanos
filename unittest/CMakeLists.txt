set(CONFIGURE_TEST_SOURCE 
        ${PROJECT_SOURCE_DIR}/src/configure.cpp
        configure_test.cpp)
set(BUFFER_TEST_SOURCE
        ${PROJECT_SOURCE_DIR}/src/easylogging++.cc
        ${PROJECT_SOURCE_DIR}/src/buffer.cpp
        buffer_test.cpp)

add_executable(configure_test ${CONFIGURE_TEST_SOURCE})
target_link_libraries(configure_test gtest gtest_main)

add_executable(buffer_test ${BUFFER_TEST_SOURCE})
target_link_libraries(buffer_test gtest gtest_main)
