#pragma once

#include <stdint.h>
#include <stdbool.h>

#define PAGE_SIZE 4096

typedef struct { uint64_t memory_minimum, memory_maximum, memory_size, exact_memory_size, pages_maximum, status_region_count; } region_t;

typedef struct { uint8_t bytes[PAGE_SIZE]; } page_t;

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