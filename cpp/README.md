# MMC
Mass Media Compressor

# Docker builder

## Build the builder & runner
`docker build -t mmc/builder:latest -f Dockerfile.builder .`
`docker build -t mmc/runner:latest -f Dockerfile.runner .`

## Run the builder
`docker run --rm -v "$PWD:/MMC" -w "/MMC" --entrypoint make mmc/builder:latest`

## Run MMC
`docker run --rm -v "$PWD:/MMC" -w "/MMC" --entrypoint bin/mmc mmc/runner:latest -i test -o res`