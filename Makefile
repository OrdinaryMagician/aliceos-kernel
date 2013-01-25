CFLAGS := -m32 -std=c99 -O0 -pipe -Wall -Wextra -pedantic -nostdlib -nostdinc -fno-stack-protector -finline-functions -ffreestanding -funsigned-char -Wno-unused-function -Wno-unused-parameter -Ikernel/include
ASFLAGS := -felf32
LDFLAGS := -melf_i386 -Tkernel/link.ld
KMAIN_C := $(patsubst %.c,%.o,$(wildcard kernel/*.c))
KMAIN_S := $(patsubst %.s,%.o,$(wildcard kernel/*.s))
AOS_C := $(patsubst %.c,%.o,$(wildcard kernel/aos/*.c))
AOS_S := $(patsubst %.s,%.o,$(wildcard kernel/aos/*.s))
SOURCES := $(KMAIN_S) $(KMAIN_C) $(AOS_S) $(AOS_C)
.PHONY: clean
aliceos-kernel: $(SOURCES) link
clean:
	rm -f $(SOURCES) vmaliceos
link:
	ld $(LDFLAGS) -o vmaliceos $(SOURCES)
.s.o:
	nasm $(ASFLAGS) $<
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
