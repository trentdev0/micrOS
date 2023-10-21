#pragma once

#include <stdint.h>

typedef struct
{
	uint16_t limit;
	uint64_t base;
} __attribute__((packed)) descriptor_t;