// 
// Created by van on 18-5-22
//
#include <iostream>
#include <string>
#include <gtest/gtest.h>
#include <cstdio>
#include <cstring>
#include "buffer.h"
#include "common_type.h"
#include "easylogging++.h"


namespace thanos {

class BufferTest : public ::testing::Test {
public:
    BufferTest();
    ~BufferTest();

    static Buffer buffer;
};

Buffer BufferTest::buffer;

BufferTest::BufferTest() = default;
BufferTest::~BufferTest() = default;

// only contains \r\n 
TEST_F(BufferTest, Case_1) {
    buffer.clear();
    ASSERT_TRUE(buffer.empty());

    char content[] = "\r\n";
    buffer.push_nbytes(content, strlen(content));

    std::string res = "";
    EXPECT_EQ(BufferReadStatus::READ_END, buffer.get_line(&res));
    EXPECT_TRUE(res.empty());
}

// normal 
TEST_F(BufferTest, Case_2) {
    buffer.clear();
    std::string res = "";

    ASSERT_TRUE(res.empty());
    ASSERT_TRUE(buffer.empty());
    
    char line1[] = "GET /webDemo/Hellow HTTP/1.1\r\n";
    char line2[] = "Host: localhost:8080\r\n";
    char line3[] = "Connection: keep-alive\r\n";
    char line4[] = "\r\n";
    buffer.push_nbytes(line1, strlen(line1));
    buffer.push_nbytes(line2, strlen(line2));
    buffer.push_nbytes(line3, strlen(line3));
    buffer.push_nbytes(line4, strlen(line4));
    
    EXPECT_EQ(BufferReadStatus::READ_SUCCESS, buffer.get_line(&res));
    EXPECT_EQ("GET /webDemo/Hellow HTTP/1.1", res);

    EXPECT_EQ(BufferReadStatus::READ_SUCCESS, buffer.get_line(&res));
    EXPECT_EQ("Host: localhost:8080", res);

    EXPECT_EQ(BufferReadStatus::READ_END, buffer.get_line(&res));
    EXPECT_EQ("Connection: keep-alive", res);

    EXPECT_EQ(BufferReadStatus::READ_END, buffer.get_line(&res));
    EXPECT_TRUE(res.empty());
}

// test failed
TEST_F(BufferTest, Case_3) {
    buffer.clear();
    std::string res = "";

    ASSERT_TRUE(res.empty());
    ASSERT_TRUE(buffer.empty());

    char line1[] = "GET /webDemo/Hellow HTTP/1.1\r\n";
    char line2[] = "GET /webDemo/Hellow HTTP/1.1";
    buffer.push_nbytes(line1, strlen(line1));
    buffer.push_nbytes(line2, strlen(line2));

    EXPECT_EQ(BufferReadStatus::READ_SUCCESS, buffer.get_line(&res));
    EXPECT_EQ("GET /webDemo/Hellow HTTP/1.1", res);
    EXPECT_EQ(BufferReadStatus::READ_FAIL, buffer.get_line(&res));
    EXPECT_TRUE(res.empty());
}
} // namepace thanos

INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv) {
    el::Configurations conf("../conf/easylog.conf");
    el::Loggers::reconfigureLogger("default", conf);
    el::Loggers::reconfigureAllLoggers(conf);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

