# build
``
GOOS=windows GOARCH=amd64 go build -o bin/windows_amd64/mmc.exe src/main.go
GOOS=linux GOARCH=amd64 go build -o bin/linux_amd64/mmc src/main.go
GOOS=darwin GOARCH=amd64 go build -o bin/darwin_amd64/mmc src/main.go
```