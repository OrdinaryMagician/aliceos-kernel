CFLAGS := -m32 -std=c99 -O0 -pipe -Wall -Wextra -pedantic -nostdlib -nostdinc\
	  -fno-stack-protector -finline-functions -ffreestanding\
	  -funsigned-char -Wno-unused-function -Wno-unused-parameter\
	  -Ikernel/include
ifneq ($(CC),clang)
CFLAGS := $(CFLAGS) -masm=intel
else
# currently no way to make Clang use Intel syntax for inline assembly
CFLAGS := $(CFLAGS) -DATT_SYNTAX
endif
ASFLAGS := -felf32
LDFLAGS := -melf_i386 -Tkernel/link.ld
SOURCES := $(patsubst %.s,%.o,$(wildcard kernel/*.s))
SOURCES := $(SOURCES) $(patsubst %.c,%.o,$(wildcard kernel/*.c))
SOURCES := $(SOURCES) $(patsubst %.s,%.o,$(wildcard kernel/aos/*.s))
SOURCES := $(SOURCES) $(patsubst %.c,%.o,$(wildcard kernel/aos/*.c))
SOURCES := $(SOURCES) $(patsubst %.s,%.o,$(wildcard kernel/aos/fs/*.s))
SOURCES := $(SOURCES) $(patsubst %.c,%.o,$(wildcard kernel/aos/fs/*.c))
SOURCES := $(SOURCES) $(patsubst %.s,%.o,$(wildcard kernel/aos/sys/*.s))
SOURCES := $(SOURCES) $(patsubst %.c,%.o,$(wildcard kernel/aos/sys/*.c))
SOURCES := $(SOURCES) $(patsubst %.s,%.o,$(wildcard kernel/aos/vga/*.s))
SOURCES := $(SOURCES) $(patsubst %.c,%.o,$(wildcard kernel/aos/vga/*.c))
SOURCES := $(SOURCES) $(patsubst %.s,%.o,$(wildcard kernel/aos/video/*.s))
SOURCES := $(SOURCES) $(patsubst %.c,%.o,$(wildcard kernel/aos/video/*.c))
SOURCES := $(SOURCES) $(patsubst %.s,%.o,$(wildcard kernel/aos/bga/*.s))
SOURCES := $(SOURCES) $(patsubst %.c,%.o,$(wildcard kernel/aos/bga/*.c))
SOURCES := $(SOURCES) $(patsubst %.s,%.o,$(wildcard kernel/aos/desc/*.s))
SOURCES := $(SOURCES) $(patsubst %.c,%.o,$(wildcard kernel/aos/desc/*.c))
SOURCES := $(SOURCES) $(patsubst %.s,%.o,$(wildcard kernel/aos/sh/*.s))
SOURCES := $(SOURCES) $(patsubst %.c,%.o,$(wildcard kernel/aos/sh/*.c))
.PHONY: clean veryclean
aliceos-kernel: $(SOURCES) link
veryclean: clean
	rm -f vmaliceos initramfs-aliceos.img
clean:
	rm -f $(SOURCES)
link:
	ld $(LDFLAGS) -o vmaliceos $(SOURCES)
.s.o:
	yasm $(ASFLAGS) -o $@ $<
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
