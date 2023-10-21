#include <stdint.h>
#include <stddef.h>

#include "limine.h"
#include "arch/amd64/amd64.h"
#include "serial.h"
#include "stream.h"
#include "interrupt.h"

void _start()
{
	stream_init();

	interrupt_wipe();
	interrupt_flush();
	interrupt_register(0, &interrupt0, 0x8E);

	stream_printf(current_stream, "Hello, world!\r\nHere's my string: `%s`.\r\n", "Hi!");

	hang();
}