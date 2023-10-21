#include <stdint.h>
#include <stddef.h>

#include "limine.h"
#include "arch/amd64/amd64.h"
#include "serial.h"
#include "stream.h"

void _start()
{
	stream_init();

	stream_printf(current_stream, "Hello, world!\r\nHere's my string: `%s`.\r\n", "Hi!");

	hang();
}