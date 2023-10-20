CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

all: cae_util

cae_util: src/cae_util.c
	$(CC) $(CFLAGS) src/cae_util.c -o bin/cae_util -lpci -lncurses

debug: CFLAGS += -g
debug: cae_util

clean:
	rm -f cae_util
