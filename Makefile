CFLAGS := -m32 -std=c99 -O0 -pipe -Wall -Wextra -pedantic -nostdlib -nostdinc -fno-stack-protector -finline-functions -ffreestanding -funsigned-char -Wno-unused-function -Wno-unused-parameter -Ikernel/include
ASFLAGS := -felf32
LDFLAGS := -melf_i386 -Tkernel/link.ld
SOURCES := $(patsubst %.c,%.o,$(wildcard kernel/*.c))
SOURCES := $(SOURCES) $(patsubst %.s,%.o,$(wildcard kernel/*.s))
SOURCES := $(SOURCES) $(patsubst %.c,%.o,$(wildcard kernel/aos/*.c))
SOURCES := $(SOURCES) $(patsubst %.s,%.o,$(wildcard kernel/aos/*.s))
SOURCES := $(SOURCES) $(patsubst %.c,%.o,$(wildcard kernel/aos/fs/*.c))
SOURCES := $(SOURCES) $(patsubst %.s,%.o,$(wildcard kernel/aos/fs/*.s))
SOURCES := $(SOURCES) $(patsubst %.c,%.o,$(wildcard kernel/aos/sys/*.c))
SOURCES := $(SOURCES) $(patsubst %.s,%.o,$(wildcard kernel/aos/sys/*.s))
SOURCES := $(SOURCES) $(patsubst %.c,%.o,$(wildcard kernel/aos/vga/*.c))
SOURCES := $(SOURCES) $(patsubst %.s,%.o,$(wildcard kernel/aos/vga/*.s))
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
