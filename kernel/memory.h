#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "thirdparty/limine.h"

typedef struct
{
	uint64_t * start;
} pagemap_t;

typedef struct
{
	/*
	 *	These values represent the address where the memory starts, ends, and
	 *	the total region's size in bytes.
	 */
	uint64_t memory_minimum, memory_maximum, memory_size;
	/* These values represent pages. */
	uint64_t pages_minumum, pages_maximum, pages_size;
	/*
	 *	How many pages are pre-allocated to contain the allocation status of
	 *	other pages?
	 */
	uint64_t status_pages_size, status_bits_size, status_bytes_size;
} region_t;

extern region_t regions[128];
extern uint64_t regions_size;

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

#define PAGE_ALIGN(VALUE) (VALUE / PAGE_SIZE) * PAGE_SIZE

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

/* Convert number of pages into bytes... */
static inline uint64_t memory_page2byte(uint64_t pages)
{
	return pages * PAGE_SIZE;
}

/* Convert an address to an index, by region index. */
static inline uint64_t memory_address2index(uint64_t index, uint64_t address)
{
	return (address - regions[index].pages_minumum) / PAGE_SIZE;
}

/* Convert an index to an address, by region index. */
static inline uint64_t memory_index2address(uint64_t index0, uint64_t index1)
{
	return regions[index0].pages_minumum + (index1 * PAGE_SIZE); 
}

/* Get only pages that aren't status pages. */
static inline uint64_t memory_offset(uint64_t index)
{
	return regions[index].pages_size - regions[index].status_pages_size;
}

/* Mark a page allocated by its index. */
static inline void memory_mark_allocated(uint64_t index0, uint64_t index1)
{
	memory_setbit((uint8_t *)regions[index0].memory_minimum, index1, true);
}

/* Mark a page free by its index. */
static inline void memory_mark_free(uint64_t index0, uint64_t index1)
{
	memory_setbit((uint8_t *)regions[index0].memory_minimum, index1, false);
}

static inline bool memory_getstatus(uint64_t index0, uint64_t index1)
{
	return memory_getbit((uint8_t *)regions[index0].memory_minimum, index1);
}

/* Is a number in between a range of two other numbers? */
static inline bool memory_inrange(uint64_t number, uint64_t minimum, uint64_t maximum)
{
	return (number >= minimum) && (number <= maximum);
}

/* Return the region index from an address. */
static inline uint64_t memory_getregion(uint64_t address)
{
	for(uint64_t i = 0; i < regions_size; i++)
	{
		if(memory_inrange(address, regions[i].memory_minimum, regions[i].memory_maximum) == true)
		{
			return i;
		}
	}

	return 0xFFFFFFFFFFFFFFFF;
}

/* Convert bytes to amount of pages, returns 1 at least. */
static inline uint64_t memory_byte2page(uint64_t bytes)
{
	uint64_t return_value = 0;
	return_value = bytes / PAGE_SIZE;

	if(return_value == 0)
	{
		return_value = 1;
		return return_value;
	}

	return return_value;
}

int memory_init();
uint64_t memory_allocate();
int memory_free(uint64_t address);