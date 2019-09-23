#ifndef MMC_UTILS_HPP_
#define MMC_UTILS_HPP_

#include <stdbool.h>

void mkdirp(const char *dir);

bool startswith(const char* str, const char* start);

bool removeStartPath(const char* str, const char* start, char* res);

#endif