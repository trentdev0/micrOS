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

void __attribute__((interrupt)) interrupt0(void * frame);
#endif