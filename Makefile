CC = gcc
CFLAGS = -std=c11
CFLAGS += `GraphicsMagickWand-config --cflags --cppflags`
LIBS = `GraphicsMagickWand-config --ldflags --libs`

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

OBJS = obj/mmc.o obj/image_compressor.o obj/mmc_context.o obj/utils.o
#OBJS = $(wildcard obj/*.o)
mmc: $(OBJS)
	$(CC) $^ $(LIBS) -o $@