# MMC
Mass Media Compressor

# Docker builder

## Build the builder
`docker build -t mmc/builder:latest -f builder.Dockerfile .`

## Run the builder
`docker run --rm -v "$PWD:/MMC" -w "/MMC" --entrypoint "make" mmc/builder:latest`
