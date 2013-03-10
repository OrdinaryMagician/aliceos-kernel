CFLAGS := -std=c99 -O2 -pipe -Wall -Wextra -Werror -pedantic -funsigned-char
.PHONY: clean
mkrd:
	clang $(CFLAGS) -o bin/mkrd mkrd.c
	clang $(CFLAGS) -o bin/dmprd dmprd.c
clean:
	rm -f bin/*
