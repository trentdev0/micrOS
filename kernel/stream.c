#include <stdarg.h>

#include "stream.h"
#include "serial.h"

stream_t streams[8];
stream_t * current_stream;

void stream_init()
{
	serial_init(COM1);
	serial_init(COM2);
	serial_init(COM3);
	serial_init(COM4);
	serial_init(COM5);
	serial_init(COM6);
	serial_init(COM7);
	serial_init(COM8);

	streams[0].read = &com1_read;
	streams[0].write = &com1_write;
	streams[1].read = &com2_read;
	streams[1].write = &com2_write;
	streams[2].read = &com3_read;
	streams[2].write = &com3_write;
	streams[3].read = &com4_read;
	streams[3].write = &com4_write;
	streams[4].read = &com5_read;
	streams[4].write = &com5_write;
	streams[5].read = &com6_read;
	streams[5].write = &com6_write;
	streams[6].read = &com7_read;
	streams[6].write = &com7_write;
	streams[7].read = &com8_read;
	streams[7].write = &com8_write;
	current_stream = &streams[0];
}

int stream_printf(stream_t * stream, const char * format, ...)
{
	va_list args;
	va_start(args, format);

	char character;
	const char * string;
	unsigned int num;
	
	while(*format != '\0')
	{
		if(*format != '%')
		{
			stream_putc(stream, *format);
		}
		else
		{
			format++;
			switch(*format)
			{
				case 'c':
					character = va_arg(args, int);
					stream_putc(stream, character);
					break;
				case 's':
					string = va_arg(args, const char *);
					stream_puts(stream, string);
					break;
				default:
					return -1;
			}
		}
		format++;
	}

	va_end(args);

	return 0;
}