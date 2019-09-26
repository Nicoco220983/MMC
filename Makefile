#    %  replacement wildcard
#    $@ to represent the full target name of the current target  
#    $? returns the dependencies that are newer than the current target  
#    $* returns the text that corresponds to % in the target     
#    $< returns the name of the first dependency 
#    $^ returns the names of all the dependencies with space as the delimiter

CC = gcc
CFLAGS = -std=c11
CFLAGS += `GraphicsMagickWand-config --cflags --cppflags`
LIBS = `GraphicsMagickWand-config --ldflags --libs`

SRC_FILES := $(wildcard src/*.c)
OBJ_FILES := $(patsubst src/%.c, obj/%.o, $(SRC_FILES))

obj:
	mkdir -p obj
bin:
	mkdir -p bin

obj/%.o: src/%.c obj
	$(CC) -c $(CFLAGS) -o $@ $<

bin/mmc: bin $(OBJ_FILES)
	$(CC) $(OBJ_FILES) $(LIBS) -o $@

build: bin/mmc

clean:
	rm -rf obj bin