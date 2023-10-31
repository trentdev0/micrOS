#include <stdint.h>
#include <stddef.h>

#include "thirdparty/limine.h"
#include "arch/amd64-pc/cpu.h"
#include "serial.h"
#include "stream.h"
#include "interrupt.h"
#include "physmem.h"
#include "virtmem.h"
#include "process.h"

/* This is the kernel's represented as a process. */
process_t process;

void _start()
{
	/*
	 *	Initializing the stream module (stream.c & stream.h) allows us to communicate through
	 *	terminals, such as the framebuffer terminal, which sits on the first screen, and allows
	 *	us to communicate through serial ports COM1, COM2, COM3, COM4, COM5, COM6, COM7 and COM8.
	 */
	if(stream_init() != 0) { hang(); }

	/*
	 *	Initializing the physmem module (physmem.c & physmem.h) allows us to obtain available
	 *	free memory, and also allows us to allocate and free 4KiB pages.
	 */
	if(physmem_init() != 0) { hang(); }

	/*
	 *	Initializing our virtual memory manager will set up the kernel process's pagemap, however
	 *	when we implement code for managing multitasking, it will manage the other parts of the
	 *	kernel's process object.
	 *	Besides that, here we re-initialize virtual memory (since limine kind of does it already for us).
	 *	The virtmem module (virtmem.c & virtmem.h) allows us to map pages, which is very useful.
	 */
	if(virtmem_init() != 0) { hang(); }

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