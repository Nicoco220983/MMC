#!/bin bash -e

FFMPEG_RELEASE=4.1

curl https://github.com/FFmpeg/FFmpeg/archive/release/${FFMPEG_RELEASE}.zip -L --output /tmp/ffmpeg.zip
unzip /tmp/ffmpeg.zip -d /usr/src
mv /usr/src/FFmpeg-release-${FFMPEG_RELEASE} /usr/src/ffmpeg
cd /usr/src/ffmpeg
./configure
make
make install