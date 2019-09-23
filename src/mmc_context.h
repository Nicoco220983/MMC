#ifndef MMC_CONTEXT_H_
#define MMC_CONTEXT_H_

#include <stdint.h>

#define MMC_OUTPUT_MODE_COPY_FILE 0
#define MMC_OUTPUT_MODE_COPY_DIR 1
#define MMC_OUTPUT_MODE_OVERWRITE 2

struct _MmcContext {
    int outputMode;
    char inputPath[1024];
    char outputPath[1024];
    void* imageCompressor;
    unsigned int imageMinSize;
};

typedef struct _MmcContext MmcContext;

void initMmcContext(MmcContext* ctx);

#endif