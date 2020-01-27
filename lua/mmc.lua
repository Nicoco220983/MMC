local utils = require "utils"

function compress_media(ctx, path)
    ctx.currentPath = path
    local iPath = utils.join(ctx.inputPath, path)
    local oPath = iPath
    if ctx.outputMode == "COPY" then
        oPath = utils.join(ctx.outputPath, path)
    end
    local oExists = utils.exists(oPath)
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
            fs::dupTime(iPath, oPath);
        }
    } catch(const std::exception& exc) {
        log(LogLevel::ERROR, ctx, exc.what());
    }
    if(not fs::exists(oPath) && iPath!=oPath){
        log(LogLevel::INFO, ctx, concat({"Copy file: ", iPath, " to ", oPath}));
        fs::copy(iPath, oPath);
        fs::dupTime(iPath, oPath);
    }
    ctx.currentPath = "";
end


function rec_compress_media(ctx, path)
    local full_path = utils.join(ctx.inputPath, path)
    if utils.is_dir(full_path) then
        for i, child_name in pairs(utils.list_dir(full_path)) do
            local child_path = utils.join(path, child_name)
            rec_compress_media(ctx, child_path)
        end
    else
        compress_media(ctx, path)
    end
end


function MMC(ctx)

	if utils.bin_exists(ctx, "convert") then ctx.imgCompressor = "convert"
    elseif utils.bin_exists(ctx, "magick") then ctx.imgCompressor = "magick"
    end
	-- else error("imagemagick should be installed") end

	if utils.bin_exists(ctx, "ffmpeg") then ctx.videoCompressor = "FFMPEG"
    elseif utils.bin_exists(ctx, "avconv") then ctx.videoCompressor = "AVCONV"
    end
    -- else error("ffmpeg, or avconv should be installed") end

	if ctx.outputMode == "COPY" then
		if ctx.outputPath == nil then
            error("In copy mode, outputPath must be filled.")
        end
		if ctx.inputPath == ctx.outputPath then
            error("In copy mode, inputPath and outputPath cannot have same value. Use overwrite mode")
        end
    end

	if ctx.compressionlevel == "S" then
		if ctx.imgMinLength == nil then ctx.imgMinLength = 800 end
		if ctx.videoBitrate == nil then ctx.videoBitrate = 1500 end
		if ctx.videoCrf == nil then ctx.videoCrf = 32 end
    elseif ctx.compressionlevel == "M" then
		if ctx.imgMinLength == nil then ctx.imgMinLength = 1024 end
		if ctx.videoBitrate == nil then ctx.videoBitrate = 2500 end
		if ctx.videoCrf == nil then ctx.videoCrf = 28 end
	elseif ctx.compressionlevel == "L" then
		if ctx.imgMinLength == nil then ctx.imgMinLength = 2018 end
		if ctx.videoBitrate == nil then ctx.videoBitrate = 5000 end
		if ctx.videoCrf == nil then ctx.videoCrf = 24 end
    else
        error("Uknown compression level: " .. ctx.compressionlevel)
    end

    if not utils.file_exists(ctx.inputPath) then
        error("File does not exists: " .. ctx.inputPath)
    end
    rec_compress_media(ctx)
end

return MMC