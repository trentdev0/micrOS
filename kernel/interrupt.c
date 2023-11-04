#include <stdint.h>
#include <stddef.h>

#include "arch/amd64-pc/cpu.h"
#include "arch/amd64-pc/descriptor.h"
#include "interrupt.h"
#include "stream.h"
#include "ansi.h"

#if defined(__x86_64__)
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

void interrupt0(void * frame)
{
	(void)frame;

	stream_printf(current_stream, "[" BOLD_RED "EXCEPTION" RESET "]:" ALIGN "Division error occured!\r\n");

	hang();
}

void interrupt2(void * frame)
{
	(void)frame;

	stream_printf(current_stream, "[" BOLD_RED "EXCEPTION" RESET "]:" ALIGN "Non maskable interrupt.\r\n");

	hang();
}

void interrupt4(void * frame)
{
	(void)frame;

	stream_printf(current_stream, "[" BOLD_RED "EXCEPTION" RESET "]:" ALIGN "Overflow occured!\r\n");

	hang();
}

void interrupt5(void * frame)
{
	(void)frame;

	stream_printf(current_stream, "[" BOLD_RED "EXCEPTION" RESET "]:" ALIGN "Bound range exceeded!\r\n");

	hang();
}

void interrupt6(void * frame)
{
	(void)frame;

	stream_printf(current_stream, "[" BOLD_RED "EXCEPTION" RESET "]:" ALIGN "Invalid opcode!\r\n");

	hang();
}

void interrupt7(void * frame)
{
	(void)frame;

	stream_printf(current_stream, "[" BOLD_RED "EXCEPTION" RESET "]:" ALIGN "Device not available!\r\n");

	hang();
}

void interrupt8(void * frame)
{
	(void)frame;

	stream_printf(current_stream, "[" BOLD_RED "EXCEPTION" RESET "]:" ALIGN "Double fault!\r\n");

	hang();
}

void interrupt10(void * frame)
{
	(void)frame;

	stream_printf(current_stream, "[" BOLD_RED "EXCEPTION" RESET "]:" ALIGN "Invalid TSS (Task State Segment)!\r\n");

	hang();
}

void interrupt11(void * frame)
{
	(void)frame;

	stream_printf(current_stream, "[" BOLD_RED "EXCEPTION" RESET "]:" ALIGN "Segment not present!\r\n");

	hang();
}

void interrupt12(void * frame)
{
	(void)frame;

	stream_printf(current_stream, "[" BOLD_RED "EXCEPTION" RESET "]:" ALIGN "Invalid TSS (Task State Segment)!\r\n");

	hang();
}

void interrupt13(void * frame)
{
	(void)frame;

	stream_printf(current_stream, "[" BOLD_RED "EXCEPTION" RESET "]:" ALIGN "Segment not present!\r\n");

	hang();
}

void interrupt14(void * frame)
{
	(void)frame;

	uint64_t fault_address;
	asm volatile("mov %%cr2, %0" :"=r"(fault_address) :: "memory");
	stream_printf(current_stream, "[" BOLD_RED "EXCEPTION" RESET "]:" ALIGN "Page fault occured!\r\n" ALIGN "Here's the address of where the page fault occured (address=" BOLD_WHITE "0x%lx" RESET ")!\r\n", fault_address);

	hang();
}

void interrupt15(void * frame)
{
	(void)frame;

	stream_printf(current_stream, "[" BOLD_RED "EXCEPTION" RESET "]:" ALIGN "General protection fault occured!\r\n");

	hang();
}

void interrupt16(void * frame)
{
	(void)frame;

	stream_printf(current_stream, "[" BOLD_RED "EXCEPTION" RESET "]:" ALIGN "Page fault occured!\r\n");

	hang();
}

void interrupt18(void * frame)
{
	(void)frame;

	stream_printf(current_stream, "[" BOLD_RED "EXCEPTION" RESET "]:" ALIGN "FPU exception occured!\r\n");

	hang();
}

void interrupt21(void * frame)
{
	(void)frame;

	stream_printf(current_stream, "[" BOLD_RED "EXCEPTION" RESET "]:" ALIGN "SIMD exception occured!\r\n");

	hang();
}
#endif