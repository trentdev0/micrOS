#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "string.h"

#define PAGE_SIZE 4096

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

typedef struct { uint8_t bytes[PAGE_SIZE]; } page_t;

extern region_t regions[32];
extern size_t regions_size;

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

/* Mark a page allocated by its index. */
static inline void physmem_mark_allocated(uint64_t index0, uint64_t index1)
{
	physmem_setbit((uint8_t *)regions[index0].memory_minimum, index1, true);
}

/* Mark a page free by its index. */
static inline void physmem_mark_free(uint64_t index0, uint64_t index1)
{
	physmem_setbit((uint8_t *)regions[index0].memory_minimum, index1, false);
}

static inline bool physmem_getstatus(uint64_t index0, uint64_t index1)
{
	return physmem_getbit((uint8_t *)regions[index0].memory_minimum, index1);
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
void physmem_printbitmap(uint64_t index);
void physmem_printpages(uint64_t index);
uint64_t physmem_find_free(uint64_t index);
uint64_t physmem_alloc();
int physmem_free(uint64_t address);

static inline uint64_t physmem_zalloc()
{
	uint64_t address = physmem_alloc();
	memset((void *)address, 0, PAGE_SIZE);
	return address;
}