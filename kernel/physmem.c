#include <stdint.h>
#include <stddef.h>

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
			regions[regions_size].memory_minimum = entry->base;
			regions[regions_size].memory_size = entry->length;
			regions[regions_size].memory_maximum = entry->base + entry->length;
			regions[regions_size].pages_maximum = regions[regions_size].memory_size / PAGE_SIZE;
			regions_size++;
			break;
		}
	}

	for(typeof(regions_size) i = 0; i < regions_size; i++)
	{
		stream_printf(current_stream, "[PHYSMEM]:\033[15GHere's an chunk of free & usable memory (min=\"0x%lx\", max=\"0x%lx\", maxpages=\"%ld\")!\r\n", regions[i].memory_minimum, regions[i].memory_maximum, regions[i].pages_maximum);
	}
}