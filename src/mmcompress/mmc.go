package mmcompress

import (
	"fmt"
	"os"
	"errors"
	"strings"
	"path"
	"path/filepath"
	"io/ioutil"
	"strconv"
)

func MMC(ctx Context) error {

	if ctx.InputPath == "" {
		return errors.New("iutputPath must be filled.")
	}

	if ctx.OutputMode == COPY {
		if ctx.OutputPath == "" {
			return errors.New("In copy mode, outputPath must be filled.")
		}
		if ctx.InputPath == ctx.OutputPath {
			return errors.New("In copy mode, inputPath and outputPath cannot have same value. Use overwrite mode")
		}
	}

	if ctx.CompressionLevel == SMALL {
		if ctx.ImgMinLength == 0 { ctx.ImgMinLength = 800 }
		if ctx.VideoBitrate == 0 { ctx.VideoBitrate = 1500 }
		if ctx.VideoCrf == 0 { ctx.VideoCrf = 32 }
	} else if ctx.CompressionLevel == MEDIUM {
		if ctx.ImgMinLength == 0 { ctx.ImgMinLength = 1024 }
		if ctx.VideoBitrate == 0 { ctx.VideoBitrate = 2500 }
		if ctx.VideoCrf == 0 { ctx.VideoCrf = 28 }
	} else if ctx.CompressionLevel == LARGE {
		if ctx.ImgMinLength == 0 { ctx.ImgMinLength = 2018 }
		if ctx.VideoBitrate == 0 { ctx.VideoBitrate = 5000 }
		if ctx.VideoCrf == 0 { ctx.VideoCrf = 24 }
	}

	if BinExists(ctx, "magick") { ctx.ImageCompressor = MAGICK }

	if BinExists(ctx, "ffmpeg") { ctx.VideoCompressor = FFMPEG
	} else if BinExists(ctx, "avconv") { ctx.VideoCompressor = AVCONV }

    if ! FileExists(ctx.InputPath) {
		return errors.New(fmt.Sprintf("Input file does not exists: %s", ctx.InputPath))
	}
    compressAllMedia(ctx, "")

	return nil
}

func compressAllMedia(ctx Context, relPath string) {

    fullPath := path.Join(ctx.InputPath, relPath)
    if IsDir(fullPath) {

		files, err := ioutil.ReadDir(fullPath)
		if err != nil { Log(ERROR, ctx, err.Error()); return }
		for _, f := range files {
            childPath := path.Join(relPath, f.Name())
            compressAllMedia(ctx, childPath)
		}
	} else {
		compressMedia(ctx, relPath)
	}
}

func compressMedia(ctx Context, relPath string) {
	var err error
	ctx.CurrentPath = relPath
	iPath := path.Join(ctx.InputPath, relPath)
	oPath := iPath
	if ctx.OutputMode == COPY {
		oPath = path.Join(ctx.OutputPath, relPath)
	}
	err = os.MkdirAll(filepath.Dir(oPath), os.ModePerm)
	if err != nil { Log(ERROR, ctx, err.Error()); return }
	err = compressMedia2(ctx, iPath, oPath)
	if err != nil { Log(ERROR, ctx, err.Error()) }
	if (!FileExists(oPath)) && iPath!=oPath {
		Log(INFO, ctx, fmt.Sprintf("Copy file: %s to %s", iPath, oPath))
		CopyFile(iPath, oPath)
		DupTime(iPath, oPath)
	}
	ctx.CurrentPath = ""
}

func compressMedia2(ctx Context, iPath string, oPath string) error {
	var err error
	mediaType := GetMediaType(iPath)
	if mediaType == 0 { Log(DEBUG, ctx, "Not a media"); return nil }
	oExists := FileExists(oPath)
	oCompressed := false
	if(oExists){
		oCompressed, err = isMediaCompressed(ctx, mediaType, oPath)
		if err != nil { return err }
		if oCompressed { Log(DEBUG, ctx, "Already compressed"); return nil }
	}
	iCompressed := false
	if oExists && iPath == oPath {
		iCompressed = oCompressed
	} else {
		iCompressed, err = isMediaCompressed(ctx, mediaType, iPath)
		if err != nil { return err }
	}
	if iCompressed { Log(DEBUG, ctx, "Already compressed"); return nil }
	oTmpPath := buildTmpFilePath(oPath)
	os.Remove(oTmpPath)
	err = compressMediaFile(ctx, mediaType, iPath, oTmpPath)
	if err != nil { return err }
	os.Remove(oPath)
	err = os.Rename(oTmpPath, oPath)
	if err != nil { return err }
	DupTime(iPath, oPath)
	return nil
}

