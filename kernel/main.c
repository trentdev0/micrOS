#include <stdint.h>
#include <stddef.h>

#include "thirdparty/limine.h"
#include "arch/amd64-pc/cpu.h"
#include "arch/amd64-pc/gdt.h"
#include "serial.h"
#include "stream.h"
#include "interrupt.h"
#include "ansi.h"
#include "physmem.h"
#include "virtmem.h"

void _start()
{
	/* Set up the GDT (Global Descriptor Table) structure & flush it down to the CPU. */
	if(gdt_init() != 0) { hang(); }

	/*
	 *	Initializing the stream module (stream.c & stream.h) allows us to communicate through
	 *	terminals, such as the framebuffer terminal, which sits on the first screen, and allows
	 *	us to communicate through serial ports COM1, COM2, COM3, COM4, COM5, COM6, COM7 and COM8.
	 */
	if(stream_init() != 0) { hang(); }

	/* Set up the page frame allocator! */
	if(physmem_init() != 0) { hang(); }

	/* Set up the virtual memory manager! */
	if(virtmem_init() != 0) { hang(); }

	/* Tell the CPU where our new IDT (Interrupt Descriptor Table) is at... */
	interrupt_flush();

	/* Add exception handler entries to the IDT! */
	interrupt_register(0, &interrupt0, 0x8E);
	interrupt_register(2, &interrupt2, 0x8E);
	interrupt_register(4, &interrupt4, 0x8E);
	interrupt_register(5, &interrupt5, 0x8E);
	interrupt_register(6, &interrupt6, 0x8E);
	interrupt_register(7, &interrupt7, 0x8E);
	interrupt_register(8, &interrupt8, 0x8E);
	interrupt_register(10, &interrupt10, 0x8E);
	interrupt_register(11, &interrupt11, 0x8E);
	interrupt_register(12, &interrupt12, 0x8E);
	interrupt_register(13, &interrupt13, 0x8E);
	interrupt_register(14, &interrupt14, 0x8E);
	interrupt_register(15, &interrupt15, 0x8E);
	interrupt_register(16, &interrupt16, 0x8E);
	interrupt_register(18, &interrupt18, 0x8E);
	interrupt_register(21, &interrupt21, 0x8E);

	/*
	 *	Printing `Hello, world!` allows us to see if the initialization of all other modules above
	 *	succeeded or not.
	 */
	stream_printf(current_stream, "Hello, world!\r\n");

	uint64_t pointer0;
	physmem_allocate(&pointer0);
	stream_printf(current_stream, "Allocated a page (address=" BOLD_WHITE "0x%lx" RESET ")!\r\n", pointer0);

	uint64_t pointer1;
	physmem_allocate(&pointer1);
	stream_printf(current_stream, "Allocated a page (address=" BOLD_WHITE "0x%lx" RESET ")!\r\n", pointer1);

	uint64_t pointer2;
	physmem_allocate(&pointer2);
	stream_printf(current_stream, "Allocated a page (address=" BOLD_WHITE "0x%lx" RESET ")!\r\n", pointer2);

	physmem_free(pointer0);

	stream_printf(current_stream, "Freed the first allocated page!\r\n");

	physmem_allocate(&pointer0);
	stream_printf(current_stream, "Allocated a page (address=" BOLD_WHITE "0x%lx" RESET ")!\r\n", pointer0);

	/* Let's hang the CPU here, causing it to disable interrupts & halt in a loop. */
	hang();
}