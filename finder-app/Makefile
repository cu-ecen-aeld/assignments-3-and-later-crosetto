CROSS_COMPILE ?=
CC = $(CROSS_COMPILE)gcc
CC = gcc
CFLAGS = -I.

writer: writer.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f writer