func buildTmpFilePath(aPath string) string {
	i := strings.LastIndex(aPath, ".")
	if i == -1 { return fmt.Sprintf("%s.tmp", aPath)}
	return fmt.Sprintf("%s.tmp%s", aPath[:i], aPath[i:])
}

func isMediaCompressed(ctx Context, mediaType MediaType, aPath string) (bool, error) {
	if mediaType == IMAGE {
		return isImageCompressed(ctx, aPath)
	} else if mediaType == VIDEO {
		return isVideoCompressed(ctx, aPath)
	}
	return true, nil
}

func isImageCompressed(ctx Context, aPath string) (bool, error) {
	if ctx.ImageCompressor == 0 { return false, errors.New("You should install image magick") }
	out, err := Exec(ctx, "magick", "identify", aPath)
	if err != nil { return false, err }
	width, height, err := parseImageInfo(out)
	if err != nil { return false, errors.New("Could not parse image description") }
	return (width <= ctx.ImgMinLength && height <= ctx.ImgMinLength), nil
}

func parseImageInfo(info string) (int, int, error){
	infos := strings.Split(info, " ")
	if len(infos) < 3 { return 0, 0, errors.New("len(infos) < 3") }
	dim := infos[2]
	dims := strings.Split(dim, "x")
	if len(dims) != 2 { return 0, 0, errors.New("len(dims) != 2") }
	width, err := strconv.Atoi(dims[0])
	if err != nil { return 0, 0, err }
	height, err := strconv.Atoi(dims[1])
	if err != nil { return 0, 0, err }
	return width, height, nil
}

func isVideoCompressed(ctx Context, aPath string) (bool, error) {
	if ctx.VideoCompressor == 0 { return false, errors.New("You should install ffmpeg") }
	out, err := Exec(ctx, "ffmpeg", "-i", aPath)
	if err != nil { out = err.Error() }
	bitrate, err := parseVideoInfo(out)
	if err != nil { return false, errors.New("Could not parse video description") }
	return bitrate <= ctx.VideoBitrate, nil
}

func parseVideoInfo(info string) (int, error) {
	lines := strings.Split(info, "\n")
	for _, line := range lines {
		words := strings.Split(line, " ")
		for i, word := range words {
			if word == "kb/s," {
				return strconv.Atoi(words[i-1])
			}
		}
	}
	return 0, errors.New("bitrate not found")
}

func compressMediaFile(ctx Context, mediaType MediaType, iPath string, oPath string) error {
	if mediaType == IMAGE {
		return compressImage(ctx, iPath, oPath)
	} else if mediaType == VIDEO {
		return compressVideo(ctx, iPath, oPath)
	}
	return nil
}

func compressImage(ctx Context, iPath string, oPath string) error {
	if ctx.ImageCompressor == 0 { return errors.New("You should install image magick") }
	Log(INFO, ctx, fmt.Sprintf("Compress image: %s to %s", iPath, oPath))
	resolution := fmt.Sprintf("%dx%d>", ctx.ImgMinLength, ctx.ImgMinLength)
	_, err := Exec(ctx, "magick", "convert", iPath, "-resize", resolution, oPath)
	return err
}

func compressVideo(ctx Context, iPath string, oPath string) error {
	if ctx.VideoCompressor == 0 { return errors.New("You should install ffmpeg") }
	Log(INFO, ctx, fmt.Sprintf("Compress video: %s to %s", iPath, oPath))
	_, err := Exec(ctx, "ffmpeg", "-i", iPath, "-c:v", "libx264", "-crf", strconv.Itoa(ctx.VideoCrf), oPath)
	return err
}
