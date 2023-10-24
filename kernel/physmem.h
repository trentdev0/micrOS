#pragma once

#include <stdint.h>

#define PAGE_SIZE 4096

typedef struct { uint64_t memory_minimum, memory_maximum, memory_size, pages_maximum; } region_t;

void physmem_init();