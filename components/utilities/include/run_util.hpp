/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef _RUN_UTIL_H_
#define _RUN_UTIL_H_
#include <string>
#include <vector>
#include <fstream>
#include <cstdint>
namespace dutilities {

std::string formatString(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    size_t sz = vsnprintf(nullptr, 0U, fmt, args);  // calculate length
    va_end(args);
    char buf[sz + 1];
    va_start(args, fmt);  // Reinitiaize args (args cannot reuse because
                          // indefinite value after vsnprintf)
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    // String don't has constructor(const char*, const size_t);
    buf[sz] = '\0';
    return std::string(buf);
}

std::vector<std::string> split_string(const std::string& content, const std::string& delimiter)
{
    std::vector<std::string> result;
    std::string::size_type pos1 = 0;
    std::string::size_type pos2 = content.find(delimiter);
    while (std::string::npos != pos2) {
        result.push_back(content.substr(pos1, pos2 - pos1));
        pos1 = pos2 + delimiter.size();
        pos2 = content.find(delimiter, pos1);
    }
    if (pos1 != content.length()) {
        result.push_back(content.substr(pos1));
    }
    return result;
}

std::vector<std::string> split_each_utf8_char(const std::string& text)
{
    std::vector<std::string> words;
    std::string input(text);
    int len = input.length();
    int i   = 0;
    while (i < len) {
        int next = 1;
        if ((input[i] & 0x80) == 0x00) {
        } else if ((input[i] & 0xE0) == 0xC0) {
            next = 2;
        } else if ((input[i] & 0xF0) == 0xE0) {
            next = 3;
        } else if ((input[i] & 0xF8) == 0xF0) {
            next = 4;
        }
        words.push_back(input.substr(i, next));
        i += next;
    }
    return words;
}

bool file_exist(const std::string& path)
{
    auto flag = false;
    std::fstream fs(path, std::ios::in | std::ios::binary);
    flag = fs.is_open();
    fs.close();
    return flag;
}

bool read_file(const std::string& path, std::vector<char>& data)
{
    std::fstream fs(path, std::ios::in | std::ios::binary);
    if (!fs.is_open()) {
        return false;
    }
    fs.seekg(std::ios::end);
    auto fs_end = fs.tellg();
    fs.seekg(std::ios::beg);
    auto fs_beg      = fs.tellg();
    auto file_size   = static_cast<size_t>(fs_end - fs_beg);
    auto vector_size = data.size();
    data.reserve(vector_size + file_size);
    data.insert(data.end(), std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>());
    fs.close();
    return true;
}

bool dump_file(const std::string& path, std::vector<char>& data)
{
    std::fstream fs(path, std::ios::out | std::ios::binary);
    if (!fs.is_open() || fs.fail()) {
        fprintf(stderr, "[ERR] cannot open file %s \n", path.c_str());
    }
    fs.write(data.data(), data.size());
    return true;
}

bool dump_file(const std::string& path, char* data, int size)
{
    std::fstream fs(path, std::ios::out | std::ios::binary);
    if (!fs.is_open() || fs.fail()) {
        fprintf(stderr, "[ERR] cannot open file %s \n", path.c_str());
    }
    fs.write(data, size);
    return true;
}
};  // namespace dutilities
#endif /* _STRING_UTIL_H_ */
