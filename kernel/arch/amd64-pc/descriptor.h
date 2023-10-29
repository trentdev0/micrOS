#pragma once

#include <stdint.h>

/* A descriptor pointer, useful for GDT (Global Descriptor Table), and IDT (Interrupt Descriptor Table). */
typedef struct
{
	uint16_t limit;
	uint64_t base;
} __attribute__((packed)) descriptor_t;