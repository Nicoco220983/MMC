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

#include "mmc.h"
#include "utils.h"
#include "mmc_options.h"
#include "mmc_context.h"
#include "image_compressor.h"

#ifndef USE_FDS
#define USE_FDS 15
#endif

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

bool compressVideo(MmcContext* tCtx, const char* inputPath, const char* outputPath){
	printf("Compress video: %s\n", inputPath);
	return false;
}

bool calcOutputPath(const MmcOptions* ctx, const char* inputPath, char* outputPath){
	int outputMode = ctx->outputMode;
	if(outputMode == MMC_OUTPUT_MODE_COPY_FILE){
		strcpy(outputPath, ctx->outputPath);
	} else if(outputMode == MMC_OUTPUT_MODE_COPY_DIR){
		char relPath[1024];
		if(removeStartPath(inputPath, ctx->inputPath, relPath)){
			sprintf(outputPath, "%s%c%s", ctx->outputPath, FS_SEP, relPath);
		} else {
			printf("[ERROR] Failed to calc outputPath for %s\n", inputPath);
			return false;
		}
	} else if(outputMode == MMC_OUTPUT_MODE_OVERWRITE){
		sprintf(outputPath, "%s.tmp", inputPath);
	}
	return true;
}

void compressFile(MmcContext* tCtx, const char* path){
	const char* ext = getExt(path);
	char outputPath[1024];
	if(calcOutputPath(tCtx->ctx, path, outputPath)){
		mkfiledirp(outputPath);
		bool OK = false;
		if(isImage(ext)){
			OK = MmcCompressImage(tCtx->imageCompressor, tCtx->ctx, path, outputPath);
		} else if(isVideo(ext)){
			OK = compressVideo(tCtx, path, outputPath);
		} else {
			printf("Ignored file: %s\n", path);
		}
		if(tCtx->ctx->outputMode == MMC_OUTPUT_MODE_OVERWRITE){
			if(OK) rename(outputPath, path);
			else remove(outputPath);
		}
	}
}

void compressDir(MmcContext* tCtx, const char* path){
	int callback(const char* filepath, const struct stat *info,
	const int typeflag, struct FTW *pathinfo){
		if(isFile(filepath)){
			compressFile(tCtx, filepath);
		}
		return 0;
	}
	nftw(path, callback, USE_FDS, FTW_PHYS);
}

bool compressAny(MmcContext* tCtx, const char* path){
	if(isDir(path)){
		compressDir(tCtx, path);
	} else if(isFile(path)){
		compressFile(tCtx, path);
	} else {
		printErr("Input file not found.");
		return false;
	}
	return true;
}

bool MmcCompress(MmcOptions* ctx){

	if(ctx->outputMode == MMC_OUTPUT_MODE_COPY_UNDEF){
		if(isFile(ctx->inputPath)) ctx->outputMode = MMC_OUTPUT_MODE_COPY_FILE;
		else if(isDir(ctx->inputPath)) ctx->outputMode = MMC_OUTPUT_MODE_COPY_DIR;
		else {
			printErr("Invalid input path.");
			return false;
		}
	}
	if(ctx->outputMode != MMC_OUTPUT_MODE_OVERWRITE && ctx->outputPath == NULL){
		printErr("outputPath must be provided");
		return false;
	}
	if(ctx->imgMinLength == 0){
		if(ctx->compressionLevel == MMC_COMPRESSION_LEVEL_SMALL)
			ctx->imgMinLength = 600;
		else if(ctx->compressionLevel == MMC_COMPRESSION_LEVEL_MEDIUM)
			ctx->imgMinLength = 800;
		else if(ctx->compressionLevel == MMC_COMPRESSION_LEVEL_SMALL)
			ctx->imgMinLength = 1024;
	}

	MmcContext tCtx;
	initMmcContext(&tCtx);

	tCtx.ctx = ctx;
	tCtx.imageCompressor = NewMmcImageCompressor();

	bool res = compressAny(&tCtx, ctx->inputPath);

	DelMmcImageCompressor(tCtx.imageCompressor);

	return res;
}
