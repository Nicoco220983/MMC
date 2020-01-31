package mmcompress

/*
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
*/

type MediaType int

const (
	IMAGE MediaType = iota + 1
	VIDEO
)

/*
type ImageCompressor int
const (
    MAGICK ImageCompressor = iota + 1
)

type VideoCompressor int
const (
	FFMPEG VideoCompressor = iota + 1
	AVCONV
)
*/
type Context struct {
	InputPath        string
	OutputPath       string
	TmpPath          string
	OutputMode       string
	CompressionLevel string
	ImgMinLength     int
	VideoBitrate     int
	VideoCrf         int
	ImageCompressor  string
	VideoCompressor  string
	Force            bool
	LogLevel         string

	CurrentPath string
}

func NewContext() Context {
	ctx := Context{}
	ctx.OutputMode = "COPY"
	ctx.CompressionLevel = "M"
	ctx.Force = false
	ctx.LogLevel = "INFO"
	return ctx
}
