#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <ctype.h>
#include <libgen.h>


void mkdirp(const char *path) {
        char tmp[256];
        char *p = NULL;
        size_t len;

        snprintf(tmp, sizeof(tmp), "%s", path);
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

void mkfiledirp(const char *path) {
    char path2[PATH_LEN];
    strcpy(path2, path);
    mkdirp(dirname(path2));
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

void printErr(const char* msg){
    printf("ERROR: %s\n", msg);
}

bool streqi(char const *a, char const *b)
{
    if(strlen(a) != strlen(b))
        return false;
    for (;; a++, b++) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return false;
    }
    return true;
}
