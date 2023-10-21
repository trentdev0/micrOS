#pragma once

#include <stdint.h>

typedef enum
{
	COM1 = 0x3F8,
	COM2 = 0x2F8,
	COM3 = 0x3E8,
	COM4 = 0x2E8,
	COM5 = 0x5F8,
	COM6 = 0x4F8,
	COM7 = 0x5E8,
	COM8 = 0x4E8
} serial_port_t;

int serial_init(serial_port_t serial_port);

void serial_write(serial_port_t serial_port, uint8_t byte);
uint8_t serial_read(serial_port_t serial_port);

void com1_write(char character);
char com1_read();
void com2_write(char character);
char com2_read();
void com3_write(char character);
char com3_read();
void com4_write(char character);
char com4_read();
void com5_write(char character);
char com5_read();
void com6_write(char character);
char com6_read();
void com7_write(char character);
char com7_read();
void com8_write(char character);
char com8_read();