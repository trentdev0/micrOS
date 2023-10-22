#include <stdint.h>

#include "arch/amd64-pc/cpu.h"
#include "serial.h"

int serial_init(serial_port_t serial_port)
{
	outb(serial_port + 1, 0x00);
	outb(serial_port + 3, 0x80);
	outb(serial_port + 0, 0x03);
	outb(serial_port + 1, 0x00);
	outb(serial_port + 3, 0x03);
	outb(serial_port + 2, 0xC7);
	outb(serial_port + 4, 0x0B);
	outb(serial_port + 4, 0x1E);
	outb(serial_port + 0, 0xAE);

	if(inb(serial_port + 0) != 0xAE) 
	{
		return -1;
	}

	outb(serial_port + 4, 0x0F);
	return 0;
}


void serial_write(serial_port_t serial_port, uint8_t byte)
{
	while (!(inb(serial_port + 5) & 0x20));
	outb(serial_port, byte);
}

uint8_t serial_read(serial_port_t serial_port)
{
	while (!(inb(serial_port + 5) & 1));
	return inb(serial_port);
}

void com1_write(char character) { serial_write(COM1, character); }
char com1_read() { return serial_read(COM1); }
void com2_write(char character) { serial_write(COM2, character); }
char com2_read() { return serial_read(COM2); }
void com3_write(char character) { serial_write(COM3, character); }
char com3_read() { return serial_read(COM3); }
void com4_write(char character) { serial_write(COM4, character); }
char com4_read() { return serial_read(COM4); }
void com5_write(char character) { serial_write(COM5, character); }
char com5_read() { return serial_read(COM5); }
void com6_write(char character) { serial_write(COM6, character); }
char com6_read() { return serial_read(COM6); }
void com7_write(char character) { serial_write(COM7, character); }
char com7_read() { return serial_read(COM7); }
void com8_write(char character) { serial_write(COM8, character); }
char com8_read() { return serial_read(COM8); }