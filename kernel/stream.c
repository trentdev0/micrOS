#include <stdarg.h>

#include "stream.h"
#include "serial.h"
#include "string.h"

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

int stream_printf(stream_t *stream, const char *format, ...)
{
	va_list args;
	va_start(args, format);

	char character;
	const char *string;
	int8_t int8_val;
	uint8_t uint8_val;
	int16_t int16_val;
	uint16_t uint16_val;
	int32_t int32_val;
	uint32_t uint32_val;
	int64_t int64_val;
	uint64_t uint64_val;

	while (*format != '\0')
	{
		if (*format != '%')
		{
			stream_putc(stream, *format);
		}
		else
		{
			format++;
			switch (*format)
			{
				case 'c':
					character = va_arg(args, int);
					stream_putc(stream, character);
					break;
				case 's':
					string = va_arg(args, const char *);
					stream_puts(stream, string);
					break;
				case 'b':
					format++;
					switch (*format)
					{
						case 's':
							int8_val = va_arg(args, int);
							char int8_string[64];
							convert_int8(int8_val, (char *)&int8_string);
							stream_puts(stream, (char *)&int8_string);
							break;
						case 'u':
							uint8_val = va_arg(args, int);
							char uint8_string[64];
							convert_uint8(uint8_val, (char *)&uint8_string);
							stream_puts(stream, (char *)&uint8_string);
							break;
						default:
							return -1;
					}
					break;
				case 'w':
					format++;
					switch (*format)
					{
						case 's':
							int16_val = va_arg(args, int);
							char int16_string[64];
							convert_int16(int16_val, (char *)&int16_string);
							stream_puts(stream, (char *)&int16_string);
							break;
						case 'u':
							uint16_val = va_arg(args, int);
							char uint16_string[64];
							convert_uint16(uint16_val, (char *)&uint16_string);
							stream_puts(stream, (char *)&uint16_string);
							break;
						default:
							return -1;
					}
					break;
				case 'l':
					format++;
					switch (*format)
					{
						case 's':
							int32_val = va_arg(args, int32_t);
							char int32_string[64];
							convert_int32(int32_val, (char *)&int32_string);
							stream_puts(stream, (char *)&int32_string);
							break;
						case 'u':
							uint32_val = va_arg(args, uint32_t);
							char uint32_string[64];
							convert_uint32(uint32_val, (char *)&uint32_string);
							stream_puts(stream, (char *)&uint32_string);
							break;
						default:
							return -1;
					}
					break;
				case 'q':
					format++;
					switch (*format)
					{
						case 's':
							int64_val = va_arg(args, int64_t);
							char int64_string[64];
							convert_int64(int64_val, (char *)&int64_string);
							stream_puts(stream, (char *)&int64_string);
							break;
						case 'u':
							uint64_val = va_arg(args, uint64_t);
							char uint64_string[64];
							convert_uint64(uint64_val, (char *)&uint64_string);
							stream_puts(stream, (char *)&uint64_string);
							break;
						default:
							return -1;
					}
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