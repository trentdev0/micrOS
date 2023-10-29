#include <stdint.h>

#include "virtmem.h"
#include "physmem.h"
#include "string.h"
#include "stream.h"
#include "thirdparty/limine.h"

extern char kernel_end[];

volatile struct limine_hhdm_request hhdm_request = {
	.id = LIMINE_HHDM_REQUEST,
	.revision = 0
};

volatile struct limine_kernel_address_request kernel_address_request = {
	.id = LIMINE_KERNEL_ADDRESS_REQUEST,
	.revision = 0
};

pagemap_t kernel_pagemap;

uint64_t * virtmem_next(uint64_t * current, uint64_t index)
{
	if((current[index] & 1) != 0)
	{
		return (uint64_t *)((current[index] & PTE_ADDRESS_MASK) + hhdm_request.response->offset);
	}

	uint64_t next = physmem_zalloc();

	if(next == 0xFFFFFFFFFFFFFFFF)
	{
		return NULL;
	}

	current[index] = next | PTE_PRESENT | PTE_WRITABLE | PTE_USER;
	return (uint64_t *)(next + hhdm_request.response->offset);
}

void virtmem_unmap(pagemap_t * pagemap, uint64_t virtual_address)
{
	uint64_t pml4_index = (virtual_address & ((uint64_t)0x1FF << 39)) >> 39;
	uint64_t pml3_index = (virtual_address & ((uint64_t)0x1FF << 30)) >> 30;
	uint64_t pml2_index = (virtual_address & ((uint64_t)0x1FF << 21)) >> 21;
	uint64_t pml1_index = (virtual_address & ((uint64_t)0x1FF << 12)) >> 12;

	uint64_t * pml3 = virtmem_next(pagemap->start, pml4_index);
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
	uint64_t * pml3 = virtmem_next(pagemap->start, pml4_index);
	uint64_t * pml2 = virtmem_next(pml3, pml3_index);
	uint64_t * pml1 = virtmem_next(pml2, pml2_index);

	pml1[pml1_index] = physical_address | flags;
}

void virtmem_init()
{
	stream_printf(current_stream, "[VIRTMEM]:\033[15GHere's the start of our kernel in physical memory (address=\"0x%lx\")!\r\n", kernel_address_request.response->physical_base);
	stream_printf(current_stream, "[VIRTMEM]:\033[15GHere's the start of our kernel in virtual memory (address=\"0x%lx\")!\r\n", kernel_address_request.response->virtual_base);
	stream_printf(current_stream, "[VIRTMEM]:\033[15GHere's the HHDM offset (address=\"0x%lx\")!\r\n", hhdm_request.response->offset);

	kernel_pagemap.start = (uint64_t *)physmem_zalloc();

	kernel_pagemap.start = (void *)kernel_pagemap.start + hhdm_request.response->offset;

	uint64_t virtual = kernel_address_request.response->virtual_base;
	uint64_t physical = kernel_address_request.response->physical_base;
	uint64_t length = ((((uint64_t)kernel_end + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE) - virtual;

	for(uint64_t i = 0; i < length; i += PAGE_SIZE)
	{
		virtmem_map(&kernel_pagemap, physical + i, virtual + i, PTE_PRESENT | PTE_WRITABLE);
	}

	for(uint64_t i = PAGE_SIZE; i < 0x100000000; i += PAGE_SIZE)
	{
		virtmem_map(&kernel_pagemap, i, i, PTE_PRESENT | PTE_WRITABLE);
		virtmem_map(&kernel_pagemap, i, i + hhdm_request.response->offset, PTE_PRESENT | PTE_WRITABLE);
	}

	virtmem_switch(&kernel_pagemap);

	stream_printf(current_stream, "[VIRTMEM]:\033[15GSuccessfully initialized virtual memory manager!\r\n");
}