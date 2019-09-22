#ifndef MMC_CONTEXT_H_
#define MMC_CONTEXT_H_

struct _MmcContext {
    void* imageCompressor;
};

typedef struct _MmcContext MmcContext;

void initMmcContext(MmcContext* ctx);

#endif