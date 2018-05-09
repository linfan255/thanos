set(CONFIGURE_TEST_SOURCE 
        ${PROJECT_SOURCE_DIR}/src/configure.cpp
        configure_test.cpp)

add_executable(configure_test ${CONFIGURE_TEST_SOURCE})
target_link_libraries(configure_test gtest gtest_main)
