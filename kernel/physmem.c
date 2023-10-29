#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "thirdparty/limine.h"
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

	for (uint64_t i = 0; i < memmap->entry_count; i++)
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
			/* Stores where pages (all of them are next to each other) start in memory. */
			regions[regions_size].pages_minumum = regions[regions_size].memory_minimum;
			/* Stores the size in pages. */
			regions[regions_size].pages_size = regions[regions_size].memory_size / PAGE_SIZE;
			/* Stores the end of the pages in memory. */
			regions[regions_size].pages_maximum = (regions[regions_size].pages_size * PAGE_SIZE) + regions[regions_size].pages_minumum;

			/* Stores the amount of bits needed to store the allocation status of all pages in the region. */
			regions[regions_size].status_bits_size = regions[regions_size].pages_size;

			/* Stores the amount of bytes needed to store the allocation status of all pages in the region. */
			regions[regions_size].status_bytes_size = regions[regions_size].status_bits_size / 8;

			/*
			 *	Stores the amount of pages needed to store the allocation status of all pages in the region.
			 *	It may require some optimization, see the comment below this one.
			 */
			regions[regions_size].status_pages_size = physmem_byte2page(regions[regions_size].status_bytes_size);

			regions_size++;
			break;
		}
	}

	for (typeof(regions_size) i = 0; i < regions_size; i++)
	{
		stream_printf(current_stream, "[PHYSMEM]:\033[15GHere's an chunk of free & usable memory (min=\"0x%lx\", max=\"0x%lx\", size=\"0x%lx\")!\r\n\033[15GWe also calculated some info about pages for you (min=\"0x%lx\", max=\"0x%lx\", size=\"0x%lx\")!\r\n\033[15GWe calculated info about the status region as well (size_bits=\"0x%lx\", size_bytes=\"0x%lx\", size_pages=\"0x%lx\")!\r\n", regions[i].memory_minimum, regions[i].memory_maximum, regions[i].memory_size, regions[i].pages_minumum, regions[i].pages_maximum, regions[i].pages_size, regions[i].status_bits_size, regions[i].status_bytes_size, regions[i].status_pages_size);
	}

	stream_printf(current_stream, "[PHYSMEM]:\033[15GSuccessfully initialized physical memory manager!\r\n");
}

/* Display the bitmap of a specific region by the region's index. */
void physmem_printbitmap(uint64_t index)
{
	for (uint64_t i = 0; i < regions[index].status_bits_size; i++)
	{
		if (physmem_getstatus(index, i) == true)
		{
			stream_printf(current_stream, "\x1b[36m1");
		}
		else if (physmem_getstatus(index, i) == false)
		{
			stream_printf(current_stream, "\x1b[35m0");
		}
	}
	stream_printf(current_stream, "\x1b[0m\r\n");
}

/*
 *	Print all of the addresses of all existing pages in a region, regardless
 *	of whether they are allocated or not.
 */
void physmem_printpages(uint64_t index)
{
	for (uint64_t i = 0; i < regions[index].pages_size; i++)
	{
		stream_printf(current_stream, "0x%lx ", physmem_index2address(index, i));
	}
	stream_printf(current_stream, "\r\n");
}

/* Find a free page, and return it's index. */
uint64_t physmem_find_free(uint64_t index)
{
	for(uint64_t i = regions[index].status_pages_size; i < regions[index].status_bits_size; i++)
	{
		if(physmem_getstatus(index, i) == false)
		{
			return i;
		}
	}

	return 0xFFFFFFFFFFFFFFFF;
}

uint64_t physmem_alloc()
{
	for(uint64_t i = 0; i < regions_size; i++)
	{
		uint64_t j = physmem_find_free(i);
		if(j != 0xFFFFFFFFFFFFFFFF)
		{
			uint64_t k = physmem_index2address(i, j);
			physmem_mark_allocated(i, j);
			return k;
		}
	}

	return 0xFFFFFFFFFFFFFFFF;
}

int physmem_free(uint64_t address)
{
	uint64_t index0 = physmem_getregion(address);
	uint64_t index1 = physmem_address2index(index0, address);

	if(index0 == 0xFFFFFFFFFFFFFFFF)
	{
		return -1;
	}

	physmem_mark_free(index0, index1);

	return 0;
}