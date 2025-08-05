CROSS_COMPILE ?= ""
CC = $(CROSS_COMPILE)gcc
CFLAGS = -I.

writer: writer.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f writer
