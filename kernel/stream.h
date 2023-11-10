#pragma once

#include <stdint.h>

#include "serial.h"
#include "thirdparty/limine.h"

/* Yes, stream.h also has some basics for framebuffers. */
extern volatile struct limine_framebuffer_request framebuffer_request;
#define RGB(r, g, b) (((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))

typedef struct
{
	char (* read)();
	void (* write)(char character);
} stream_t;

extern stream_t streams[8];
extern stream_t * current_stream;

int stream_init();

static inline void stream_putc(stream_t * stream, char character)
{
	stream->write(character);
}

static inline char stream_getchar(stream_t * stream)
{
	return stream->read();
}

static inline void stream_puts(stream_t * stream, const char * string)
{
	while(*string != '\0')
	{
		stream_putc(stream, *string);
		string++;
	}
}

int stream_printf(stream_t * stream, const char * format, ...);