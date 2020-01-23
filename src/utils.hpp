#pragma once

#include <cstring>
#include <vector>

#include "Context.hpp"

// common includes
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

namespace mmc {

#ifdef _WIN32
const char DIR_SEP = '\\';
#else
const char DIR_SEP = '/';
#endif

enum class FileType { NONE, DIR, REG };

struct cstrless {
    inline  bool operator()(const char* a, const char* b) const {
        return strcmp(a, b) < 0;
    }
};

std::string concat(const std::vector<std::string>& strs);

std::vector<std::string> split(const std::string& str, char del);

std::string exec(const Context& ctx, const std::vector<std::string>& cmd);

std::string toLower(const std::string& str);
bool iequals(const std::string& str1, const std::string& str2);

void log(LogLevel logLevel, const Context& ctx, const std::string& text);


namespace fs {

const char* getExt(const std::string& name);

FileType getFileType(const std::string& path);

std::string join(const std::string& a, const std::string& b);

bool exists(const std::string& path);

void copy(const std::string& ipath, const std::string& opath);

std::string dirname(const std::string& path);

void mkdirp(const std::string& path, mode_t mode);

}

}