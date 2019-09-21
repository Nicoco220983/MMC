CC = gcc
CFLAGS = -std=c11
CFLAGS += `GraphicsMagickWand-config --cflags --cppflags`
CFLAGS += `GraphicsMagickWand-config --ldflags --libs`

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) -I. -o $@ $<

OBJS = obj/mmc.o obj/compress_image.o
#OBJS = $(wildcard obj/*.o)
mmc: $(OBJS)
	$(CC) -o $@ $(OBJS)