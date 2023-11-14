#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "thirdparty/limine.h"

extern volatile struct limine_hhdm_request hhdm_request;

#define PAGE_SIZE 0x1000

static inline bool bitmap_get(void *bitmap, uint64_t bit)
{
	uint8_t *bitmap_byte = bitmap;
	return bitmap_byte[bit / 8] & (1 << (bit % 8));
}

static inline void bitmap_set(void *bitmap, uint64_t bit)
{
	uint8_t *bitmap_byte = bitmap;
	bitmap_byte[bit / 8] |= (1 << (bit % 8));
}

static inline void bitmap_clear(void *bitmap, uint64_t bit)
{
	uint8_t *bitmap_byte = bitmap;
	bitmap_byte[bit / 8] &= ~(1 << (bit % 8));
}

int physmem_init();
void *physmem_allocate(uint64_t pages);
void physmem_deallocate(void *address, uint64_t pages);