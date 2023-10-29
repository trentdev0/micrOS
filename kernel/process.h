#pragma once

#include "virtmem.h"

typedef struct
{
	pagemap_t pagemap;
	char name[32];
	uint64_t pid;
} process_t;