//
// Created by van on 18-5-7.
//

#include <fstream>
#include "configure.h"

namespace tiny_server {

ConfigureValueType::ConfigureValueType() : _conf_value("") {}

ConfigureValueType::ConfigureValueType(const std::string& conf_value) :
        _conf_value(conf_value) {}

ConfigureValueType::~ConfigureValueType() = default;

ConfigureValueType::ConfigureValueType(const ConfigureValueType& other) : 
        _conf_value(other._conf_value) {}

int32_t ConfigureValueType::to_int32() const {
    return _to_type<int32_t>();
}

int64_t ConfigureValueType::to_int64() const {
    return _to_type<int64_t>();
}

uint32_t ConfigureValueType::to_uint32() const {
    return _to_type<uint32_t>();
}

uint64_t ConfigureValueType::to_uint64() const {
    return _to_type<uint64_t>();
}

double ConfigureValueType::to_double() const {
    return _to_type<double>();
}

std::string ConfigureValueType::to_string() const {
    return _to_type<std::string>();
}

char ConfigureValueType::to_char() const {
    return _to_type<char>();
}

///////////////////////////////////////////////////////////////////////

ConfigureParser::ConfigureParser() = default;
ConfigureParser::~ConfigureParser() = default;

bool ConfigureParser::_str_format(std::string* str) {
    if (str == nullptr) {
        return false;
    }
    if (str->empty()) {
        return true;
    }

    // 忽略‘#’打头的注释
    uint64_t comment_pos = str->find("#");
    if (comment_pos != std::string::npos) {
        *str = str->substr(0, comment_pos);
    }

    // 清理字符串的前后空格
    uint64_t pre_pos = 0;
    uint64_t post_pos = str->length();

    while (pre_pos < str->length() && (*str)[pre_pos++] == ' ');
    while (post_pos > 0 && (*str)[--post_pos] == ' ');
    if (pre_pos - 1 >= post_pos + 1) {
        str->clear();
        return true;
    }
    *str = str->substr(pre_pos - 1, post_pos + 1);

    return true;
}

bool ConfigureParser::parse(const std::string& file_path,
        std::map<std::string, std::string>* conf_pairs) {
    std::ifstream fin(file_path.c_str());
    if (!fin) {
        return false;
    }

    std::string line = "";
    std::string key = "";
    std::string value = "";
    uint64_t pos = 0;

    while (getline(fin, line)) {
        if (!_str_format(&line)) {
            return false;
        }
        if (line.empty()) {
            continue;
        }

        pos = line.find(":");
        if (pos == std::string::npos) {
            return false;
        }

        key = line.substr(0, pos);
        if (!_str_format(&key)) {
            return false;
        }
        value = line.substr(pos + 1);
        if (!_str_format(&value)) {
            return false;
        }
        if (key.empty() || value.empty()) {
            return false;
        }

        (*conf_pairs)[key] = value;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////

Configure::Configure() : _conf_pairs(), _conf_parser() {}
Configure::~Configure() = default;

bool Configure::init_config(const std::string& file_path) {
    return _conf_parser.parse(file_path, &_conf_pairs);
}

ConfigureValueType Configure::operator[](const std::string& conf_key) {
    if (_conf_pairs.find(conf_key) == _conf_pairs.end()) {
        throw std::logic_error("operator[]--invalid config key:" + conf_key);
    } else {
        return _conf_pairs[conf_key];
    }
}

}
