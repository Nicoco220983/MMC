package main

import (
	"fmt"
	"os"
	"strings"
	"strconv"
	"./mmc"
)

const USAGE = `mmc [OPTIONS]
  -h/--help: Print this help.
  -i/--input PATH: Input path to compress (file or dir).
  -o/--output PATH: compression destination.
  --overwrite: Overwrite the input content.
  -c/--compression-level LEVEL: Compression level (Possible values: S, [M], L).
  --img-min-length: Image minimum length.
  --vid-bitrate: Video bitrate.
  --vid-crf: Video CRF.
  -q/--quiet: Minimal logs.
  -v/--verbose: Maximal logs.
`

func main() {

	ctx := mmc.NewContext()

	i, nbArgs := 1, len(os.Args)
	for i < nbArgs {
		arg := os.Args[i]
		if arg=="-h" || arg=="--help" {
			fmt.Printf(USAGE)
			os.Exit(0)
		} else if arg=="-i" || arg=="--input" {
			i++; ctx.InputPath = os.Args[i]
		} else if arg=="-o" || arg=="--output" {
			i++; ctx.OutputPath = os.Args[i]
		} else if(arg == "--overwrite"){
			ctx.OutputMode = mmc.OVERWRITE
		} else if arg=="-c" || arg=="--compression-level" {
			i++; val := os.Args[i]
			if strings.EqualFold(val, "S") {
				ctx.CompressionLevel = mmc.SMALL
			} else if strings.EqualFold(val, "M") {
				ctx.CompressionLevel = mmc.MEDIUM
			} else if strings.EqualFold(val, "L") {
				ctx.CompressionLevel = mmc.LARGE
			} else {
				panic(fmt.Sprintf("Unknown value for compression-level: %s", val))
			}
		} else if arg=="--img-min-length" {
			i++; ctx.ImgMinLength = parseIntArg(os.Args[i])
		} else if arg=="--vid-bitrate" {
			i++; ctx.VideoBitrate = parseIntArg(os.Args[i])
		} else if arg=="--vid-crf" {
			i++; ctx.VideoCrf = parseIntArg(os.Args[i])
		} else if(arg=="-v" || arg=="--verbose"){
			ctx.LogLevel = mmc.DEBUG
		} else if(arg=="-q" || arg=="--quiet"){
			ctx.LogLevel = mmc.ERROR
		} else {
			fmt.Printf("%s\n", USAGE)
			panic(fmt.Sprintf("[ERROR] Unknown arg: %s\n", arg))
		}
		i++
	}

	err := mmc.MMC(ctx)
	if err != nil { panic(err) }
}

func parseIntArg(arg string) int {
	res, err := strconv.Atoi(arg)
	if err != nil { panic(err) }
	return res
}