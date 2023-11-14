#include <stdint.h>

#include "range.h"
#include "physmem.h"
#include "terminal.h"
#include "ansi.h"

extern char __kernel_end[];

static volatile struct limine_kernel_address_request kernel_address_request = {
	.id = LIMINE_KERNEL_ADDRESS_REQUEST,
	.revision = 0
};

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

	/* The userland's bounds in virtual address space.*/
	userland_address_minimum = 0x1000;
	userland_address_maximum = 0x00007FFFFFFFFFFF;
	userland_address_maximum = (userland_address_maximum / PAGE_SIZE) * PAGE_SIZE;
	userland_address_size = userland_address_maximum - userland_address_minimum;

	terminal_printf(current_terminal, "[" BOLD_RED "RANGE" RESET "]:" ALIGN "Here are the bounds of the kernel in virtual memory (min=" BOLD_WHITE "0x%lx" RESET ", max=" BOLD_WHITE "0x%lx" RESET ", size=" BOLD_WHITE "0x%lx" RESET ")!\r\n", kernel_minimum, kernel_maximum, kernel_size);
	terminal_printf(current_terminal, "[" BOLD_RED "RANGE" RESET "]:" ALIGN "Here are the bounds of the kernel in physical memory (min=" BOLD_WHITE "0x%lx" RESET ", max=" BOLD_WHITE "0x%lx" RESET ", size=" BOLD_WHITE "0x%lx" RESET ")!\r\n", kernel_physical_minimum, kernel_physical_maximum, kernel_physical_size);
	terminal_printf(current_terminal, "[" BOLD_RED "RANGE" RESET "]:" ALIGN "Here are the bounds of userland in virtual address space (min=" BOLD_WHITE "0x%lx" RESET ", max=" BOLD_WHITE "0x%lx" RESET ", size=" BOLD_WHITE "0x%lx" RESET ")!\r\n", userland_address_minimum, userland_address_maximum, userland_address_size);

	return 0;
}