#include <stdint.h>
#include <stddef.h>

#include "thirdparty/limine.h"
#include "arch/amd64-pc/cpu.h"
#include "serial.h"
#include "stream.h"
#include "interrupt.h"
#include "ansi.h"

extern char kernel_end[];

void _start()
{
	/*
	 *	Initializing the stream module (stream.c & stream.h) allows us to communicate through
	 *	terminals, such as the framebuffer terminal, which sits on the first screen, and allows
	 *	us to communicate through serial ports COM1, COM2, COM3, COM4, COM5, COM6, COM7 and COM8.
	 */
	if(stream_init() != 0) { hang(); }

	/*
	 *	We merged the physical memory & virtual memory modules into one, since we want the page
	 *	frame allocator to be "more connected" to our virtual memory module, meaning that we
	 *	just need to call a function for allocation and de-allocation, without worrying about
	 *	conversion of physical address to virtual address after allocation.
	 */
	if(memory_init() != 0) { hang(); }

	/* Tell the CPU where our new IDT (Interrupt Descriptor Table) is at... */
	interrupt_flush();

	/* Add division error interrupt handler as an entry in the IDT. */
	interrupt_register(0, &interrupt0, 0x8E);

	/*
	 *	Printing `Hello, world!` allows us to see if the initialization of all other modules above
	 *	succeeded or not.
	 */
	stream_printf(current_stream, "Hello, world!\r\n");

	/* Let's hang the CPU here, causing it to disable interrupts & halt in a loop. */
	hang();
}