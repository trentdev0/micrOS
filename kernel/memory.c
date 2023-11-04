#include <stddef.h>
#include <stdint.h>

#include "memory.h"
#include "stream.h"
#include "memory.h"
#include "ansi.h"
#include "string.h"
#include "thirdparty/limine.h"
#include "arch/amd64-pc/cpu.h"

pagemap_t pagemap;

region_t regions[128];
uint64_t regions_size = 0;

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

uint64_t * memory_next(uint64_t * current, uint64_t index)
{
	if((current[index] & 1) != 0)
	{
		return (uint64_t *)((current[index] & PTE_ADDRESS_MASK));
	}

	uint64_t next = memory_allocate();
	memset((void *)next, 0, PAGE_SIZE);

	if(next == 0xFFFFFFFFFFFFFFFF)
	{
		return NULL;
	}

	current[index] = (next - OFFSET) | PTE_PRESENT | PTE_WRITABLE | PTE_USER;
	return (uint64_t *)(next);
}


void memory_unmap(pagemap_t * pagemap, uint64_t virtual_address)
{
	uint64_t pml4_index = (virtual_address & ((uint64_t)0x1FF << 39)) >> 39;
	uint64_t pml3_index = (virtual_address & ((uint64_t)0x1FF << 30)) >> 30;
	uint64_t pml2_index = (virtual_address & ((uint64_t)0x1FF << 21)) >> 21;
	uint64_t pml1_index = (virtual_address & ((uint64_t)0x1FF << 12)) >> 12;

	uint64_t * pml3 = memory_next(pagemap->start, pml4_index);
	uint64_t * pml2 = memory_next(pml3, pml3_index);
	uint64_t * pml1 = memory_next(pml2, pml2_index);

	pml1[pml1_index] = 0;

	asm volatile("invlpg %0" : : "m"(*(char *)virtual_address) : "memory");
}

void memory_map(pagemap_t * pagemap, uint64_t physical_address, uint64_t virtual_address, uint64_t flags)
{
	uint64_t pml4_index = (virtual_address & ((uint64_t)0x1FF << 39)) >> 39;
	uint64_t pml3_index = (virtual_address & ((uint64_t)0x1FF << 30)) >> 30;
	uint64_t pml2_index = (virtual_address & ((uint64_t)0x1FF << 21)) >> 21;
	uint64_t pml1_index = (virtual_address & ((uint64_t)0x1FF << 12)) >> 12;
	uint64_t * pml3 = memory_next(pagemap->start, pml4_index);
	uint64_t * pml2 = memory_next(pml3, pml3_index);
	uint64_t * pml1 = memory_next(pml2, pml2_index);

	pml1[pml1_index] = physical_address | flags;
}

uint64_t * memory_virt2pte(pagemap_t * pagemap, uint64_t address)
{
	uint64_t pml4_index = (address & ((uint64_t)0x1FFLLU << 39)) >> 39;
	uint64_t pml3_index = (address & ((uint64_t)0x1FFLLU << 30)) >> 30;
	uint64_t pml2_index = (address & ((uint64_t)0x1FFLLU << 21)) >> 21;
	uint64_t pml1_index = (address & ((uint64_t)0x1FFLLU << 12)) >> 12;

	uint64_t *pml3 = memory_next(pagemap->start, pml4_index);
	if(pml3 == NULL)
	{
		return NULL;
	}

	uint64_t *pml2 = memory_next(pml3, pml3_index);
	if(pml2 == NULL)
	{
		return NULL;
	}

	uint64_t *pml1 = memory_next(pml2, pml2_index);
	if(pml1 == NULL)
	{
		return NULL;
	}

	return &pml1[pml1_index];
}

uint64_t memory_virt2phys(pagemap_t * pagemap, uint64_t address)
{
	uint64_t * pte = memory_virt2pte(pagemap, address);
	return *pte & PTE_ADDRESS_MASK;
}

