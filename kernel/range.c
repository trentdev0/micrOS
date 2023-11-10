#include <stdint.h>

#include "range.h"
#include "physmem.h"
#include "stream.h"
#include "ansi.h"

extern char __kernel_end[];

uint64_t kernel_minimum, kernel_maximum, kernel_size;
uint64_t kernel_physical_minimum, kernel_physical_maximum, kernel_physical_size;
uint64_t virtual_address_minimum, virtual_address_maximum, virtual_address_size;
uint64_t userland_address_minimum, userland_address_maximum, userland_address_size;

int range_init()
{
	/* The kernel's bounds in virtual address space. */
	kernel_minimum = kernel_address_request.response->virtual_base;
	kernel_maximum = (uint64_t)&__kernel_end;
	kernel_size = kernel_maximum - kernel_minimum;

	/* The kernel's bounds in physical memory. */
	kernel_physical_minimum = kernel_address_request.response->physical_base;
	kernel_physical_size = kernel_size;
	kernel_physical_maximum = kernel_physical_minimum + kernel_physical_size;

	/* Free region of virtual address space. */
	virtual_address_minimum = regions[regions_size - 1].memory_maximum + OFFSET;
	virtual_address_maximum = kernel_address_request.response->virtual_base;
	virtual_address_size = virtual_address_maximum - virtual_address_minimum;
	virtual_address_size = PAGE_ALIGN_DOWN(virtual_address_size);

	/* The userland's bounds in virtual address space.*/
	userland_address_minimum = 0x1000;
	userland_address_maximum = 0x00007FFFFFFFFFFF;
	userland_address_maximum = PAGE_ALIGN_DOWN(userland_address_maximum);
	userland_address_size = userland_address_maximum - userland_address_minimum;

	stream_printf(current_stream, "[" BOLD_RED "MEMORY" RESET "]:" ALIGN "Here are the bounds of the kernel in virtual memory (min=" BOLD_WHITE "0x%lx" RESET ", max=" BOLD_WHITE "0x%lx" RESET ", size=" BOLD_WHITE "0x%lx" RESET ")!\r\n", kernel_minimum, kernel_maximum, kernel_size);
	stream_printf(current_stream, "[" BOLD_RED "MEMORY" RESET "]:" ALIGN "Here are the bounds of the kernel in physical memory (min=" BOLD_WHITE "0x%lx" RESET ", max=" BOLD_WHITE "0x%lx" RESET ", size=" BOLD_WHITE "0x%lx" RESET ")!\r\n", kernel_physical_minimum, kernel_physical_maximum, kernel_physical_size);
	stream_printf(current_stream, "[" BOLD_RED "MEMORY" RESET "]:" ALIGN "Here are the bounds of free virtual address space (min=" BOLD_WHITE "0x%lx" RESET ", max=" BOLD_WHITE "0x%lx" RESET ", size=" BOLD_WHITE "0x%lx" RESET ")!\r\n", virtual_address_minimum, virtual_address_maximum, virtual_address_size);
	stream_printf(current_stream, "[" BOLD_RED "MEMORY" RESET "]:" ALIGN "Here are the bounds of userland in virtual address space (min=" BOLD_WHITE "0x%lx" RESET ", max=" BOLD_WHITE "0x%lx" RESET ", size=" BOLD_WHITE "0x%lx" RESET ")!\r\n", userland_address_minimum, userland_address_maximum, userland_address_size);

	return 0;
}