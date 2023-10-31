#include <stdarg.h>

#include "stream.h"
#include "serial.h"
#include "string.h"
#include "thirdparty/limine.h"
#include "thirdparty/fb.h"
#include "thirdparty/flanterm.h"

stream_t streams[8];
stream_t * current_stream;

/*
 *	If we're compiling M/UX with third-party features enabled, we will have a framebuffer
 *	terminal, which is provided by flanterm. If we don't have flanterm, all communications
 *	will happen in the COM1 serial port.
 */
#if defined(THIRDPARTY)
struct flanterm_context * flanterm;

static volatile struct limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0
};

void flanterm_putc(char character)
{
	flanterm_write(flanterm, &character, 1);
}
#endif

int stream_init()
{
#if defined(THIRDPARTY)
	flanterm = flanterm_fb_simple_init(framebuffer_request.response->framebuffers[0]->address, framebuffer_request.response->framebuffers[0]->width, framebuffer_request.response->framebuffers[0]->height, framebuffer_request.response->framebuffers[0]->pitch);
#endif

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
#if defined(THIRDPARTY)
	streams[8].read = NULL;
	streams[8].write = &flanterm_putc;
	current_stream = &streams[8];
#else
	current_stream = &streams[0];
#endif

	return 0;
}

/*
 *	A nice print function that allows us to print to a stream with format specifiers. Similar to
 *	the `printf` function in userland programming.
 *	Here are the possible format specifiers:
 *	%c	Allows us to print a single character
 *	%s	Allows us to print a string
 *	%d	Allows us to print a signed integer, can be a int8_t, int16_t, int32_t, or int.
 *	%u	Allows us to print an unsigned integer, can be an uint8_t, uint16_t, uint32_t, or unsigned int.
 *	%x	Allows us to print an unsigned integer in hexadecimal form, can be uint8_t, uint16_t, uint32_t, or unsigned int.
 *	You can also use these format specifiers, but with an l before the format specifier character to print 64-bit numbers.
 */
int stream_printf(stream_t * stream, const char * format, ...)
{
	va_list args;
	va_start(args, format);

	char character_val;
	const char *string_val;
	int int_val;
	unsigned int uint_val;
	unsigned int hex_val;
	long long_val;
	unsigned long ulong_val;
	unsigned long hecks_val;

	char value_string[32];

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
					character_val = va_arg(args, int);
					stream_putc(stream, character_val);
					break;
				case 's':
					string_val = va_arg(args, const char *);
					stream_puts(stream, string_val);
					break;
				case 'd':
					int_val = va_arg(args, int);
					convert_int(int_val, (char *)&value_string, 32);
					stream_puts(stream, (char *)&value_string);
					break;
				case 'u':
					uint_val = va_arg(args, unsigned int);
					convert_uint(uint_val, (char *)&value_string, 32);
					stream_puts(stream, (char *)&value_string);
					break;
				case 'x':
					hex_val = va_arg(args, unsigned int);
					convert_hex(hex_val, (char *)&value_string, 32);
					stream_puts(stream, (char *)&value_string);
					break;
				case 'l':
					format++;
					switch(*format)
					{
					case 'd':
						long_val = va_arg(args, long);
						convert_long(long_val, (char *)&value_string, 32);
						stream_puts(stream, (char *)&value_string);
						break;
					case 'u':
						ulong_val = va_arg(args, unsigned long);
						convert_ulong(ulong_val, (char *)&value_string, 32);
						stream_puts(stream, (char *)&value_string);
						break;
					case 'x':
						hecks_val = va_arg(args, unsigned long);
						convert_hecks(hecks_val, (char *)&value_string, 32);
						stream_puts(stream, (char *)&value_string);
						break;
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