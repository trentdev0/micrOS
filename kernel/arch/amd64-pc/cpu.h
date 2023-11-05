#pragma once

#include <stdint.h>

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

static inline void invlpg(void * address)
{
	asm volatile("invlpg (%0)" : : "r"(address) : "memory");
}

static inline uint64_t read_cr0()
{
	uint64_t return_value;
	asm volatile ("mov %%cr0, %0" : "=r"(return_value) :: "memory");
	return return_value;
}

static inline uint64_t read_cr2()
{
	uint64_t return_value;
	asm volatile ("mov %%cr2, %0" : "=r"(return_value) :: "memory");
	return return_value;
}

static inline uint64_t read_cr3()
{
	uint64_t return_value;
	asm volatile ("mov %%cr3, %0" : "=r"(return_value) :: "memory");
	return return_value;
}

static inline uint64_t read_cr4()
{
	uint64_t return_value;
	asm volatile ("mov %%cr4, %0" : "=r"(return_value) :: "memory");
	return return_value;
}

static inline void write_cr0(uint64_t value)
{
	asm volatile ("mov %0, %%cr0" :: "r"(value) : "memory");
}

static inline void write_cr2(uint64_t value)
{
	asm volatile ("mov %0, %%cr2" :: "r"(value) : "memory");
}

static inline void write_cr3(uint64_t value)
{
	asm volatile ("mov %0, %%cr3" :: "r"(value) : "memory");
}

static inline void write_cr4(uint64_t value)
{
	asm volatile ("mov %0, %%cr4" :: "r"(value) : "memory");
}