#pragma once

#include <stdint.h>

#define MEMORY_OFFSET 0xFFFF800000000000

static inline uint64_t virtmem_virt2phys(uint64_t address)
{
	return address - MEMORY_OFFSET;
}

static inline uint64_t virtmem_phys2virt(uint64_t address)
{
	return address + MEMORY_OFFSET;
}

typedef struct { uint64_t * start; } pagemap_t;

static inline void virtmem_switch(pagemap_t * pagemap)
{
	__asm__ volatile("mov %0,%%cr3" : : "r"(pagemap->start) : "memory");
}

void virtmem_init();