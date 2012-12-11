CFLAGS := -std=c99 -O2 -pipe -Wall -Wextra -pedantic
SOURCES := mkinitrd.o
.PHONY: clean
mkinitrd: $(SOURCES)
clean:
	rm -f *.o mkinitrd
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
