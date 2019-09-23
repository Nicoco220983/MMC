#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

void mkdirp(const char *dir) {
        char tmp[256];
        char *p = NULL;
        size_t len;

        snprintf(tmp, sizeof(tmp), "%s", dir);
        len = strlen(tmp);
        if(tmp[len - 1] == '/')
                tmp[len - 1] = 0;
        for(p = tmp + 1; *p; p++)
                if(*p == '/') {
                        *p = 0;
                        mkdir(tmp, S_IRWXU);
                        *p = '/';
                }
        mkdir(tmp, S_IRWXU);
}


bool startswith(const char* str, const char* start){
    size_t start_len = strlen(start);
    if(strlen(str) < start_len)
        return false;
    return strncmp(str, start, start_len) == 0;
}


bool removeStartPath(const char* str, const char* start, char* res){
    if(strcmp(str, start) == 0) {
        strcpy(res, ".");
        return true;
    }
    else if(startswith(str, start)){
        strcpy(res, str+strlen(start)+1);
        return true;
    }
    return false;
}