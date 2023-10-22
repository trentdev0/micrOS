#pragma once

#include <stdint.h>

#include "string.h"

#if defined(__x86_64__)
extern void * apic_base;

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
void interrupt2(frame_t * frame) __attribute__((interrupt));
void interrupt4(frame_t * frame) __attribute__((interrupt));
void interrupt5(frame_t * frame) __attribute__((interrupt));
void interrupt6(frame_t * frame) __attribute__((interrupt));
void interrupt7(frame_t * frame) __attribute__((interrupt));

#define APIC_ID 0x20
#define APIC_VERSION 0x30
#define APIC_SPURIOUS_INT 0xF0
#define APIC_EOI 0xB0
#define APIC_ICR_LOW 0x300
#define APIC_ICR_HIGH 0x310

#define APIC_SPURIOUS_VECTOR_NUM 0xFF
#define APIC_FLAG_ENABLE (1 << 8)

static inline uint32_t apic_read(uint16_t offset)
{
    return *(volatile uint32_t*)(apic_base + offset);
}

static inline void apic_write(uint16_t offset, uint32_t value)
{
    *(volatile uint32_t*)(apic_base + offset) = value;
}

static inline void apic_eoi()
{
	apic_write(APIC_EOI, 1);
}

static inline void apic_init()
{
	apic_write(APIC_SPURIOUS_INT, APIC_FLAG_ENABLE | APIC_SPURIOUS_VECTOR_NUM);
}
#endif