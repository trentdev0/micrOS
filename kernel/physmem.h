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

void physmem_init();

static inline void physmem_mark_allocated(uint64_t index)
{
	uint8_t * byte = (uint8_t *)regions[index].memory_minimum;
	setbit(byte, index, true);
}

static inline void physmem_mark_deallocated(uint64_t index)
{
	uint8_t * byte = (uint8_t *)regions[index].memory_minimum;
	setbit(byte, index, false);
}