#pragma once

#include <stdint.h>

typedef struct
{
	uint64_t * start;
} pagemap_t;

typedef struct
{
	pagemap_t pagemap;
	char name[32];
	uint64_t pid;
} process_t;