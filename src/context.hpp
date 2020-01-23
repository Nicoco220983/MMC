#pragma once

namespace mmc {

enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

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
	int videoBitrate = 0;
	int videoCrf = 0;
	ImageCompressor imgCompressor;
	VideoCompressor videoCompressor;
	bool force = false;
	LogLevel logLevel = LogLevel::INFO;

	std::string currentPath;
};

}