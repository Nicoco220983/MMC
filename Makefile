#    %  replacement wildcard
#    $@ to represent the full target name of the current target  
#    $? returns the dependencies that are newer than the current target  
#    $* returns the text that corresponds to % in the target     
#    $< returns the name of the first dependency 
#    $^ returns the names of all the dependencies with space as the delimiter

CC = g++
#CFLAGS = -I/usr/local/include
#CFLAGS = -std=c11 -I/usr/local/include
#CFLAGS += `GraphicsMagickWand-config --cflags --cppflags` -I/usr/local/include

#GRAPHICSMAGICK_LIBS := `GraphicsMagickWand-config --ldflags --libs`
#FFMPEG_LIBS := -L/usr/local/lib -l avfilter -l swscale -l avdevice -l avformat -l avcodec -l avutil -l swresample -lz -ldl -l:libbsd.so.0 -l rt -l bz2 -l xcb -l lzma -l Xau -l Xdmcp -l pthread -lm # -l avresample -l postproc
#LIBS := $(GRAPHICSMAGICK_LIBS) $(FFMPEG_LIBS)

SRC_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(patsubst src/%.cpp, obj/%.o, $(SRC_FILES))

obj:
	mkdir -p obj
bin:
	mkdir -p bin

obj/%.o: src/%.cpp obj
	$(CC) -c $(CFLAGS) -o $@ $<

bin/mmc: bin $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@
#	$(CC) $(OBJ_FILES) $(LIBS) -o $@

build: bin/mmc

clean:
	rm -rf obj bin