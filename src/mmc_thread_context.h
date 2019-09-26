#ifndef MMC_THREAD_CONTEXT_H_
#define MMC_THREAD_CONTEXT_H_

#include "mmc_context.h"

struct MmcThreadContext_s {
    const MmcContext* ctx;
    void* imageCompressor;
};
typedef struct MmcThreadContext_s MmcThreadContext;

static inline void initMmcThreadContext(MmcThreadContext* tCtx){
    tCtx->ctx = NULL;
    tCtx->imageCompressor = NULL;
}

#endif