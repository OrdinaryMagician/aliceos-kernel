CFLAGS := -m32 -std=c99 -O0 -pipe -Wall -Wextra -Werror -pedantic -nostdlib\
	  -nostdinc -fno-stack-protector -finline-functions -ffreestanding\
	  -funsigned-char -Wno-unused-function -Wno-unused-parameter\
	  -Ikernel/include
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
KERNEL := vmaliceos
RDISK := initramfs-aliceos.img
BINFILES := $(KERNEL) $(RDISK)
.PHONY: clean veryclean
aliceos-kernel: $(SOURCES) link
veryclean: clean
	@$(foreach F,$(BINFILES),\
		if [[ -e $F ]]; then\
		  echo -e "\e[1;32m[RM]\e[0m `basename ${F}`";\
		  rm -f $F;\
		fi;)
clean:
	@$(foreach F,$(SOURCES),\
		if [[ -e $F ]]; then\
		  echo -e "\e[1;32m[RM]\e[0m `basename ${F}`";\
		  rm -f $F;\
		fi;)
link:
	@echo -e "\e[1;32m[LD]\e[0m ${KERNEL}"
	@ld $(LDFLAGS) -o $(KERNEL) $(SOURCES)
.s.o:
	@echo -e "\e[1;32m[NASM]\e[0m `basename ${<}` -> `basename ${@}`"
	@nasm $(ASFLAGS) -o $@ $<
.c.o:
	@echo -e "\e[1;32m[CLANG]\e[0m `basename ${<}` -> `basename ${@}`"
	@clang $(CFLAGS) -c -o $@ $<
