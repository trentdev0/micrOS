#include "gdt.h"
#include "descriptor.h"

gdt_t gdt[11];

/* Set up the GDT (Global Descriptor Table). */
int gdt_init()
{
	/* Wipe the GDT. */
	gdt_wipe();
	/* Null descriptor. */
	gdt_register(0, 0, 0, 0, 0);
	/* Kernel code (16). */
	gdt_register(1, 0xFFFF, 0, 0b10011010, 0);
	/* Kernel data (16). */
	gdt_register(2, 0xFFFF, 0, 0b10010010, 0);
	/* Kernel code (32). */
	gdt_register(3, 0xFFFF, 0, 0b10011010, 0b11001111);
	/* Kernel data (32). */
	gdt_register(4, 0xFFFF, 0, 0b10010010, 0b11001111);
	/* Kernel code (64). */
	gdt_register(5, 0, 0, 0b10011010, 0b00100000);
	/* Kernel data (64). */
	gdt_register(6, 0, 0, 0b10010010, 0);
	/* User code (64). */
	gdt_register(9, 0, 0, 0b11111010, 0b00100000);
	/* Kernel data (64). */
	gdt_register(10, 0, 0, 0b11110010, 0);
	/* Flush the GDT structure down to the CPU. */
	gdt_flush();

	return 0;
}

/* Flush the GDT structure down to the CPU... */
void gdt_flush()
{
	descriptor_t descriptor;
	descriptor.limit = sizeof(gdt) - 1;
	descriptor.base = (uint64_t)&gdt;

	asm volatile ("lgdt %0" :: "m"(descriptor) : "memory");
}