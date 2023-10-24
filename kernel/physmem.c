#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "limine.h"
#include "stream.h"
#include "physmem.h"

volatile struct limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0
};

region_t regions[32];
size_t regions_size = 0;

void physmem_init()
{
	struct limine_memmap_response * memmap = memmap_request.response;
	struct limine_memmap_entry ** entries = memmap->entries;

	for(uint64_t i = 0; i < memmap->entry_count; i++)
	{
		struct limine_memmap_entry * entry = entries[i];

		stream_printf(current_stream, "[MEMMAP]:\033[15GFound a memmap entry (base=\"0x%lx\", length=\"0x%lx\", type=\"%ld\")!\r\n", entry->base, entry->length, entry->type);

		switch (entry->type)
		{
		case LIMINE_MEMMAP_USABLE:
			/* The value memory_minimum represents the start of the region of free memory. */
			regions[regions_size].memory_minimum = entry->base;
			/* The value memory_size represents the exact amount of free memory in bytes. */
			regions[regions_size].memory_size = entry->length;
			/* The value memory_maximum represents the end of the region of free memory. */
			regions[regions_size].memory_maximum = entry->base + entry->length;
			/* The value pages_maximum represents the maximum amount of 4KiB pages that can be achieved using this region of free memory. */
			regions[regions_size].pages_maximum = regions[regions_size].memory_size / PAGE_SIZE;
			/* The value exact_memory_size represents the exact amount of memory in bytes that can be made using the maximum amount of pages. */
			regions[regions_size].exact_memory_size = regions[regions_size].pages_maximum * PAGE_SIZE;
			/* The value status_region_byte_count represents the amount of bytes that are needed to represent the status of all pages in the region. */
			regions[regions_size].status_region_byte_count = (regions[regions_size].pages_maximum + 7) / 8;
			/*
			 *	The value status_region_page_count represents the amount of pages that are automatically allocated for knowing the status of
			 *	of all pages in the region of memory.
			 */
			regions[regions_size].status_region_page_count = ((regions[regions_size].pages_maximum + 7) / 8) / PAGE_SIZE;
			regions_size++;
			break;
		}
	}

	for(typeof(regions_size) i = 0; i < regions_size; i++)
	{
		stream_printf(current_stream, "[PHYSMEM]:\033[15GHere's an chunk of free & usable memory (min=\"0x%lx\", max=\"0x%lx\", maxpages=\"%ld\")!\r\n", regions[i].memory_minimum, regions[i].memory_maximum, regions[i].pages_maximum);
	}
}

/* Return an index in status region for page that is free... */
uint64_t physmem_find_free(uint64_t index)
{
	uint8_t * byte = (uint8_t *)regions[index].memory_minimum;

	for(uint64_t i = 0; i < (regions[index].memory_size / 8); i++)
	{
		if(byte[i] != 0xFF)
		{
			for(int j = 0; j < 8; j++)
			{
				if((byte[i] & (1 << j)) == 0)
				{
					return i * 8 + j;
				}
			}
		}
	}

	/* In this case, 0xFFFFFFFFFFFFFFFF is our error code, since it's the most rare number for us! */
	return 0xFFFFFFFFFFFFFFFF;
}

uint64_t physmem_index_to_address(uint64_t)
{

}