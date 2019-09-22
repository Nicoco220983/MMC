/* We want POSIX.1-2008 + XSI, i.e. SuSv4, features */
#define _XOPEN_SOURCE 700

/* Added on 2017-06-25:
   If the C library can support 64-bit file sizes
   and offsets, using the standard names,
   these defines tell the C library to do so. */
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <ftw.h>

#include "mmc_context.h"
#include "image_compressor.h"

#ifndef USE_FDS
#define USE_FDS 15
#endif

const char* usage = "ccm [-i INPUT_PATH]\n";

void exitBadArg(){
	printf("ERROR: Bad arguments.\n");
	printf("%s", usage);
	exit(1);
}

void exitErr(const char* msg){
	printf("ERROR: %s\n", msg);
	exit(1);
}

char* getArg(int i, int argc, char **argv){
	if(i>=argc) exitBadArg();
	return argv[i];
}

bool isDir(const char* path){
	struct stat sb;
	return (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode));
}

bool isFile(const char* path){
	struct stat sb;
	return (stat(path, &sb) == 0 && S_ISREG(sb.st_mode));
}

bool isImage(const char* ext){
	return ((strcasecmp(ext, "jpg") == 0)
		|| (strcasecmp(ext, "jpeg") == 0)
		|| (strcasecmp(ext, "png") == 0));
}

bool isVideo(const char* ext){
	return ((strcasecmp(ext, "avi") == 0)
		|| (strcasecmp(ext, "mp4") == 0));
}

const char* getExt(const char* path){
	int len = strlen(path);
	const char* res = path;
	for(int i=0; i<len; ++i)
		if(path[i]=='.')
			res = path+i+1;
	return res;
}

void compressVideo(MmcContext* ctx, const char* path){
	printf("Compress video: %s\n", path);
}

void compressFile(MmcContext* ctx, const char* path){
	const char* ext = getExt(path);
	if(isImage(ext)){
		compressImage(ctx, path);
	} else if(isVideo(ext)){
		compressVideo(ctx, path);
	} else {
		printf("Ignored file: %s\n", path);
	}
}

void compressDir(MmcContext* ctx, const char* path){
	int callback(const char* filepath, const struct stat *info,
	const int typeflag, struct FTW *pathinfo){
		if(isFile(filepath))
			compressFile(ctx, filepath);
		return 0;
	}
	nftw(path, callback, USE_FDS, FTW_PHYS);
}

void compressAny(MmcContext* ctx, const char* path){
	if(isDir(path)){
		compressDir(ctx, path);
	} else if(isFile(path)){
		compressFile(ctx, path);
	} else exitErr("Input file not found.");
}

int main(int argc, char **argv){
	MmcContext ctx;
	const char* inputPath = NULL;
	initMmcContext(&ctx);

	for(int i=0; i<argc; ++i){
		const char* arg = argv[i];
		if((strcmp("-i", arg) == 0) || (strcmp("--input", arg) == 0)){
			inputPath = getArg(++i, argc, argv);
		}
	}
	if(inputPath == NULL) exitBadArg();

	newImageCompressor(&ctx);

	compressAny(&ctx, inputPath);

	delImageCompressor(&ctx);

	return 0;
}