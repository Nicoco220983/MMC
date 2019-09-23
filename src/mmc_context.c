#include "mmc_context.h"

#include <stddef.h>
#include <string.h>

void initMmcContext(MmcContext* ctx){

    ctx->outputMode = MMC_OUTPUT_MODE_COPY_DIR;
    strcpy(ctx->outputPath, "res");

    ctx->imageCompressor = NULL;
    ctx->imageMinSize = 100;
}