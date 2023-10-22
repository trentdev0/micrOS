#pragma once

#include <stdint.h>

#define MASTER_COMMAND = 0x20
#define SLAVE_COMMAND = 0xA0
#define MASTER_DATA = MASTER_COMMAND + 1
#define SLAVE_DATA = SLAVE_COMMAND + 1

void pic_init();

static inline void pic_eoi(int interrupt)
{
	outb(interrupt < 8 ? MASTER_COMMAND : SLAVE_COMMAND, 0x20);
}