CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

all: system_info

system_info: system_info.c
	$(CC) $(CFLAGS) system_info.c -o system_info -lpci -lncurses

clean:
	rm -f system_info
