#ifndef MMC_THREAD_CONTEXT_H_
#define MMC_THREAD_CONTEXT_H_

#include "mmc_options.h"

struct MmcContext_s {
    const MmcOptions* ctx;
    void* imageCompressor;
};
typedef struct MmcContext_s MmcContext;

static inline void initMmcContext(MmcContext* tCtx){
    tCtx->ctx = NULL;
    tCtx->imageCompressor = NULL;
}

#endif
