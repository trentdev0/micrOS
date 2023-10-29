#include "virtmem.h"

pagemap_t kernel_pagemap;

void virtmem_init()
{
	return;
	virtmem_switch(&kernel_pagemap);
}