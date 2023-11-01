#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "thirdparty/limine.h"

typedef struct
{
	uint64_t * start;
} pagemap_t;

extern pagemap_t pagemap;

enum
{
	PTE_ADDRESS_MASK = 0x000FFFFFFFFFF000,
	PTE_PRESENT = (1ull << 0ull),
	PTE_WRITABLE = (1ull << 1ull),
	PTE_USER = (1ull << 2ull),
	PTE_NX = (1ull << 63ull)
};

/*
 *	All processes, including the kernel, have their own pagemap.
 */
enum
{
	/* Space reserved in virtual memory for a single userland program. */
	RANGE_USERLAND_MINIMUM = 0x0000000000000000,
	RANGE_USERLAND_MAXIMUM = 0xFFFFFFFF7FFFFFFF,
	/* Space reserved in virtual memory for the kernel. */
	RANGE_KERNEL_START = 0xFFFFFFFF80000000,
	RANGE_KERNEL_END = 0xFFFFFFFFFFFFFFFE
};

extern volatile struct limine_hhdm_request hhdm_request;

#define OFFSET hhdm_request.response->offset

#define PAGE_SIZE 4096

/* Set the state of a bit in memory. */
static inline void memory_setbit(uint8_t * array, uint64_t index, bool value)
{
	uint64_t byte_index = index / 8;
	uint64_t bit_offset = index % 8;

	if(value)
	{
		array[byte_index] |= (1 << bit_offset);
	}
	else
	{
		array[byte_index] &= ~(1 << bit_offset);
	}
}

/* Get the state of a bit in memory. */
static inline bool memory_getbit(uint8_t * array, uint64_t index)
{
	uint64_t byte_index = index / 8;
	uint64_t bit_offset = index % 8;

	return (array[byte_index] & (1 << bit_offset)) != 0;
}

uint64_t * memory_virt2pte(pagemap_t * pagemap, uint64_t address);
uint64_t memory_virt2phys(pagemap_t * pagemap, uint64_t address);
uint64_t * memory_next(uint64_t * current, uint64_t index);
void memory_unmap(pagemap_t * pagemap, uint64_t virtual_address);
int memory_map(pagemap_t * pagemap, uint64_t physical_address, uint64_t virtual_address, uint64_t flags);
int memory_init();