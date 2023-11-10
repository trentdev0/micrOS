#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "physmem.h"

typedef struct
{
	/* Is the target present? */
	uint8_t p : 1;
	/* Are we allowed to write to the target? */
	uint64_t rw : 1;
	/* Can ring3 interact with the target? */
	uint8_t us : 1;
	uint8_t pwt : 1;
	uint8_t pcd : 1;
	uint8_t a : 1;
	uint8_t avl0 : 1;
	uint8_t rsvd : 1;
	uint8_t avl1 : 4;
	uint64_t zerome : 40;
	uint16_t avl2 : 11;
	uint8_t xd : 1;
} page_level_t;

typedef struct
{
	/* Is the target present? */
	uint8_t p : 1;
	/* Are we allowed to write to the target? */
	uint8_t rw : 1;
	/* Can ring3 interact with the target? */
	uint8_t us : 1;
	uint8_t pwt : 1;
	uint8_t pcd : 1;
	uint8_t a : 1;
	uint8_t d : 1;
	uint8_t pat : 1;
	uint8_t g : 1;
	uint8_t avl0 : 4;
	uint64_t zerome : 40;
	uint16_t pk : 11;
	uint8_t xd : 1;
} page_table_t;

typedef struct
{
	uint64_t * top;
} pagemap_t;

#define PTE_ADDRESS_MASK 0x000FFFFFFFFFF000
#define PTE_PRESENT (1ull << 0ull)
#define PTE_WRITABLE (1ull << 1ull)
#define PTE_USER (1ull << 2ull)
#define PTE_NX (1ull << 63ull)

static inline void virtmem_switch(pagemap_t *pagemap)
{
	asm volatile("mov %0, %%cr3" : : "r"((void *)pagemap->top - OFFSET) : "memory");
}

int virtmem_init();