#pragma once

#include <stddef.h>
#include <stdint.h>

#include "../../string.h"

typedef struct
{
	uint16_t limit;
	uint16_t base_low16;
	uint8_t base_mid8;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high8;
} gdt_t;

extern gdt_t gdt[11];

static inline void gdt_register(int index, uint16_t limit, uint32_t base, uint8_t access, uint8_t granularity)
{
	gdt[index].limit = limit;
	gdt[index].base_low16 = (uint16_t)(base & 0xFFFF);
	gdt[index].base_mid8 = (uint8_t)((base >> 16) & 0xFF);
	gdt[index].access = access;
	gdt[index].granularity = granularity;
	gdt[index].base_high8 = (uint8_t)((base >> 24) & 0xFF);
}

static inline void gdt_wipe()
{
	memset(&gdt, 0, sizeof(gdt));
}

int gdt_init();
void gdt_flush();