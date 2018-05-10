//
// Created by van on 18-5-7.
//

#ifndef TINY_SERVER_CONFIGURE_H
#define TINY_SERVER_CONFIGURE_H

#include <map>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>

namespace tiny_server {

class ConfigureValueType {
public:
    ConfigureValueType();
    ConfigureValueType(const std::string& conf_value);
    ~ConfigureValueType();

    // must allow copy ctor for Configure's operator[]
    ConfigureValueType(const ConfigureValueType& other);

    int32_t to_int32() const;
    int64_t to_int64() const;
    uint32_t to_uint32() const;
    uint64_t to_uint64() const;
    double to_double() const;
    std::string to_string() const;
    char to_char() const;

private:
    template <typename T>
    T _to_type() const;

    std::string _conf_value;

    // disallow assign
    ConfigureValueType& operator=(const ConfigureValueType& other);
};

template <typename T>
T ConfigureValueType::_to_type() const {
    if (_conf_value.empty()) {
        throw std::logic_error("_to_type--invalid config key");
    }

    std::istringstream iss(_conf_value);
    T return_var;
    iss >> return_var;
    return return_var;
}

/////////////////////////////////////////////////////////////////////////////

class ConfigureParser {
public:
    ConfigureParser();
    ~ConfigureParser();

    // @brief: 对给出的配置文件进行解析
    // @file_path: 文件路径
    // @conf_pairs: map指针，存放解析后的键值对
    // @return: 成功true，失败false
    bool parse(const std::string& file_path, 
            std::map<std::string, std::string>* conf_pairs);

private:
    // @brief: 对目标字符串进行格式化，包括
    // 忽略空格、忽略‘#’打头的子串
    // @str: 需要格式化的字符串
    // @return: 成功true，失败false
    bool _str_format(std::string* str);

    // disallow copy and assign
    ConfigureParser(const ConfigureParser& other);
    ConfigureParser& operator=(const ConfigureParser& other);
};

//////////////////////////////////////////////////////////////////////////////

class Configure {
public:
    Configure();
    ~Configure();

    bool init_config(const std::string& file_path);
    ConfigureValueType operator[](const std::string& conf_key);

private:
    ConfigureParser _conf_parser; // 解析配置文件的解析器
    std::map<std::string, std::string> _conf_pairs; // 存储配置文件中的键值对

    // disallow copy and assign
    Configure(const Configure& other);
    Configure& operator=(const Configure& other);
};

}

#endif //TINY_SERVER_CONFIGURE_H
