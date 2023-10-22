#include <stdint.h>
#include <stddef.h>

#include "limine.h"
#include "arch/amd64-pc/cpu.h"
#include "arch/amd64-pc/acpi.h"
#include "serial.h"
#include "stream.h"
#include "interrupt.h"

void _start()
{
	stream_init();

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