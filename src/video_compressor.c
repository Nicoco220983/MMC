#include "video_compressor.h"

#include <libavformat/avformat.h>

bool MmcCompressVideo(const MmcOptions* ctx, const char* inputPath, const char* outputPath){
    AVFormatContext *fmt_ctx = NULL;
    if (avformat_open_input(&fmt_ctx, inputPath, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", inputPath);
        exit(1);
    }
	return true;
}
