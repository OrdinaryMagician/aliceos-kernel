CFLAGS := -std=c99 -O2 -pipe -Wall -Wextra -pedantic -funsigned-char
.PHONY: clean
mkrd:
	$(CC) $(CFLAGS) -o bin/mkrd mkrd.c
	$(CC) $(CFLAGS) -o bin/dmprd dmprd.c
clean:
	rm -f bin/*
