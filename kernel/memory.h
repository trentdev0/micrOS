#pragma once

#include <stdint.h>

#define MEM_VIRT_OFFSET 0xFFFF800000000000

static inline uint64_t virt_to_phys(uint64_t virt_address)
{
	return virt_address - MEM_VIRT_OFFSET;
}

static inline uint64_t phys_to_virt(uint64_t phys_address)
{
	return phys_address + MEM_VIRT_OFFSET;
}