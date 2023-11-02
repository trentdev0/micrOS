#pragma once

#include <stdint.h>

#include "../../memory.h"

/* Disable interrupts. */
static inline void cli()
{
	asm("cli");
}

/* Enable interrupts. */
static inline void sti()
{
	asm("sti");
}

/* Halt the current CPU. */
static inline void hlt()
{
	asm("hlt");
}

/* Hang a CPU. */
void hang();

/* Switch to a pagemap. */
static inline void memory_switch(pagemap_t *pagemap)
{
	asm volatile("mov %0, %%cr3" : : "r"((void *)pagemap->start - hhdm_request.response->offset) : "memory");
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t data;
	asm volatile("inb %1, %0" : "=a" (data) : "dN" (port));
	return data;
}

static inline void outb(uint16_t port, uint8_t data)
{
	asm volatile("outb %0, %1" : : "a" (data), "dN" (port));
}

static inline uint16_t inw(uint16_t port)
{
	uint16_t data;
	asm volatile("inw %1, %0" : "=a" (data) : "dN" (port));
	return data;
}

static inline void outw(uint16_t port, uint16_t data)
{
	asm volatile("outw %0, %1" : : "a" (data), "dN" (port));
}

static inline uint32_t inl(uint16_t port)
{
	uint32_t data;
	asm volatile("inl %1, %0" : "=a" (data) : "dN" (port));
	return data;
}

static inline void outl(uint16_t port, uint32_t data)
{
	asm volatile("outl %0, %1" : : "a" (data), "dN" (port));
}