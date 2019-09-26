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

void determineNewSize(MagickWand* wand, const MmcContext* ctx, size_t* oWidth, size_t* oHeight){
	size_t width = MagickGetImageWidth(wand);
	size_t height = MagickGetImageHeight(wand);
	size_t minLength = ctx->imgMinLength;
	if(width > height){
		*oWidth = (size_t)( ((float)width) / ( ((float)height) / (float)minLength) );
		*oHeight = minLength;
	} else {
		*oWidth = minLength;
		*oHeight = (size_t)( ((float)height) / ( ((float)width) / (float)minLength) );
	}

}

void MmcCompressImage(MagickWand* wand, const MmcContext* ctx, const char* inputPath, const char* outputPath){
	unsigned int status;
	status = MagickReadImage(wand, inputPath);
  	if(status == MagickTrue) {
  		MagickResetIterator(wand);
  		while (MagickNextImage(wand) != MagickFalse){
			size_t newWidth, newHeight;
			determineNewSize(wand, ctx, &newWidth, &newHeight);
			printf("Compress image: %s into %s %ix%i\n", inputPath, outputPath, (int)newWidth, (int)newHeight);
    		status = MagickResizeImage(wand, newWidth, newHeight, LanczosFilter, 1.0);
		  }
	}
  	if(status == MagickTrue)
		status = MagickWriteImages(wand, outputPath, MagickTrue);
	if(status == MagickFalse) {
		printf("[ERROR] Failed to compress image: %s\n", inputPath);
	}
}