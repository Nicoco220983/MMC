#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <libgen.h>

#include "mmc.h"
#include "utils.h"
#include "mmc_options.h"

const char* usage = \
  "ccm [OPTIONS]\n"
  "  -h/--help: Print this help.\n"
  "  -i/--input PATH: Input path to compress (file or dir).\n"
  "  -o/--output PATH: compression destination.\n"
  "  --overwrite: Overwrite the input content.\n"
  "  -l/--compression-level LEVEL: Compression level (Possible value: S, M, L).\n"
  "  --img-min-length: Image minimum length.\n";

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

int main(int argc, char **argv){

	MmcOptions ctx;
	initMmcOptions(&ctx);

	for(int i=0; i<argc; ++i){
		const char* arg = argv[i];
		if(streq("-h", arg) || streq("--help", arg)){
			printf("%s", usage);
			exit(0);
		}
		if(streq("-i", arg) || streq("--input", arg)){
			strcpy(ctx.inputPath, getArg(++i, argc, argv));
		}
		if(streq("-o", arg) || streq("--output", arg)){
			strcpy(ctx.outputPath, getArg(++i, argc, argv));
		}
		if(streq("--overwrite", arg)){
			ctx.outputMode = MMC_OUTPUT_MODE_OVERWRITE;
		}
		if(streq("-l", arg) || streq("--compression-level", arg)){
			const char* val = getArg(++i, argc, argv);
			if(streqi("S", val)){
				ctx.compressionLevel = MMC_COMPRESSION_LEVEL_SMALL;
			} else if(streqi("M", val)){
				ctx.compressionLevel = MMC_COMPRESSION_LEVEL_MEDIUM;
			} else if(streqi("L", val)){
				ctx.compressionLevel = MMC_COMPRESSION_LEVEL_LARGE;
			} else {
				exitErr("Unknown value for compression-level.");
			}
		}
		if(streq("--img-min-length", arg)){
			ctx.imgMinLength = atoi(arg);
		}
	}

	bool res = MmcCompress(&ctx);

	return res ? 0 : 1;
}
