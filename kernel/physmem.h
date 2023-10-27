#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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

static inline bool physmem_getbit(uint8_t * array, uint64_t index)
{
	uint64_t byte_index = index / 8;
	uint64_t bit_offset = index % 8;

	return (array[byte_index] & (1 << bit_offset)) != 0;
}

void physmem_init();
void physmem_printbitmap(uint64_t index);