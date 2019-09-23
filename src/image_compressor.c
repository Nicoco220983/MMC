#include "image_compressor.h"

#include <stdio.h>

MagickWand* NewMmcImageCompressor(){
	//MagickWandGenesis();
	return NewMagickWand();
}

void DelMmcImageCompressor(MagickWand* wand){
	DestroyMagickWand(wand);
	//MagickWandTerminus();
}

void determineNewSize(MagickWand* wand, MmcContext* ctx, unsigned int* oWidth, unsigned int* oHeight){
	unsigned int width = MagickGetImageWidth(wand);
	unsigned int height = MagickGetImageHeight(wand);
	unsigned int minSize = ctx->imageMinSize;
	if(width > height){
		*oWidth = (unsigned int)( ((float)width) / ( ((float)height) / (float)minSize) );
		*oHeight = minSize;
	} else {
		*oWidth = minSize;
		*oHeight = (unsigned int)( ((float)height) / ( ((float)width) / (float)minSize) );
	}

}

void MmcCompressImage(MagickWand* wand, MmcContext* ctx, const char* inputPath, const char* outputPath){
	unsigned int status;
	status = MagickReadImage(wand, inputPath);
  	if(status == MagickTrue) {
  		MagickResetIterator(wand);
  		while (MagickNextImage(wand) != MagickFalse){
			unsigned int newWidth, newHeight;
			determineNewSize(wand, ctx, &newWidth, &newHeight);
			printf("Compress image: %s into %s %ix%i\n", inputPath, outputPath, newWidth, newHeight);
    		status = MagickResizeImage(wand, newWidth, newHeight, LanczosFilter, 1.0);
		  }
	}
  	if(status == MagickTrue)
		status = MagickWriteImages(wand, outputPath, MagickTrue);
	if(status == MagickFalse) {
		printf("[ERROR] Failed to compress image: %s\n", inputPath);
	}
}