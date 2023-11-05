#include <stddef.h>
#include <stdint.h>

#include "physmem.h"
#include "stream.h"
#include "ansi.h"
#include "string.h"
#include "thirdparty/limine.h"
#include "arch/amd64-pc/cpu.h"

region_t regions[128];
uint64_t regions_size = 0;

uint64_t kernel_minimum, kernel_maximum, kernel_size;
uint64_t kernel_physical_minimum, kernel_physical_maximum, kernel_physical_size;
uint64_t virtual_address_minimum, virtual_address_maximum, virtual_address_size;

extern char __text_start[];
extern char __text_end[];
extern char __rodata_start[];
extern char __rodata_end[];
extern char __data_start[];
extern char __data_end[];
extern char __kernel_end[];

volatile struct limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0
};

volatile struct limine_hhdm_request hhdm_request = {
	.id = LIMINE_HHDM_REQUEST,
	.revision = 0
};

volatile struct limine_kernel_address_request kernel_address_request = {
	.id = LIMINE_KERNEL_ADDRESS_REQUEST,
	.revision = 0
};

static volatile struct limine_paging_mode_request paging_mode_request = {
	.id = LIMINE_PAGING_MODE_REQUEST,
	.revision = 0,
	.response = NULL,
	.mode = LIMINE_PAGING_MODE_X86_64_5LVL,
	.flags = 0
};

int physmem_init()
{
	struct limine_memmap_response * memmap = memmap_request.response;
	struct limine_memmap_entry ** entries = memmap->entries;

	for (uint64_t i = 0; i < memmap->entry_count; i++)
	{
		struct limine_memmap_entry * entry = entries[i];

		switch (entry->type)
		{
		case LIMINE_MEMMAP_USABLE:
			/* The value memory_minimum represents the start of the region of free memory. */
			regions[regions_size].memory_minimum = entry->base;
			/* The value memory_size represents the exact amount of free memory in bytes. */
			regions[regions_size].memory_size = entry->length;
			/* The value memory_maximum represents the end of the region of free memory. */
			regions[regions_size].memory_maximum = regions[regions_size].memory_minimum + entry->length;
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

			stream_printf(current_stream, "[" BOLD_RED "MEMORY" RESET "]:" ALIGN "Here are the bounds of entry " BOLD_WHITE "%lu" RESET " of usable memory (min=" BOLD_WHITE "0x%lx" RESET ", max=" BOLD_WHITE "0x%lx" RESET ", size=" BOLD_WHITE "0x%lx" RESET ")!\r\n", regions_size + 1, regions[regions_size].memory_minimum, regions[regions_size].memory_maximum, regions[regions_size].memory_size);

			regions_size++;
			break;
		}
	}

	uint64_t installed_memory = 0;
	for(uint64_t i = 0; i < regions_size; i++)
	{
		installed_memory += regions[i].memory_size;
	}

	stream_printf(current_stream, "[" BOLD_RED "MEMORY" RESET "]:" ALIGN "In total, there are " BOLD_WHITE "%lu" RESET " bytes of usable memory.\r\n", installed_memory);

	kernel_minimum = kernel_address_request.response->virtual_base;
	kernel_maximum = (uint64_t)&__kernel_end;
	kernel_size = (uint64_t)&__kernel_end - kernel_address_request.response->virtual_base;

	kernel_physical_minimum = kernel_address_request.response->physical_base;
	kernel_physical_size = kernel_size;
	kernel_physical_maximum = kernel_physical_minimum + kernel_physical_size;

	stream_printf(current_stream, "[" BOLD_RED "MEMORY" RESET "]:" ALIGN "Here are the bounds of the kernel in virtual memory (min=" BOLD_WHITE "0x%lx" RESET ", max=" BOLD_WHITE "0x%lx" RESET ", size=" BOLD_WHITE "0x%lx" RESET ")!\r\n", kernel_minimum, kernel_maximum, kernel_size);
	stream_printf(current_stream, "[" BOLD_RED "MEMORY" RESET "]:" ALIGN "Here are the bounds of the kernel in physical memory (min=" BOLD_WHITE "0x%lx" RESET ", max=" BOLD_WHITE "0x%lx" RESET ", size=" BOLD_WHITE "0x%lx" RESET ")!\r\n", kernel_physical_minimum, kernel_physical_maximum, kernel_physical_size);

	/* Virtual memory drivers were removed, working on them right now! */

	return 0;
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

/* Allocate a 4KiB page. */
int physmem_allocate(uint64_t * address)
{
	for(uint64_t i = 0; i < regions_size; i++)
	{
		uint64_t j = physmem_find_free(i);
		if(j != 0xFFFFFFFFFFFFFFFF)
		{
			uint64_t k = physmem_index2address(i, j);
			physmem_mark_allocated(i, j);
			*address = k;
			return 0;
		}
	}

	return -1;
}

/* Free a 4KiB page. */
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