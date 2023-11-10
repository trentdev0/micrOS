#include <stdint.h>
#include <stdbool.h>

#include "virtmem.h"
#include "physmem.h"
#include "string.h"
#include "arch/amd64-pc/cpu.h"

extern char __kernel_start[];
extern char __text_start[];
extern char __text_end[];
extern char __rodata_start[];
extern char __rodata_end[];
extern char __data_start[];
extern char __data_end[];
extern char __kernel_end[];

pagemap_t kernel_pagemap;

uint64_t * virtmem_next(uint64_t * current, uint64_t index)
{
	if((current[index] & 1) != 0)
	{
		return (uint64_t *)((current[index] & PTE_ADDRESS_MASK));
	}

	uint64_t next;
	physmem_allocate(&next);
	memset((void *)next, 0, PAGE_SIZE);

	if(next == 0xFFFFFFFFFFFFFFFF)
	{
		return NULL;
	}

	next += OFFSET;

	current[index] = (next - OFFSET) | PTE_PRESENT | PTE_WRITABLE | PTE_USER;
	return (uint64_t *)(next);
}

void virtmem_unmap(pagemap_t * pagemap, uint64_t virtual_address)
{
	uint64_t pml4_index = (virtual_address & ((uint64_t)0x1FF << 39)) >> 39;
	uint64_t pml3_index = (virtual_address & ((uint64_t)0x1FF << 30)) >> 30;
	uint64_t pml2_index = (virtual_address & ((uint64_t)0x1FF << 21)) >> 21;
	uint64_t pml1_index = (virtual_address & ((uint64_t)0x1FF << 12)) >> 12;

	uint64_t * pml3 = virtmem_next(pagemap->top, pml4_index);
	uint64_t * pml2 = virtmem_next(pml3, pml3_index);
	uint64_t * pml1 = virtmem_next(pml2, pml2_index);

	pml1[pml1_index] = 0;

	asm volatile("invlpg %0" : : "m"(*(char *)virtual_address) : "memory");
}

void virtmem_map(pagemap_t * pagemap, uint64_t physical_address, uint64_t virtual_address, uint64_t flags)
{
	uint64_t pml4_index = (virtual_address & ((uint64_t)0x1FF << 39)) >> 39;
	uint64_t pml3_index = (virtual_address & ((uint64_t)0x1FF << 30)) >> 30;
	uint64_t pml2_index = (virtual_address & ((uint64_t)0x1FF << 21)) >> 21;
	uint64_t pml1_index = (virtual_address & ((uint64_t)0x1FF << 12)) >> 12;
	uint64_t * pml3 = virtmem_next(pagemap->top, pml4_index);
	uint64_t * pml2 = virtmem_next(pml3, pml3_index);
	uint64_t * pml1 = virtmem_next(pml2, pml2_index);

	pml1[pml1_index] = physical_address | flags;
}

uint64_t * virtmem_virt2pte(pagemap_t * pagemap, uint64_t address)
{
	uint64_t pml4_index = (address & ((uint64_t)0x1FFLLU << 39)) >> 39;
	uint64_t pml3_index = (address & ((uint64_t)0x1FFLLU << 30)) >> 30;
	uint64_t pml2_index = (address & ((uint64_t)0x1FFLLU << 21)) >> 21;
	uint64_t pml1_index = (address & ((uint64_t)0x1FFLLU << 12)) >> 12;

	uint64_t *pml3 = virtmem_next(pagemap->top, pml4_index);
	if(pml3 == NULL)
	{
		return NULL;
	}

	uint64_t *pml2 = virtmem_next(pml3, pml3_index);
	if(pml2 == NULL)
	{
		return NULL;
	}

	uint64_t *pml1 = virtmem_next(pml2, pml2_index);
	if(pml1 == NULL)
	{
		return NULL;
	}

	return &pml1[pml1_index];
}

uint64_t virtmem_virt2phys(pagemap_t * pagemap, uint64_t address)
{
	uint64_t * pte = virtmem_virt2pte(pagemap, address);
	return *pte & PTE_ADDRESS_MASK;
}

int virtmem_init()
{
	uint64_t top;
	physmem_allocate(&top);

	top += OFFSET;

	kernel_pagemap.top = (uint64_t *)top;

	for(uint64_t i = 0; i < ((((uint64_t)&__kernel_end + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE) - kernel_address_request.response->virtual_base; i += PAGE_SIZE)
	{
		virtmem_map(&kernel_pagemap, kernel_address_request.response->physical_base + i, kernel_address_request.response->virtual_base + i, PTE_PRESENT | PTE_WRITABLE);
	}

	for(uint64_t address = 0x1000; address < 0x100000000; address += PAGE_SIZE)
	{
		virtmem_map(&kernel_pagemap, address, address, PTE_PRESENT | PTE_WRITABLE);
		virtmem_map(&kernel_pagemap, address, address + OFFSET, PTE_PRESENT | PTE_WRITABLE | PTE_NX);
	}

	virtmem_switch(&kernel_pagemap);

	return 0;
}