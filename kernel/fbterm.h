#pragma once

#include <stdint.h>

extern uint8_t font[];

enum
{
	PSF1_FONT_MAGIC = 0x0436,
	PSF_FONT_MAGIC = 0x864AB572
};
 
typedef struct
{
	uint16_t magic;
	uint8_t font_mode;
	uint8_t character_size;
} psf1_header_t;
 
typedef struct
{
	uint32_t magic; 
	uint32_t version;
	uint32_t headersize;
	uint32_t flags;
	uint32_t numglyph;
	uint32_t bytesperglyph;
	uint32_t height;
	uint32_t width;
} psf_t;

static inline uint32_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
	return (((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b);
}

int fbterm_init();
void fbterm_drawchar(uint16_t character, int x, int y, uint32_t foreground, uint32_t background);