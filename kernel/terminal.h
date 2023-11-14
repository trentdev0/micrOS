#pragma once

#include <stdint.h>

#include "serial.h"
#include "thirdparty/limine.h"

/* Yes, terminal.h also has some basics for framebuffers. */
extern volatile struct limine_framebuffer_request framebuffer_request;
#define RGB(r, g, b) (((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))

typedef struct
{
	char (* read)();
	void (* write)(char character);
} terminal_t;

extern terminal_t terminals[8];
extern terminal_t * current_terminal;

int terminal_init();

static inline void terminal_putc(terminal_t * terminal, char character)
{
	terminal->write(character);
}

static inline char terminal_getchar(terminal_t * terminal)
{
	return terminal->read();
}

static inline void terminal_puts(terminal_t * terminal, const char * string)
{
	while(*string != '\0')
	{
		terminal_putc(terminal, *string);
		string++;
	}
}

int terminal_printf(terminal_t * terminal, const char * format, ...);