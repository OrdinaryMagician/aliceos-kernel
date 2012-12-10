CFLAGS := -m32 -std=c99 -Wall -Wextra -pedantic -nostdlib -nostdinc -fno-builtin -fno-stack-protector
ASFLAGS := -felf32
LDFLAGS := -melf_i386 -belf32-i386
SOURCES := boot.o main.o aos/port.o aos/string.o aos/screen.o
.PHONY: clean
aliceos-kernel: $(SOURCES) link
qemu-run:
	qemu-system-i386 -kernel vmaliceos
clean:
	rm -f *.o aos/*.o vmaliceos
link:
	ld $(LDFLAGS) -Tlink.ld -o vmaliceos $(SOURCES)
.s.o:
	nasm $(ASFLAGS) $<
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
