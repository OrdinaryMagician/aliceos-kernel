/* The linker script for AliceOS kernel */
OUTPUT_FORMAT(elf32-i386)	/* I don't know how to x86_64 so deal w/ it */
ENTRY(start)
SECTIONS
{
	/* Code */
	.text 0x100000 :
	{
		code = .;
		*(.text)
		*(.rodata)
		. = ALIGN(4096);
	}
	/* All the data */
	.data :
	{
		data = .;
		*(.data)
		*(.rodata)
		. = ALIGN(4096);
	}
	/* Static vars */
	.bss :
	{
		bss = .;
		*(.bss)
		. = ALIGN(4096);
	}
	/* End of kernel goes here. */
	end = .;
	/* GCC a shit */
	/DISCARD/ :
	{
		*(.comment)
		*(.eh_frame)
		*(.note.gnu.build-id)
	}
}
