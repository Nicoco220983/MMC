#include "mmc_context.h"

#include <stddef.h>

void initMmcContext(MmcContext* ctx){
    ctx->imageCompressor = NULL;
}