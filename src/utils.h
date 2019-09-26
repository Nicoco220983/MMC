#ifndef MMC_UTILS_HPP_
#define MMC_UTILS_HPP_

#include <stdbool.h>
#include <string.h>

#define PATH_LEN 1024

#ifdef _WIN32
    #define FS_SEP '\\'
#else
    #define FS_SEP '/'
#endif

static inline bool streq(const char* str1, const char* str2){
    return strcmp(str1, str2) == 0;
}

void mkdirp(const char *dir);

bool startswith(const char* str, const char* start);

bool removeStartPath(const char* str, const char* start, char* res);

void printErr(const char* msg);

#endif