package mmc

type LogLevel int
const (
    DEBUG LogLevel = iota
    INFO
    WARNING
    ERROR
)

type OutputMode int
const (
    COPY OutputMode = iota
    OVERWRITE
)

type CompressionLevel int
const (
    SMALL CompressionLevel = iota
	MEDIUM
	LARGE
)

type MediaType int
const (
    IMAGE MediaType = iota + 1
	VIDEO
)

type ImageCompressor int
const (
    MAGICK ImageCompressor = iota + 1
)

type VideoCompressor int
const (
	FFMPEG VideoCompressor = iota + 1
	AVCONV
)

type Context struct {  
    InputPath   string
    OutputPath  string
    OutputMode  OutputMode
	CompressionLevel CompressionLevel
	ImgMinLength int
	VideoBitrate int
	VideoCrf int
	ImageCompressor ImageCompressor
	VideoCompressor VideoCompressor
	Force bool
	LogLevel LogLevel

	CurrentPath string
}

func NewContext() Context {
	ctx := Context{}
	ctx.CompressionLevel = MEDIUM
	ctx.LogLevel = INFO
	return ctx
}