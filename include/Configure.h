//
// Created by van on 18-5-7.
//

#ifndef TINY_SERVER_CONFIGURE_H
#define TINY_SERVER_CONFIGURE_H

#include <map>
#include <string>
#include <sstream>
#include <stdexcept>

namespace tiny_server {

class ConfigureValueType {
public:
    ConfigureValueType();
    ConfigureValueType(const std::string &conf_value);
    ~ConfigureValueType();

    int32_t to_int32();
    int64_t to_int64();
    uint32_t to_uint32();
    uint64_t to_uint64();

    double to_double();
    std::string to_string();
    char to_char();

private:
    template <typename T>
    T _to_type();

    std::string _conf_value;

    // disallow copy & assign
    ConfigureValueType(const ConfigureValueType &);
    ConfigureValueType &operator=(const ConfigureValueType &);
};

template <typename T>
T ConfigureValueType::_to_type() {
    if (_conf_value.empty()) {
        throw std::logic_error("invalid config key");
    }

    std::istringstream iss(_conf_value);
    T return_var;
    iss >> return_var;
    return return_var;
}

/////////////////////////////////////////////////////////////////////////////

class Configure {
public:
    Configure();
    ~Configure();

    bool init_config(const char* file_path);
    const ConfigureValueType &operator[](const std::string conf_key);

private:
    std::map<std::string, std::string> _conf_pairs; // 存储配置文件中的键值对

    // disallow copy and assign
    Configure(const Configure &);
    Configure &operator=(const Configure &);
};

}



#endif //TINY_SERVER_CONFIGURE_H
