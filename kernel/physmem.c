#include <stdint.h>
#include <stddef.h>

#include "string.h"
#include "physmem.h"
#include "terminal.h"
#include "ansi.h"
#include "thirdparty/limine.h"

volatile struct limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0
};

volatile struct limine_hhdm_request hhdm_request = {
	.id = LIMINE_HHDM_REQUEST,
	.revision = 0
};

uint8_t *bitmap = NULL;
uint64_t highest_page_index = 0;
uint64_t last_used_index = 0;
uint64_t usable_pages = 0;
uint64_t used_pages = 0;
uint64_t reserved_pages = 0;

int physmem_init()
{
	struct limine_memmap_response *memmap = memmap_request.response;
	struct limine_hhdm_response *hhdm = hhdm_request.response;
	struct limine_memmap_entry **entries = memmap->entries;

	uint64_t highest_address = 0;

	for(uint64_t i = 0; i < memmap->entry_count; i++)
	{
		struct limine_memmap_entry *entry = entries[i];

		switch(entry->type)
		{
			case LIMINE_MEMMAP_USABLE:
				usable_pages += (entry->length + (PAGE_SIZE - 1)) / PAGE_SIZE;
				highest_address = highest_address > (entry->base + entry->length) ? highest_address : (entry->base + entry->length);
				break;
			case LIMINE_MEMMAP_RESERVED:
			case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
			case LIMINE_MEMMAP_ACPI_NVS:
			case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
			case LIMINE_MEMMAP_KERNEL_AND_MODULES:
				reserved_pages += (entry->length + (PAGE_SIZE - 1)) / PAGE_SIZE;
				break;
		}
	}

	highest_page_index = highest_address / PAGE_SIZE;
	uint64_t bitmap_size = ((highest_page_index / 8) + (PAGE_SIZE - 1)) / PAGE_SIZE * PAGE_SIZE;

	for(size_t i = 0; i < memmap->entry_count; i++)
	{
		struct limine_memmap_entry *entry = entries[i];

		if(entry->type != LIMINE_MEMMAP_USABLE)
		{
			continue;
		}

		if(entry->length >= bitmap_size)
		{
			bitmap = (uint8_t *)(entry->base + hhdm->offset);

			memset(bitmap, 0xFF, bitmap_size);

			entry->length -= bitmap_size;
			entry->base += bitmap_size;

			break;
		}
	}

	for(size_t i = 0; i < memmap->entry_count; i++)
	{
		struct limine_memmap_entry *entry = entries[i];

		if(entry->type != LIMINE_MEMMAP_USABLE)
		{
			continue;
		}

		for(uint64_t j = 0; j < entry->length; j += PAGE_SIZE)
		{
			bitmap_clear(bitmap, (entry->base + j) / PAGE_SIZE);
		}
	}

	terminal_printf(current_terminal, "[" BOLD_RED "PHYSMEM" RESET "]:" ALIGN "Detected available memory (size=" BOLD_WHITE "%luMiB" RESET ")!\r\n", (usable_pages * 4096) / 1024 / 1024);

	return 0;
}

static void *inner_allocate(uint64_t pages, uint64_t limit)
{
	uint64_t p = 0;

	while(last_used_index < limit)
	{
		if(!bitmap_get(bitmap, last_used_index++))
		{
			if(++p == pages)
			{
				uint64_t page = last_used_index - pages;
				for(uint64_t i = page; i < last_used_index; i++)
				{
					bitmap_set(bitmap, i);
				}
				return (void *)(page * PAGE_SIZE);
			}
		}
		else
		{
			p = 0;
		}
	}

	return NULL;
}

void *physmem_allocate(uint64_t pages)
{
	uint64_t last = last_used_index;
	void *return_value = inner_allocate(pages, highest_page_index);

	if(return_value == NULL)
	{
		last_used_index = 0;
		return_value = inner_allocate(pages, last);
	}

	used_pages += pages;

	return return_value;
}

void physmem_deallocate(void *address, uint64_t pages)
{
	uint64_t page = (uint64_t)address / PAGE_SIZE;

	for(uint64_t i = page; i < page + pages; i++)
	{
		bitmap_clear(bitmap, i);
	}

	used_pages -= pages;
}