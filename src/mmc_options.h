#ifndef MMC_CONTEXT_H_
#define MMC_CONTEXT_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define MMC_OUTPUT_MODE_COPY_UNDEF 0
#define MMC_OUTPUT_MODE_COPY_FILE 1
#define MMC_OUTPUT_MODE_COPY_DIR 2
#define MMC_OUTPUT_MODE_OVERWRITE 3

#define MMC_COMPRESSION_LEVEL_SMALL 0
#define MMC_COMPRESSION_LEVEL_MEDIUM 1
#define MMC_COMPRESSION_LEVEL_LARGE 1

struct MmcOptions_s {
    char inputPath[1024];
    char outputPath[1024];
    int outputMode;
    int compressionLevel;
    size_t imgMinLength;
};
typedef struct MmcOptions_s MmcOptions;

static inline void initMmcOptions(MmcOptions* ctx){
    strcpy(ctx->inputPath, "");
    strcpy(ctx->outputPath , "");
    ctx->outputMode = MMC_OUTPUT_MODE_COPY_UNDEF;
    ctx->compressionLevel = MMC_COMPRESSION_LEVEL_MEDIUM;
    ctx->imgMinLength = 0;
}

#endif
