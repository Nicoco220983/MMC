package mmcompress

import (
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"path"
	"runtime"
	"strings"
)

func Log(level string, ctx Context, msg interface{}) {
	if level == "DEBUG" {
		if ctx.LogLevel != "DEBUG" {
			return
		}
	} else if level == "INFO" {
		if ctx.LogLevel == "ERROR" {
			return
		}
	} else if level == "WARNING" {
		if ctx.LogLevel == "ERROR" {
			return
		}
	}
	if ctx.CurrentPath == "" {
		log.Printf("[%s] %s", level, msg)
	} else {
		log.Printf("[%s] %s: %s", level, ctx.CurrentPath, msg)
	}
}

func BinExists(ctx Context, name string) bool {
	cmd := exec.Command("which", name)
	if runtime.GOOS == "windows" {
		cmd = exec.Command("where", name)
	}
	out, err := cmd.CombinedOutput()
	if err != nil {
		return false
	}
	return len(out) > 0
}

func FileExists(name string) bool {
	_, err := os.Stat(name)
	if err != nil {
		return false
	}
	return true
}

func IsDir(name string) bool {
	info, err := os.Stat(name)
	if err != nil {
		return false
	}
	return info.IsDir()
}

func GetMediaType(name string) MediaType {
	ext := strings.ToLower(path.Ext(name))
	if len(ext) == 0 {
		return 0
	}
	ext = ext[1:]
	if ContainsStr(IMAGE_EXTS, ext) {
		return IMAGE
	}
	if ContainsStr(VIDEO_EXTS, ext) {
		return VIDEO
	}
	return 0
}

func ContainsStr(arr []string, str string) bool {
	for _, a := range arr {
		if a == str {
			return true
		}
	}
	return false
}

func CopyFile(iPath string, oPath string) error {
	info, err := os.Stat(iPath)
	if err != nil {
		return err
	}
	data, err := ioutil.ReadFile(iPath)
	if err != nil {
		return err
	}
	err = ioutil.WriteFile(oPath, data, info.Mode())
	return err
}

func MoveFile(iPath string, oPath string) error {
	err := os.Rename(iPath, oPath)
	if err != nil {
		// in some situations, the move may fail (different drive on windows)
		// in this case make a copy
		err = CopyFile(iPath, oPath)
		if err != nil {
			return err
		}
		err = os.Remove(iPath)
		if err != nil {
			return err
		}
	}
	return nil
}

func DupTime(iPath string, oPath string) error {
	info, err := os.Stat(iPath)
	if err != nil {
		return err
	}
	t := info.ModTime()
	return os.Chtimes(oPath, t, t)
}

func Exec(ctx Context, arg0 string, args ...string) (string, error) {
	Log("DEBUG", ctx, arg0+" "+strings.Join(args, " "))
	cmd := exec.Command(arg0, args...)
	out, err := cmd.CombinedOutput()
	if err != nil {
		return "", fmt.Errorf("%s\n%s", err.Error(), out)
	}
	return string(out), nil
}
