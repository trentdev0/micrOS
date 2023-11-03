/* Tell the linker that we want an x86_64 ELF64 output file */
OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

/* We want the symbol _start to be our entry point */
ENTRY(_start)

PHDRS
{
	text PT_LOAD FLAGS((1 << 0) | (1 << 2));
	rodata PT_LOAD FLAGS((1 << 2));
	data PT_LOAD FLAGS((1 << 1) | (1 << 2));
	dynamic PT_DYNAMIC FLAGS((1 << 1) | (1 << 2));
}

SECTIONS
{
	. = 0xffffffff80000000;

	__text_start = .;

	.text : {
		*(.text .text.*)
	} :text

	__text_end = .;

	. += CONSTANT(MAXPAGESIZE);

	__rodata_start = .;

	.rodata : {
		*(.rodata .rodata.*)
	} :rodata

	__rodata_end = .;

	. += CONSTANT(MAXPAGESIZE);

	__data_start = .;

	.data : {
		*(.data .data.*)
	} :data

	.dynamic : {
		*(.dynamic)
	} :data :dynamic

	.bss : {
		*(.bss .bss.*)
		*(COMMON)
	} :data

	__data_end = .;

	__kernel_end = .;

	/DISCARD/ : {
		*(.eh_frame)
		*(.note .note.*)
	}
}