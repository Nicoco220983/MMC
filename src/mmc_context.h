#ifndef MMC_CONTEXT_H_
#define MMC_CONTEXT_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define MMC_OUTPUT_MODE_COPY_UNDEF 0
#define MMC_OUTPUT_MODE_COPY_FILE 1
#define MMC_OUTPUT_MODE_COPY_DIR 2
#define MMC_OUTPUT_MODE_OVERWRITE 3

struct MmcContext_s {
    char inputPath[1024];
    char outputPath[1024];
    int outputMode;
    size_t imgMinLength;
};
typedef struct MmcContext_s MmcContext;

static inline void initMmcContext(MmcContext* ctx){
    strcpy(ctx->inputPath, "");
    strcpy(ctx->outputPath , "");
    ctx->outputMode = MMC_OUTPUT_MODE_COPY_UNDEF;
    ctx->imgMinLength = 0;
}

#endif