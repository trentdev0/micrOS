#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "thirdparty/limine.h"

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

extern uint64_t virtual_address_minimum, virtual_address_maximum, virtual_address_size;

extern volatile struct limine_kernel_address_request kernel_address_request;
extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_hhdm_request hhdm_request;

#define OFFSET hhdm_request.response->offset

#define PAGE_SIZE 4096
#define PAGE_ALIGN_UP(ADDRESS) (ADDRESS + (PAGE_SIZE - 1)) / PAGE_SIZE * PAGE_SIZE
#define PAGE_ALIGN_DOWN(ADDRESS) (ADDRESS / PAGE_SIZE) * PAGE_SIZE

/* Set the state of a bit in memory. */
static inline void physmem_setbit(uint8_t * array, uint64_t index, bool value)
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
static inline bool physmem_getbit(uint8_t * array, uint64_t index)
{
	uint64_t byte_index = index / 8;
	uint64_t bit_offset = index % 8;

	return (array[byte_index] & (1 << bit_offset)) != 0;
}

/* Convert number of pages into bytes... */
static inline uint64_t physmem_page2byte(uint64_t pages)
{
	return pages * PAGE_SIZE;
}

/* Convert an address to an index, by region index. */
static inline uint64_t physmem_address2index(uint64_t index, uint64_t address)
{
	return (address - regions[index].pages_minumum) / PAGE_SIZE;
}

/* Convert an index to an address, by region index. */
static inline uint64_t physmem_index2address(uint64_t index0, uint64_t index1)
{
	return regions[index0].pages_minumum + (index1 * PAGE_SIZE); 
}

/* Get only pages that aren't status pages. */
static inline uint64_t physmem_offset(uint64_t index)
{
	return regions[index].pages_size - regions[index].status_pages_size;
}

/*
 *	Mark a page as allocated by its index.
 *	Make sure we're writing the mapped memory, by adding the HHDM offset
 *	because we can't actually write to physical memory here...
 */
static inline void physmem_mark_allocated(uint64_t index0, uint64_t index1)
{
	physmem_setbit((uint8_t *)regions[index0].memory_minimum + OFFSET, index1, true);
}

/*
 *	Mark a page free by its index.
 *	Make sure we're writing the mapped memory, by adding the HHDM offset
 *	because we can't actually write to physical memory here...
 */
static inline void physmem_mark_free(uint64_t index0, uint64_t index1)
{
	physmem_setbit((uint8_t *)regions[index0].memory_minimum + OFFSET, index1, false);
}

/*
 *	Let's make sure we're reading the mapped memory, because we can't actually
 *	read or write to physical memory here.
 */
static inline bool physmem_getstatus(uint64_t index0, uint64_t index1)
{
	return physmem_getbit((uint8_t *)regions[index0].memory_minimum + OFFSET, index1);
}

/* Is a number in between a range of two other numbers? */
static inline bool physmem_inrange(uint64_t number, uint64_t minimum, uint64_t maximum)
{
	return (number >= minimum) && (number <= maximum);
}

/* Return the region index from an address. */
static inline uint64_t physmem_getregion(uint64_t address)
{
	for(uint64_t i = 0; i < regions_size; i++)
	{
		if(physmem_inrange(address, regions[i].memory_minimum, regions[i].memory_maximum) == true)
		{
			return i;
		}
	}

	return 0xFFFFFFFFFFFFFFFF;
}

/* Convert bytes to amount of pages, returns 1 at least. */
static inline uint64_t physmem_byte2page(uint64_t bytes)
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

int physmem_init();
int physmem_allocate(uint64_t * address);
int physmem_free(uint64_t address);