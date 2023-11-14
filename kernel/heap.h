#pragma once

#include <stdint.h>

typedef struct
{
	void **first_free;
	uint64_t entry_size;
} slab_t;

typedef struct
{
	slab_t *slab;
} header_t;

typedef struct
{
	uint64_t pages;
	uint64_t size;
} metadata_t;

extern slab_t slabs[10];

static inline slab_t *heap_slab_for(uint64_t size)
{
	for(uint64_t i = 0; i < (sizeof(slabs) / sizeof(slabs[0])); i++)
	{
		slab_t *slab = &slabs[i];

		if(slab->entry_size >= size)
		{
			return slab;
		}
	}

	return NULL;
}

int heap_init();
void heap_create_slab(slab_t *slab, uint64_t entry_size);
void *heap_allocate_from_slab(slab_t *slab);
void heap_free_in_slab(slab_t *slab, void *address);
void *kmalloc(uint64_t size);
void *krealloc(void *address, uint64_t new_size);
void kfree(void *address);