int memory_init()
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
			regions[regions_size].memory_minimum = entry->base + OFFSET;
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
			regions[regions_size].status_pages_size = memory_byte2page(regions[regions_size].status_bytes_size);

			regions_size++;
			break;
		}
	}

	uint64_t kernel_minimum, kernel_maximum, kernel_size;
	uint64_t kernel_physical_minimum, kernel_physical_maximum, kernel_physical_size;

	kernel_minimum = kernel_address_request.response->virtual_base;
	kernel_maximum = (uint64_t)&__kernel_end;
	kernel_size = (uint64_t)&__kernel_end - kernel_address_request.response->virtual_base;

	kernel_physical_minimum = kernel_address_request.response->physical_base;
	kernel_physical_size = kernel_size;
	kernel_physical_maximum = kernel_physical_minimum + kernel_physical_size;

	stream_printf(current_stream, "[" BOLD_RED "MEMORY" RESET "]:" ALIGN "Here are the bounds of the kernel in virtual memory (min=" BOLD_WHITE "0x%lx" RESET ", max=" BOLD_WHITE "0x%lx" RESET ", size=" BOLD_WHITE "0x%lx" RESET ")!\r\n", kernel_minimum, kernel_maximum, kernel_size);
	stream_printf(current_stream, "[" BOLD_RED "MEMORY" RESET "]:" ALIGN "Here are the bounds of the kernel in physical memory (min=" BOLD_WHITE "0x%lx" RESET ", max=" BOLD_WHITE "0x%lx" RESET ", size=" BOLD_WHITE "0x%lx" RESET ")!\r\n", kernel_physical_minimum, kernel_physical_maximum, kernel_physical_size);

	pagemap.start = (uint64_t *)memory_allocate();
	memset((uint64_t *)pagemap.start, 0, PAGE_SIZE);

	for(uint64_t i = 0; i < ((((uint64_t)&__kernel_end + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE) - kernel_minimum; i += PAGE_SIZE)
	{
		memory_map(&pagemap, kernel_physical_minimum + i, kernel_minimum + i, PTE_PRESENT | PTE_WRITABLE);
	}

	for(uint64_t address = 0x1000; address < 0x100000000; address += PAGE_SIZE)
	{
		memory_map(&pagemap, address, address, PTE_PRESENT | PTE_WRITABLE);
		memory_map(&pagemap, address, address + OFFSET, PTE_PRESENT | PTE_WRITABLE | PTE_NX);
	}

	memory_switch(&pagemap);

	return 0;
}

/* Display the bitmap of a specific region by the region's index. */
void memory_printbitmap(uint64_t index)
{
	for (uint64_t i = 0; i < regions[index].status_bits_size; i++)
	{
		if (memory_getstatus(index, i) == true)
		{
			stream_printf(current_stream, "\x1b[36m1");
		}
		else if (memory_getstatus(index, i) == false)
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
void memory_printpages(uint64_t index)
{
	for (uint64_t i = 0; i < regions[index].pages_size; i++)
	{
		stream_printf(current_stream, "0x%lx ", memory_index2address(index, i));
	}
	stream_printf(current_stream, "\r\n");
}

/* Find a free page, and return it's index. */
uint64_t memory_find_free(uint64_t index)
{
	for(uint64_t i = regions[index].status_pages_size; i < regions[index].status_bits_size; i++)
	{
		if(memory_getstatus(index, i) == false)
		{
			return i;
		}
	}

	return 0xFFFFFFFFFFFFFFFF;
}

/* Allocate a 4KiB page. */
uint64_t memory_allocate()
{
	for(uint64_t i = 0; i < regions_size; i++)
	{
		uint64_t j = memory_find_free(i);
		if(j != 0xFFFFFFFFFFFFFFFF)
		{
			uint64_t k = memory_index2address(i, j);
			memory_mark_allocated(i, j);
			return k;
		}
	}

	return 0xFFFFFFFFFFFFFFFF;
}

/* Free a 4KiB page. */
int memory_free(uint64_t address)
{
	uint64_t index0 = memory_getregion(address);
	uint64_t index1 = memory_address2index(index0, address);

	if(index0 == 0xFFFFFFFFFFFFFFFF)
	{
		return -1;
	}

	memory_mark_free(index0, index1);

	return 0;
}