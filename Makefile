CC=gcc
CFLAGS=-std=gnu17 -Wall -Wextra -Werror -Os

all: caesar

caesar: main.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf caesar