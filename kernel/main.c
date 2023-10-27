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

	/* Set value to 0! */
	uint64_t value = 0;
	stream_printf(current_stream, "Value before setting it's first bit: %lx.\r\n", value);
	/* Set value to 0000000000000000000000000000000000000000000000000000000000000001. */
	physmem_setbit((uint8_t *)&value, 0, true);
	stream_printf(current_stream, "Value after setting it's first bit: %lx.\r\n", value);
	/* Set value to 0000000000000000000000000000000000000000000000000000000000000011. */
	physmem_setbit((uint8_t *)&value, 1, true);
	stream_printf(current_stream, "Value after setting it's second bit: %lx.\r\n", value);
	/* Set value to 0000000000000000000000000000000000000000000000000000000000000010. */
	physmem_setbit((uint8_t *)&value, 0, false);
	stream_printf(current_stream, "Value after setting it's first bit again: %lx.\r\n", value);

	hang();
}