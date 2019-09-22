#include "image_compressor.h"

#include <stdio.h>
#include <wand/magick_wand.h>

void newImageCompressor(MmcContext* ctx){
	//MagickWandGenesis();
	ctx->imageCompressor = NewMagickWand();
}

void delImageCompressor(MmcContext* ctx){
	DestroyMagickWand(ctx->imageCompressor);
	//MagickWandTerminus();
}

void compressImage(MmcContext* ctx, const char* path){
	printf("Compress image: %s\n", path);
}