#include <stddef.h>
#include <stdint.h>

#include "memory.h"
#include "stream.h"
#include "memory.h"
#include "ansi.h"
#include "thirdparty/limine.h"

pagemap_t pagemap;

extern char kernel_end[];

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

int memory_init()
{
	struct limine_memmap_response * memmap = memmap_request.response;
	struct limine_memmap_entry ** entries = memmap->entries;

	for (uint64_t i = 0; i < memmap->entry_count; i++)
	{
		struct limine_memmap_entry * entry = entries[i];

		stream_printf(current_stream, "[MEMORY]:" ALIGN "Found a memmap entry (base=" BOLD_WHITE "0x%lx" RESET ", length=" BOLD_WHITE "0x%lx" RESET ", type=" BOLD_WHITE "%ld" RESET ")!\r\n", entry->base, entry->length, entry->type);

		switch (entry->type)
		{
		case LIMINE_MEMMAP_USABLE:
			
			break;
		}
	}

	stream_printf(current_stream, "[MEMORY]:" ALIGN "Here are the bounds of the kernel in virtual memory (min=" BOLD_WHITE "0x%lx" RESET ", max=" BOLD_WHITE "0x%lx" RESET ", size=" BOLD_WHITE "0x%lx" RESET ")!\r\n", kernel_address_request.response->virtual_base, (uint64_t)&kernel_end, (uint64_t)&kernel_end - kernel_address_request.response->virtual_base);

	return 0;
}