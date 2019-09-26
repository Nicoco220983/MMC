#ifndef MMC_IMAGE_COMPRESSOR_H_
#define MMC_IMAGE_COMPRESSOR_H_

#include "mmc_context.h"
#include <wand/magick_wand.h>

MagickWand* NewMmcImageCompressor();

void DelMmcImageCompressor(MagickWand*);

void MmcCompressImage(MagickWand* wand, const MmcContext* ctx, const char* inputPath, const char* outputPath);

#endif