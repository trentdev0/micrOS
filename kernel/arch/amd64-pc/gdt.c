#include "gdt.h"
#include "descriptor.h"

struct _entries entries;

void gdt_flush()
{
	descriptor_t descriptor;
	descriptor.limit = sizeof(entries) - 1;
	descriptor.base = (uint64_t)&entries;

	asm volatile ("lgdt %0" :: "m"(descriptor) : "memory");
}