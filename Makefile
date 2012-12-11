CFLAGS := -m32 -std=c99 -O2 -pipe -Wall -Wextra -pedantic -nostdlib -nostdinc -fno-builtin -fno-stack-protector
ASFLAGS := -felf32
LDFLAGS := -melf_i386 -belf32-i386 -Tlink.ld
SOURCES := boot.o main.o aos/port.o aos/string.o aos/screen.o aos/desc.o aos/isr.o aos/interrupt.o aos/gdt.o aos/timer.o aos/kheap.o aos/paging.o aos/panic.o aos/ordered_array.o aos/fs.o aos/initrd.o
.PHONY: clean
aliceos-kernel: $(SOURCES) link
qemu-run:
	qemu-system-i386 -m 128 -kernel vmaliceos -initrd mkinitrd/initramfs-aliceos.img
clean:
	rm -f *.o aos/*.o vmaliceos
link:
	ld $(LDFLAGS) -o vmaliceos $(SOURCES)
.s.o:
	nasm $(ASFLAGS) $<
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
