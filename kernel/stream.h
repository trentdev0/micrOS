#pragma once

#include "serial.h"

typedef struct
{
	char (* read)();
	void (* write)(char character);
} stream_t;

extern stream_t streams[8];
extern stream_t * current_stream;

void stream_init();

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