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

typedef struct
{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rax;
    uint64_t int_num;
    uint64_t error_code;
    uint64_t rip;
    uint64_t rsp;
    uint64_t rflags;
    uint64_t cs;
    uint64_t ss;
    uint64_t ds;
    uint64_t es;
    uint64_t fs;
    uint64_t gs;
} frame_t;

extern idt_t idt[256];

void interrupt_register(uint8_t vector, void * interrupt_handler, uint8_t flags);
void interrupt_flush();

static inline void interrupt_wipe()
{
	memset(idt, 0, sizeof(idt));
}

void interrupt0(frame_t * frame) __attribute__((interrupt));
#endif