package main

import (
	"fmt"
	"log"
	"os"
	"strconv"

	"../mmcompress"
)

const USAGE = `mmcompress [OPTIONS]
  -h/--help: Print this help.
  -i/--input PATH: Input path to compress (file or dir).
  -o/--output PATH: compression destination.
  --overwrite: Overwrite the input content.
  --force: Force re-compression, even if not necessary).
  -q/--quiet: Minimal logs.
  -v/--verbose: Maximal logs.
  --tmp-dir DIR: directory where computation take place.
  -c/--compression-level LEVEL: Compression level (Possible values: S, [M], L).
  --img-min-length: Image minimum length.
  --vid-bitrate: Video bitrate.
  --vid-crf: Video CRF.
`

func main() {

	ctx := mmcompress.NewContext()

	i, nbArgs := 1, len(os.Args)
	for i < nbArgs {
		arg := os.Args[i]
		if arg == "-h" || arg == "--help" {
			fmt.Printf(USAGE)
			os.Exit(0)
		} else if arg == "-i" || arg == "--input" {
			i++
			ctx.InputPath = os.Args[i]
		} else if arg == "-o" || arg == "--output" {
			i++
			ctx.OutputPath = os.Args[i]
		} else if arg == "--overwrite" {
			ctx.OutputMode = "OVERWRITE"
		} else if arg == "--force" {
			ctx.Force = true
		} else if arg == "-v" || arg == "--verbose" {
			ctx.LogLevel = "DEBUG"
		} else if arg == "-q" || arg == "--quiet" {
			ctx.LogLevel = "ERROR"
		} else if arg == "--tmp-dir" {
			i++
			ctx.TmpPath = os.Args[i]
		} else if arg == "-c" || arg == "--compression-level" {
			i++
			ctx.CompressionLevel = os.Args[i]
		} else if arg == "--img-min-length" {
			i++
			ctx.ImgMinLength = parseIntArg(os.Args[i])
		} else if arg == "--vid-bitrate" {
			i++
			ctx.VideoBitrate = parseIntArg(os.Args[i])
		} else if arg == "--vid-crf" {
			i++
			ctx.VideoCrf = parseIntArg(os.Args[i])
		} else {
			fmt.Printf("%s\n", USAGE)
			log.Fatalf("Unknown arg: %s\n", arg)
		}
		i++
	}

	err := mmcompress.MMC(ctx)
	if err != nil {
		panic(err)
	}
}

func parseIntArg(arg string) int {
	res, err := strconv.Atoi(arg)
	if err != nil {
		panic(err)
	}
	return res
}
