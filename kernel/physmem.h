#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define PAGE_SIZE 4096

typedef struct { uint64_t memory_minimum, memory_maximum, memory_size, exact_memory_size, pages_maximum, status_region_page_count, status_region_byte_count; } region_t;

typedef struct { uint8_t bytes[PAGE_SIZE]; } page_t;

extern region_t regions[32];
extern size_t regions_size;

static inline void setbit(uint8_t * array, uint64_t index, bool value)
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

static inline bool getbit(uint8_t * array, uint64_t index)
{
	uint64_t byte_index = index / 8;
	uint64_t bit_offset = index % 8;

	return (array[byte_index] & (1 << bit_offset)) != 0;
}

void physmem_init();

static inline void physmem_mark_allocated(uint64_t index0, uint64_t index1)
{
	uint8_t * byte = (uint8_t *)regions[index0].memory_minimum;
	setbit(byte, index1, true);
}

static inline void physmem_mark_deallocated(uint64_t index0, uint64_t index1)
{
	uint8_t * byte = (uint8_t *)regions[index0].memory_minimum;
	setbit(byte, index1, false);
}

uint64_t physmem_find_free(uint64_t index);

static inline uint64_t physmem_index_to_address(uint64_t index0, uint64_t index1)
{
	return regions[index0].memory_minimum + (index1 * PAGE_SIZE) + (regions[index0].status_region_page_count * PAGE_SIZE);
}

static inline uint64_t physmem_address_to_index(uint64_t index0, uint64_t address)
{
	if (address >= regions[index0].memory_minimum && address < regions[index0].memory_maximum)
	{
		uint64_t offset = address - regions[index0].memory_minimum;
		// Subtract only the size of the status region (byte count)
		uint64_t status_region_byte_count = regions[index0].status_region_byte_count;
		return offset / PAGE_SIZE - status_region_byte_count / PAGE_SIZE;
	}

	return 0xFFFFFFFFFFFFFFFF;
}

page_t * physmem_alloc();

static inline int physmem_free(page_t * page)
{
	return 0;
}