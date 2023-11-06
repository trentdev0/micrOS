#pragma once

#include <stdint.h>

#include "string.h"

#if defined(__x86_64__)
typedef struct
{
	uint16_t offset_low;
	uint16_t selector;
	uint8_t stack_table;
	uint8_t flags;
	uint16_t offset_middle;
	uint32_t offset_high;
	uint32_t reserved;
} idt_t;

extern idt_t idt[256];

void interrupt_register(uint8_t vector, void * interrupt_handler, uint8_t flags);
void interrupt_flush();

static inline void interrupt_wipe()
{
	memset(idt, 0, sizeof(idt));
}

extern void interrupt0();
extern void interrupt2();
extern void interrupt4();
extern void interrupt5();
extern void interrupt6();
extern void interrupt7();
extern void interrupt8();
extern void interrupt10();
extern void interrupt11();
extern void interrupt12();
extern void interrupt13();
extern void interrupt14();
extern void interrupt15();
extern void interrupt16();
extern void interrupt18();
extern void interrupt21();
#endif