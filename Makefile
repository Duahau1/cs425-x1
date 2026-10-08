CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -O2

.PHONY: all clean

all: pageload

pageload: pageload.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f pageload
