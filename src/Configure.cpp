//
// Created by van on 18-5-7.
//

#include <fstream>
#include "../include/Configure.h"

namespace tiny_server {

ConfigureValueType::ConfigureValueType() : _conf_value("") {}

ConfigureValueType::ConfigureValueType(const std::string &conf_value) :
        _conf_value(conf_value) {}

ConfigureValueType::~ConfigureValueType() = default;

int32_t ConfigureValueType::to_int32() {
    return _to_type<int32_t>();
}

int64_t ConfigureValueType::to_int64() {
    return _to_type<int64_t>();
}

uint32_t ConfigureValueType::to_uint32() {
    return _to_type<uint32_t>();
}

uint64_t ConfigureValueType::to_uint64() {
    return _to_type<uint64_t>();
}

double ConfigureValueType::to_double() {
    return _to_type<double>();
}

std::string ConfigureValueType::to_string() {
    return _to_type<std::string>();
}

char ConfigureValueType::to_char() {
    return _to_type<char>();
}

/////////////////////////////////////////////////////////////

Configure::Configure() : _conf_pairs() {}

Configure::~Configure() = default;

bool Configure::init_config(const char *file_path) {
    std::ifstream fin(file_path);

    std::string line = "";
    std::string key = "";
    std::string value = "";
    uint64_t pos = 0; // 配置文件中分割符的位置

    while (getline(fin, line)) {
        key.clear();
        value.clear();

        pos = line.find(":");
        if (pos == std::string::npos) {
            return false;
        }

        key = line.substr(0, pos);
        value = line.substr(pos + 1);
        if (key.empty() || value.empty()) {
            return false;
        }

        _conf_pairs[key] = value;
    }
    return true;
}

const ConfigureValueType& Configure::operator[](const std::string conf_key) {
    if (_conf_pairs.find(conf_key) == _conf_pairs.end()) {
        throw std::logic_error("invalid config key");
    } else {
        return _conf_pairs[conf_key];
    }
}

}
