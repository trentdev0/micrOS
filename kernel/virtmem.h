#pragma once

#include <stdint.h>

#include "thirdparty/limine.h"

#define MEMORY_OFFSET 0xFFFF800000000000

enum
{
	PTE_ADDRESS_MASK = 0x000FFFFFFFFFF000,
	PTE_PRESENT = (1ull << 0ull),
	PTE_WRITABLE = (1ull << 1ull),
	PTE_USER = (1ull << 2ull),
	PTE_NX = (1ull << 63ull)
};

typedef struct { uint64_t * start; } pagemap_t;

extern volatile struct limine_hhdm_request hhdm_request;
extern volatile struct limine_hhdm_request hhdm_request;

static inline void virtmem_switch(pagemap_t *pagemap)
{
	uint64_t * top = (void *)pagemap->start - hhdm_request.response->offset;
	asm volatile("mov %0, %%cr3" : : "r"(top) : "memory");
}

void virtmem_init();
uint64_t * virtmem_next(uint64_t * current, uint64_t index);
void virtmem_map(pagemap_t * pagemap, uint64_t physical_address, uint64_t virtual_address, uint64_t flags);
void virtmem_unmap(pagemap_t * pagemap, uint64_t virtual_address);