#include <stdint.h>

#include "pic.h"
#include "arch/amd64/cpu.h"

void pic_init()
{
	outb(MASTER_COMMAND, 0x11);
	outb(SLAVE_COMMAND, 0x11);

	outb(MASTER_DATA, 0x20);
	outb(SLAVE_DATA, 0x28);

	outb(MASTER_DATA, 0b100);
	outb(SLAVE_DATA, 0b010);

	outb(MASTER_DATA, 0x01);
	outb(SLAVE_DATA, 0x01);

	outb(MASTER_DATA, 0xFF);
	outb(SLAVE_DATA, 0xFF);
}