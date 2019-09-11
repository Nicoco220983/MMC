CC = gcc
CFLAGS = -std=c11

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) -I. -o $@ $<

OBJS = obj/mmc.o obj/compress_image.o
#OBJS = $(wildcard obj/*.o)
mmc: $(OBJS)
	$(CC) -o $@ $(OBJS)