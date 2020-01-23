#include <sstream>
#include <exception>
#include <functional>
#include <dirent.h>
#include <cassert>

#include "utils.hpp"
#include "conf.hpp"
#include "mmc.hpp"

namespace mmc {


bool _binExists(const Context& ctx, const std::string& name){
	std::stringstream cmd;
	try {
		auto res = exec(ctx, {"which", name});
		return (res.size() > 0);
	} catch(std::exception) {
		return false;
	}
}


Context::MediaType _getMediaExt(const std::string& name){
	const char* ext = fs::getExt(name);
	if(ext == nullptr) return Context::MediaType::NONE;
	std::string extLower = toLower(ext);
	if(IMAGE_EXTS.find(extLower.c_str()) != IMAGE_EXTS.end())
		return Context::MediaType::IMAGE;
	if(VIDEO_EXTS.find(extLower.c_str()) != VIDEO_EXTS.end())
		return Context::MediaType::VIDEO;
	return Context::MediaType::NONE;
}


void _forEachFile(const Context& ctx, const std::string& basePath, const std::string& relPath, const std::function<void(const std::string&)>& next){
    DIR *dir;
	struct dirent *ent;
	std::string fullPath = fs::join(basePath, relPath);
	if((dir = opendir(fullPath.c_str())) != NULL) {
		while((ent = readdir(dir)) != NULL) {
			auto childName = ent->d_name;
			if(strcmp(childName, ".")==0 || strcmp(childName, "..")==0)
				continue;
			std::string childPath = fs::join(relPath, childName);
			if(ent->d_type == DT_DIR){
				// dir: recursive call
				_forEachFile(ctx, basePath, childPath, next);
			} else if(ent->d_type == DT_REG){
				// file: callbacks
				next(childPath);
			}
		}
		closedir(dir);
	} else {
		// could not open directory
		log(LogLevel::WARNING, ctx, concat({"Could not open directory: ", fullPath}));
	}
}


bool _isImageCompressed(const Context& ctx, const std::string& path){
	std::string desc = exec(ctx, {"identify", path});
	std::vector<std::string> descs = split(desc, ' ');
	assert(descs.size() >= 3);
	std::string& dim = descs[2];
	std::vector<std::string> dims = split(dim, 'x');
	assert(dims.size() == 2);
	int width = atoi(dims[0].c_str());
	int height = atoi(dims[1].c_str());
	return (width <= ctx.imgMinLength && height <= ctx.imgMinLength);
}


void _compressImage(Context& ctx, const std::string& iPath, const std::string& oPath){
	log(LogLevel::INFO, ctx, concat({"Compress image: ", iPath, " to ", oPath}));
	std::stringstream resolution;
	resolution << ctx.imgMinLength << 'x' << ctx.imgMinLength << ">";
	exec(ctx, { "convert", iPath, "-resize", resolution.str(), oPath });
}


bool _isVideoCompressed(const Context& ctx, const std::string& path){
	std::string desc = exec(ctx, {"ffprobe" , "-i", path});
	std::vector<std::string> lines = split(desc, '\n');
	for(auto& line : lines){
		std::vector<std::string> words = split(line, ' ');
		for(size_t i = 1; i<words.size(); ++i){
			if(words[i] == "kb/s,"){
				int bitrate = atoi(words[i-1].c_str());
				return bitrate <= ctx.videoBitrate;
			}
		}
	}
	throw std::runtime_error("Could not determine video bitrate");
}


void _compressVideo(Context& ctx, const std::string& iPath, const std::string& oPath){
	log(LogLevel::INFO, ctx, concat({"Compress video: ", iPath, " to ", oPath}));
	std::string stdout = exec(ctx, { "ffmpeg", "-i", iPath, "-c:v", "libx264", "-crf", std::to_string(ctx.videoCrf), oPath });
	log(LogLevel::DEBUG, ctx, stdout);
}


bool _isMediaCompressed(const Context& ctx, Context::MediaType ext, const std::string& path){
	if(ext == Context::MediaType::IMAGE){
		return _isImageCompressed(ctx, path);
	} else if(ext == Context::MediaType::VIDEO){
		return _isVideoCompressed(ctx, path);
	}
	throw std::logic_error("Unmanaged MediaType");
}


std::string _buildTmpFilePath(const std::string& path){
	std::stringstream res;
	auto idx = path.find_last_of('.');
	res << path.substr(0,idx) << ".tmp" << path.substr(idx);
	return res.str();
}


void _compressMedia(Context& ctx, const std::string& relPath){
	ctx.currentPath = relPath;
	std::string iPath = fs::join(ctx.inputPath, relPath);
	std::string oPath = iPath;
	if(ctx.outputMode == Context::OutputMode::COPY){
		oPath = fs::join(ctx.outputPath, relPath);
	}
	bool oExists = fs::exists(oPath);
	try {
		auto ext = _getMediaExt(iPath);
		bool oCompressed = false;
		if(oExists){
			oCompressed = _isMediaCompressed(ctx, ext, oPath);
			if(oCompressed) return;
		}
		fs::mkdirp(fs::dirname(oPath), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		bool iCompressed = false;
		if(oExists && iPath == oPath) iCompressed = oCompressed;
		else iCompressed = _isMediaCompressed(ctx, ext, iPath);
		if(not iCompressed){
			const std::string oTmpPath = _buildTmpFilePath(oPath);
			remove(oTmpPath.c_str());
			if(ext == Context::MediaType::IMAGE)
				_compressImage(ctx, iPath, oTmpPath);
			else if(ext == Context::MediaType::VIDEO)
				_compressVideo(ctx, iPath, oTmpPath);
			remove(oPath.c_str());
			rename(oTmpPath.c_str(), oPath.c_str());
		}
	} catch(const std::exception& exc) {
		log(LogLevel::ERROR, ctx, exc.what());
	}
	if(not fs::exists(oPath) && iPath!=oPath){
		log(LogLevel::INFO, ctx, concat({"Copy file: ", iPath, " to ", oPath}));
		fs::copy(iPath, oPath);
	}
	ctx.currentPath = "";
}


void execute(Context& ctx){

	if(_binExists(ctx, "convert")) ctx.imgCompressor = Context::ImageCompressor::MAGICK;
	else throw std::runtime_error("imagemagick should be installed");

	if(_binExists(ctx, "ffmpeg")) ctx.videoCompressor = Context::VideoCompressor::FFMPEG;
	else if(_binExists(ctx, "avconv")) ctx.videoCompressor = Context::VideoCompressor::AVCONV;
	else throw std::runtime_error("ffmpeg, or avconv should be installed");

	if(ctx.outputMode == Context::OutputMode::COPY) {
		if(ctx.outputPath == "")
			throw std::runtime_error("In copy mode, outputPath must be filled.");
		if(ctx.inputPath == ctx.outputPath)
			throw std::runtime_error("In copy mode, inputPath and outputPath cannot have same value. Use overwrite mode");
	}

	if(ctx.compressionlevel == Context::CompressionLevel::SMALL){
		if(ctx.imgMinLength == 0)
			ctx.imgMinLength = 800;
		if(ctx.videoBitrate == 0)
			ctx.videoBitrate = 1500;
		if(ctx.videoCrf == 0)
			ctx.videoCrf = 32;
	} else if(ctx.compressionlevel == Context::CompressionLevel::MEDIUM){
		if(ctx.imgMinLength == 0)
			ctx.imgMinLength = 1024;
		if(ctx.videoBitrate == 0)
			ctx.videoBitrate = 2500;
		if(ctx.videoCrf == 0)
			ctx.videoCrf = 28;
	} else if(ctx.compressionlevel == Context::CompressionLevel::LARGE){
		if(ctx.imgMinLength == 0)
			ctx.imgMinLength = 2018;
		if(ctx.videoBitrate == 0)
			ctx.videoBitrate = 5000;
		if(ctx.videoCrf == 0)
			ctx.videoCrf = 24;
	}

	FileType inputType = fs::getFileType(ctx.inputPath);
	if(inputType == FileType::REG){
		_compressMedia(ctx, ctx.inputPath);
	} else if(inputType == FileType::DIR){
		_forEachFile(ctx, ctx.inputPath, "", [&](const std::string& mediaRelPath){
			_compressMedia(ctx, mediaRelPath);
		});
	} else {
		throw std::runtime_error(concat({"Cannot compress this file: ", ctx.inputPath}).c_str());
	}
}

}