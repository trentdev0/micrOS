#include <stdint.h>
#include <stddef.h>

#include "heap.h"
#include "physmem.h"
#include "string.h"

slab_t slabs[10];

void heap_create_slab(slab_t *slab, uint64_t entry_size)
{
	slab->first_free = physmem_allocate(1) + hhdm_request.response->offset;
	slab->entry_size = entry_size;

	uint64_t header_offset = (sizeof(header_t) + (entry_size - 1)) / entry_size * entry_size;
	uint64_t available_size = PAGE_SIZE - header_offset;

	header_t *slab_pointer = (header_t *)slab->first_free;
	slab_pointer->slab = slab;
	slab->first_free = (void **)((void *)slab->first_free + header_offset);

	void **array = (void **)slab->first_free;
	uint64_t max = available_size / entry_size - 1;
	uint64_t fact = entry_size / sizeof(void *);

	for(uint64_t i = 0; i < max; i++)
	{
		array[i * fact] = &array[(i + 1) * fact];
	}

	array[max * fact] = NULL;
}

void *heap_allocate_from_slab(slab_t *slab)
{
	if(slab->first_free == NULL)
	{
		heap_create_slab(slab, slab->entry_size);
	}

	void **old_free = slab->first_free;
	slab->first_free = *old_free;
	memset(old_free, 0, slab->entry_size);

	return old_free;
}

void heap_free_in_slab(slab_t *slab, void *address)
{
	if(address == NULL)
	{
		return;
	}

	void **new_head = address;
	*new_head = slab->first_free;
	slab->first_free = new_head;
}

int heap_init()
{
	heap_create_slab(&slabs[0], 8);
	heap_create_slab(&slabs[1], 16);
	heap_create_slab(&slabs[2], 24);
	heap_create_slab(&slabs[3], 32);
	heap_create_slab(&slabs[4], 48);
	heap_create_slab(&slabs[5], 64);
	heap_create_slab(&slabs[6], 128);
	heap_create_slab(&slabs[7], 256);
	heap_create_slab(&slabs[8], 512);
	heap_create_slab(&slabs[9], 1024);

	return 0;
}

void *kmalloc(uint64_t size)
{
	slab_t *slab = heap_slab_for(size);
	if(slab != NULL)
	{
		return heap_allocate_from_slab(slab);
	}

	uint64_t page_count = (size + (PAGE_SIZE - 1)) / PAGE_SIZE;
	void *return_value = physmem_allocate(page_count + 1);

	if(return_value == NULL)
	{
		return NULL;
	}

	return_value += hhdm_request.response->offset;
	metadata_t *metadata = (metadata_t *)return_value;

	metadata->pages = page_count;
	metadata->size = size;

	return return_value + PAGE_SIZE;
}

void *krealloc(void *address, uint64_t new_size)
{
	if(address == NULL)
	{
		return kmalloc(new_size);
	}

	if(((uintptr_t)address & 0xFFF) == 0)
	{
		metadata_t *metadata = (metadata_t *)(address - PAGE_SIZE);
		if(((metadata->size + (PAGE_SIZE - 1)) / PAGE_SIZE) == ((new_size + (PAGE_SIZE - 1)) / PAGE_SIZE))
		{
			metadata->size = new_size;
			return address;
		}

		void *new_address = kmalloc(new_size);
		if(new_address == NULL)
		{
			return NULL;
		}

		if(metadata->size > new_size)
		{
			memcpy(new_address, address, new_size);
		}
		else
		{
			memcpy(new_address, address, metadata->size);
		}

		kfree(address);
		return new_address;
	}

	header_t *slab_header = (header_t *)((uintptr_t)address & ~0xFFF);
	slab_t *slab = slab_header->slab;

	if(new_size > slab->entry_size)
	{
		void *new_address = kmalloc(new_size);
		if(new_address == NULL)
		{
			return NULL;
		}

		memcpy(new_address, address, slab->entry_size);
		heap_free_in_slab(slab, address);
		return new_address;
	}

	return address;
}

void kfree(void *address)
{
	if(address == NULL)
	{
		return;
	}

	if(((uintptr_t)address & 0xFFF) == 0)
	{
		metadata_t *metadata = (metadata_t *)(address - PAGE_SIZE);
		physmem_deallocate((void *)metadata - hhdm_request.response->offset, metadata->pages + 1);
		return;
	}

	header_t *slab_header = (header_t *)((uintptr_t)address & ~0xFFF);
	heap_free_in_slab(slab_header->slab, address);
}