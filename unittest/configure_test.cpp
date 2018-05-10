//
// Created by van on 18-5-8.
//
#include <iostream>
#include <gtest/gtest.h>
#include "configure.h"

namespace tiny_server {

class ConfigureTest : public ::testing::Test {
public:
    ConfigureTest();
    ~ConfigureTest();

    static Configure conf;
protected:
    virtual void SetUp();
    // virtual void TearDown();
};

Configure ConfigureTest::conf;

ConfigureTest::ConfigureTest() = default;
ConfigureTest::~ConfigureTest() = default;

void ConfigureTest::SetUp() {
    if (!conf.init_config("../conf/server.conf")) {
        std::cerr << "conf.init_config failed" << std::endl;
        return;
    }
}
/////////////////////////////////////////////////////////////////////////

TEST_F(ConfigureTest, TestInt) {
    // int32_t & uint32_t
    ASSERT_EQ(5, conf["int32_key_0"].to_int32());
    ASSERT_EQ(-5, conf["int32_key_1"].to_int32());
    ASSERT_EQ(2147483648, conf["uint32_key"].to_uint32());

    ASSERT_EQ(2147483648, conf["int64_key_0"].to_int64());
    ASSERT_EQ(-2147483648, conf["int64_key_1"].to_int64());
    ASSERT_EQ(9223372036854775806, conf["uint64_key"].to_uint64());
}

TEST_F(ConfigureTest, TestDouble) {
    ASSERT_DOUBLE_EQ(3.1415926, conf["double_key_0"].to_double());
    ASSERT_DOUBLE_EQ(-3.1415926, conf["double_key_1"].to_double());
}

TEST_F(ConfigureTest, TestString) {
    ASSERT_EQ("value1", conf["key1"].to_string());
}

}
