# build
```
GOOS=windows GOARCH=amd64 go build -o bin/windows_amd64/mmcompress.exe src/cli/main.go
GOOS=linux GOARCH=amd64 go build -o bin/linux_amd64/mmcompress src/cli/main.go
GOOS=darwin GOARCH=amd64 go build -o bin/darwin_amd64/mmcompress src/cli/main.go

GOOS=windows GOARCH=amd64 go build -o bin/windows_amd64/mmcompress_wui.exe src/wui/main.go
GOOS=linux GOARCH=amd64 go build -o bin/linux_amd64/mmcompress_wui src/wui/main.go
GOOS=darwin GOARCH=amd64 go build -o bin/darwin_amd64/mmcompress_wui src/wui/main.go
```
