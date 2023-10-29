#include <stdint.h>
#include <stddef.h>

#include "thirdparty/limine.h"
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

	stream_printf(current_stream, "Hello, world!\r\n");

	virtmem_init();

	uint64_t pointer0 = physmem_alloc();
	uint64_t pointer1 = physmem_alloc();
	uint64_t pointer2 = physmem_alloc();
	stream_printf(current_stream, "Allocating a page!\r\nHere's the address: 0x%lx!\r\n", pointer0);
	stream_printf(current_stream, "Allocating a page!\r\nHere's the address: 0x%lx!\r\n", pointer1);
	stream_printf(current_stream, "Allocating a page!\r\nHere's the address: 0x%lx!\r\n", pointer2);

	stream_printf(current_stream, "Freeing a page!\r\n");
	physmem_free(pointer0);
	stream_printf(current_stream, "Freeing a page!\r\n");
	physmem_free(pointer1);
	stream_printf(current_stream, "Freeing a page!\r\n");
	physmem_free(pointer2);

	uint64_t pointer3 = physmem_alloc();
	stream_printf(current_stream, "Allocating a page again!\r\nHere's the address: 0x%lx!\r\n", pointer3);
	physmem_free(pointer3);

	hang();
}