#pragma once

namespace mmc {

struct Context {

    enum class OutputMode { COPY, OVERWRITE };
    enum class CompressionLevel { SMALL, MEDIUM, LARGE };
    enum class MediaType { NONE, IMAGE, VIDEO };
    enum class ImageCompressor { NONE, MAGICK };
    enum class VideoCompressor { NONE, FFMPEG, AVCONV };

	std::string inputPath;
	std::string outputPath;
	OutputMode outputMode = OutputMode::COPY;
	CompressionLevel compressionlevel  = CompressionLevel::MEDIUM;
	int imgMinLength = 0;
	int videoMinBitrate = 0;
	ImageCompressor imgCompressor;
	VideoCompressor videoCompressor;
	bool force = false;

	std::string currentPath;
};

}