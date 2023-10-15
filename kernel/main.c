#include <stdint.h>
#include <stddef.h>

#include "limine.h"
#include "arch/amd64/amd64.h"
#include "serial.h"
#include "flanterm.h"
#include "fb.h"

static volatile struct limine_framebuffer_request framebuffer_request = { .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0 };

void _start()
{
	serial_init(COM1);

	struct flanterm_context *ft_ctx = flanterm_fb_simple_init(framebuffer_request.response->framebuffers[0]->address, framebuffer_request.response->framebuffers[0]->width, framebuffer_request.response->framebuffers[0]->height, framebuffer_request.response->framebuffers[0]->pitch);

	serial_write(COM1, 'H');
	serial_write(COM1, 'e');
	serial_write(COM1, 'l');
	serial_write(COM1, 'l');
	serial_write(COM1, 'o');
	serial_write(COM1, ',');
	serial_write(COM1, ' ');
	serial_write(COM1, 'w');
	serial_write(COM1, 'o');
	serial_write(COM1, 'r');
	serial_write(COM1, 'l');
	serial_write(COM1, 'd');
	serial_write(COM1, '!');
	serial_write(COM1, '\r');
	serial_write(COM1, '\n');
	hang();
}