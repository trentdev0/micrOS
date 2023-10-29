#pragma once

#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
unsigned int strlen(const char * s);
char * strcpy(char * destination, const char * source);

static inline void convert_hecks(unsigned long num, char *buffer, int buffer_size)
{
	int i = 0;

	do
	{
		unsigned long remainder = num % 16;
		if(remainder < 10)
		{
			buffer[i++] = '0' + remainder;
		}
		else
		{
			buffer[i++] = 'A' + (remainder - 10);
		}
		num /= 16;
	} while(num > 0 && i < buffer_size - 1);

	buffer[i] = '\0';

	int len = i;
	for(int j = 0; j < len / 2; j++)
	{
		char temp = buffer[j];
		buffer[j] = buffer[len - j - 1];
		buffer[len - j - 1] = temp;
	}
}


static inline void convert_long(long num, char * buffer, int buffer_size)
{
	int i = 0;
	int is_negative = 0;

	if(num < 0)
	{
		is_negative = 1;
		num = -num;
	}

	do
	{
		buffer[i++] = num % 10 + '0';
		num /= 10;
	} while (num > 0 && i < buffer_size - 1);

	if(is_negative && i < buffer_size - 1)
	{
		buffer[i++] = '-';
	}

	buffer[i] = '\0';

	int len = i;
	for(int j = 0; j < len / 2; j++)
	{
		char temp = buffer[j];
		buffer[j] = buffer[len - j - 1];
		buffer[len - j - 1] = temp;
	}
}

static inline void convert_ulong(unsigned long num, char * buffer, int buffer_size)
{
	int i = 0;

	do
	{
		buffer[i++] = num % 10 + '0';
		num /= 10;
	} while (num > 0 && i < buffer_size - 1);

	buffer[i] = '\0';

	int len = i;
	for(int j = 0; j < len / 2; j++)
	{
		char temp = buffer[j];
		buffer[j] = buffer[len - j - 1];
		buffer[len - j - 1] = temp;
	}
}

static inline void convert_hex(unsigned int num, char * buffer, int buffer_size)
{
	int i = 0;

	do
	{
		int remainder = num % 16;
		if(remainder < 10)
		{
			buffer[i++] = '0' + remainder;
		}
		else
		{
			buffer[i++] = 'A' + (remainder - 10);
		}
		num /= 16;
	} while(num > 0 && i < buffer_size - 1);

	buffer[i] = '\0';

	int len = i;
	for(int j = 0; j < len / 2; j++)
	{
		char temp = buffer[j];
		buffer[j] = buffer[len - j - 1];
		buffer[len - j - 1] = temp;
	}
}

static inline void convert_int(int num, char * buffer, int buffer_size)
{
	int i = 0;
	int is_negative = 0;

	if(num < 0)
	{
		is_negative = 1;
		num = -num;
	}

	do
	{
		buffer[i++] = num % 10 + '0';
		num /= 10;
	} while (num > 0 && i < buffer_size - 1);

	if(is_negative && i < buffer_size - 1)
	{
		buffer[i++] = '-';
	}

	buffer[i] = '\0';

	int len = i;
	for(int j = 0; j < len / 2; j++)
	{
		char temp = buffer[j];
		buffer[j] = buffer[len - j - 1];
		buffer[len - j - 1] = temp;
	}
}

static inline void convert_uint(unsigned int num, char * buffer, int buffer_size)
{
	int i = 0;

	do
	{
		buffer[i++] = num % 10 + '0';
		num /= 10;
	} while(num > 0 && i < buffer_size - 1);

	buffer[i] = '\0';

	int len = i;
	for(int j = 0; j < len / 2; j++)
	{
		char temp = buffer[j];
		buffer[j] = buffer[len - j - 1];
		buffer[len - j - 1] = temp;
	}
}