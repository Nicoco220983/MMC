#ifndef IMAGE_COMPRESSOR_H_
#define IMAGE_COMPRESSOR_H_

#include "mmc_context.h"

void newImageCompressor(MmcContext*);

void delImageCompressor(MmcContext*);

void compressImage(MmcContext*, const char*);

#endif