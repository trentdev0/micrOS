#pragma once

#include <stddef.h>
#include <stdint.h>

#include "string.h"

typedef struct
{
	uint16_t limit;
	uint16_t base_low16;
	uint8_t base_mid8;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high8;
} gdt_t;

typedef struct
{
	uint16_t length;
	uint16_t base_low16;
	uint8_t base_mid8;
	uint8_t flags0;
	uint8_t flags1;
	uint8_t base_high8;
	uint32_t base_upper32;
	uint32_t reserved;
} tss_t;

/* The package containing the GDT entries and the TSS. */
struct _entries {gdt_t null0; gdt_t kernel16code; gdt_t kernel16data; gdt_t kernel32code; gdt_t kernel32data; gdt_t kernel64code; gdt_t kernel64data; gdt_t null1; gdt_t null2; tss_t tss};

extern struct _entries entries;

static inline void gdt_wipe()
{
	memset(&entries, 0, sizeof(entries));
}