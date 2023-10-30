#pragma once

#include <stdint.h>
#include <stddef.h>

#include "thirdparty/limine.h"

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

uint64_t virtmem_virt2phys(pagemap_t * pagemap, uint64_t virt);
uint64_t * virtmem_virt2pte(pagemap_t * pagemap, uint64_t virt);
int virtmem_init();
uint64_t * virtmem_next(uint64_t * current, uint64_t index);
int virtmem_map(pagemap_t * pagemap, uint64_t physical_address, uint64_t virtual_address, uint64_t flags);
void virtmem_unmap(pagemap_t * pagemap, uint64_t virtual_address);