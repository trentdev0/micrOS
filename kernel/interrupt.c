#include <stdint.h>
#include <stddef.h>

#include "arch/amd64-pc/cpu.h"
#include "arch/amd64-pc/descriptor.h"
#include "interrupt.h"
#include "stream.h"

#if defined(__x86_64__)
#include "memory.h"

idt_t idt[256];

void interrupt_register(uint8_t vector, void * interrupt_handler, uint8_t flags)
{
	uint64_t handler_interrupt = (uint64_t)interrupt_handler;

	idt[vector].offset_low = (uint16_t)handler_interrupt;
	idt[vector].offset_middle = (uint16_t)(handler_interrupt >> 16);
	idt[vector].offset_high = (uint32_t)(handler_interrupt >> 32);
	idt[vector].selector = 0x28;
	idt[vector].stack_table = 0;
	idt[vector].flags = flags;
	idt[vector].reserved = 0;
}

void interrupt_flush()
{
	descriptor_t descriptor;
	descriptor.limit = sizeof(idt) - 1;
	descriptor.base = (uint64_t)&idt;

	asm volatile ("lidt %0" :: "m"(descriptor) : "memory");
}

#if defined(THIRDPARTY)
void interrupt0(frame_t * frame)
{
	(void)frame;
	stream_printf(current_stream, "[EXCEPTION]:\033[15GDivision error occured!\r\n");

	hang();
}

void interrupt2(frame_t * frame)
{
	(void)frame;
	stream_printf(current_stream, "[EXCEPTION]:\033[15GNMI (Non Maskable Interrupt) occured!\r\n");

	hang();
}

void interrupt4(frame_t * frame)
{
	(void)frame;
	stream_printf(current_stream, "[EXCEPTION]:\033[15GOverflow occured!\r\n");

	hang();
}

void interrupt5(frame_t * frame)
{
	(void)frame;
	stream_printf(current_stream, "[EXCEPTION]:\033[15GUnauthorized access in memory!\r\n");

	hang();
}

void interrupt6(frame_t * frame)
{
	(void)frame;
	stream_printf(current_stream, "[EXCEPTION]:\033[15GInvalid opcode!\r\n");

	hang();
}

void interrupt7(frame_t * frame)
{
	(void)frame;
	stream_printf(current_stream, "[EXCEPTION]:\033[15GFPU (Floating-Point Unit) fault!\r\n");

	hang();
}

void interrupt14(frame_t * frame)
{
	(void)frame;
	stream_printf(current_stream, "[EXCEPTION]:\033[15GPage fault!\r\n");

	hang();
}

void interrupt32(frame_t * frame)
{
	(void)frame;

	stream_printf(current_stream, "[INTERRUPT]:\033[15GPIT interrupt triggered!\r\n");
}
#endif
#endif