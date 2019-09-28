#ifndef MMC_IMAGE_COMPRESSOR_H_
#define MMC_IMAGE_COMPRESSOR_H_

#include "mmc_options.h"
#include <wand/magick_wand.h>
#include <stdbool.h>

MagickWand* NewMmcImageCompressor();

void DelMmcImageCompressor(MagickWand*);

bool MmcCompressImage(MagickWand* wand, const MmcOptions* ctx, const char* inputPath, const char* outputPath);

#endif
