#include <stdint.h>
#include <stddef.h>

#include "limine.h"
#include "arch/amd64-pc/cpu.h"
#include "arch/amd64-pc/acpi.h"
#include "serial.h"
#include "stream.h"
#include "interrupt.h"
#include "physmem.h"

void _start()
{
	stream_init();

	physmem_init();

	for(int i = 0; i < 10; i++)
	{
		uint64_t addr = physmem_index_to_address(1, i);
		uint64_t indx = physmem_address_to_index(1, addr);

		stream_printf(current_stream, "Here's an address: \"0x%lx\"!\r\n", addr);
		stream_printf(current_stream, "Here's a page index: \"0x%lx\"!\r\n", indx);
	}

	interrupt_wipe();
	interrupt_flush();
	interrupt_register(0, &interrupt0, 0x8E);
	interrupt_register(2, &interrupt2, 0x8E);
	interrupt_register(4, &interrupt4, 0x8E);
	interrupt_register(5, &interrupt5, 0x8E);
	interrupt_register(6, &interrupt6, 0x8E);
	interrupt_register(7, &interrupt7, 0x8E);
	interrupt_register(14, &interrupt14, 0x8E);

	acpi_init();

	stream_printf(current_stream, "Hello, world!\r\n");

	hang();
}