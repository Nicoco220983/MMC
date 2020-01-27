local Context = require "context"
local MMC = require "mmc"

usage = [[mmc [OPTIONS]\n
  -h/--help: Print this help.\n
  -i/--input PATH: Input path to compress (file or dir).\n
  -o/--output PATH: compression destination.\n
  --overwrite: Overwrite the input content.\n
  -c/--compression-level LEVEL: Compression level (Possible values: S, [M], L).\n
  --img-min-length: Image minimum length.\n
  --vid-bitrate: Video bitrate.\n
  --vid-crf: Video CRF.\n
  -l/--log-level LEVEL: Log level (Possible values: DEBUG, [INFO], ERROR).\n]]

local ctx = Context:new()

local i = 1
while i <= #arg do
    a = arg[i]
    if a=="-h" or a=="--help" then
        print(usage)
        os.exit(0)
    elseif a=="-i" or a=="--input" then
        i = i+1; ctx.inputPath = arg[i]
    elseif a=="-o" or a=="--output" then
        i = i+1; ctx.outputPath = arg[i]
    elseif a == "--overwrite" then
        ctx.outputMode = "OVERWRITE"
    elseif a=="-c" or a=="--compression-level" then
        i = i+1; ctx.compressionlevel = string.upper(arg[i])
    elseif a =="--img-min-length" then
        i = i+1; ctx.imgMinLength = tonumber(arg[i])
    elseif a == "--vid-bitrate" then
        i = i+1; ctx.videoBitrate = tonumber(arg[i])
    elseif a == "--vid-crf" then
        i = i+1; ctx.videoCrf = tonumber(arg[i])
    elseif a=="-l" or a=="--log-level" then
        i = i+1; ctx.loglevel = string.upper(arg[i])
    else
        error("Unknown arg " .. a)
    end
    i = i+1
end

MMC(ctx)