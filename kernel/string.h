#pragma once

#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
unsigned int strlen(const char * s);

static inline void convert_int8(int8_t number, char * string)
{
	if(number < 0)
	{
		*string = '-';
		string++;
		number = -number;
	}

	int8_t temp = number;
	int8_t rev = 0;
	int8_t count = 0;

	while(temp != 0)
	{
		rev = rev * 10 + temp % 10;
		temp /= 10;
		count++;
	}

	if(count == 0)
	{
		*string = '0';
		string++;
	}
	else
	{
		while(count > 0)
		{
			*string = '0' + rev % 10;
			rev /= 10;
			string++;
			count--;
		}
	}

	*string = '\0';
}

static inline void convert_uint8(uint8_t number, char * string)
{
	uint8_t temp = number;
	uint8_t rev = 0;
	uint8_t count = 0;

	while(temp != 0)
	{
		rev = rev * 10 + temp % 10;
		temp /= 10;
		count++;
	}

	if(count == 0)
	{
		*string = '0';
		string++;
	}
	else
	{
		while(count > 0)
		{
			*string = '0' + rev % 10;
			rev /= 10;
			string++;
			count--;
		}
	}

	*string = '\0';
}

static inline void convert_int16(int16_t number, char * string)
{
	if(number < 0)
	{
		*string = '-';
		string++;
		number = -number;
	}

	int16_t temp = number;
	int16_t rev = 0;
	int16_t count = 0;

	while(temp != 0)
	{
		rev = rev * 10 + temp % 10;
		temp /= 10;
		count++;
	}

	if(count == 0)
	{
		*string = '0';
		string++;
	}
	else
	{
		while(count > 0)
		{
			*string = '0' + rev % 10;
			rev /= 10;
			string++;
			count--;
		}
	}

	*string = '\0';
}

static inline void convert_uint16(uint16_t number, char * string)
{
	uint16_t temp = number;
	uint16_t rev = 0;
	uint16_t count = 0;

	while(temp != 0)
	{
		rev = rev * 10 + temp % 10;
		temp /= 10;
		count++;
	}

	if(count == 0)
	{
		*string = '0';
		string++;
	}
	else
	{
		while(count > 0)
		{
			*string = '0' + rev % 10;
			rev /= 10;
			string++;
			count--;
		}
	}

	*string = '\0';
}

static inline void convert_int32(int32_t number, char * string)
{
	if(number < 0)
	{
		*string = '-';
		string++;
		number = -number;
	}

	int32_t temp = number;
	int32_t rev = 0;
	int32_t count = 0;

	while(temp != 0)
	{
		rev = rev * 10 + temp % 10;
		temp /= 10;
		count++;
	}

	if(count == 0)
	{
		*string = '0';
		string++;
	}
	else
	{
		while(count > 0)
		{
			*string = '0' + rev % 10;
			rev /= 10;
			string++;
			count--;
		}
	}

	*string = '\0';
}

static inline void convert_uint32(uint32_t number, char * string)
{
	uint32_t temp = number;
	uint32_t rev = 0;
	uint32_t count = 0;

	while(temp != 0)
	{
		rev = rev * 10 + temp % 10;
		temp /= 10;
		count++;
	}

	if(count == 0)
	{
		*string = '0';
		string++;
	}
	else
	{
		while(count > 0)
		{
			*string = '0' + rev % 10;
			rev /= 10;
			string++;
			count--;
		}
	}

	*string = '\0';
}

static inline void convert_int64(int64_t number, char * string)
{
	if(number < 0)
	{
		*string = '-';
		string++;
		number = -number;
	}

	int64_t temp = number;
	int64_t rev = 0;
	int64_t count = 0;

	while(temp != 0)
	{
		rev = rev * 10 + temp % 10;
		temp /= 10;
		count++;
	}

	if(count == 0)
	{
		*string = '0';
		string++;
	}
	else
	{
		while(count > 0)
		{
			*string = '0' + rev % 10;
			rev /= 10;
			string++;
			count--;
		}
	}

	*string = '\0';
}

static inline void convert_uint64(uint64_t number, char * string)
{
	uint64_t temp = number;
	uint64_t rev = 0;
	uint64_t count = 0;

	while(temp != 0)
	{
		rev = rev * 10 + temp % 10;
		temp /= 10;
		count++;
	}

	if(count == 0)
	{
		*string = '0';
		string++;
	}
	else
	{
		while(count > 0)
		{
			*string = '0' + rev % 10;
			rev /= 10;
			string++;
			count--;
		}
	}

	*string = '\0';
}