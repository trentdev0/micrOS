#include <stdint.h>
#include <stdbool.h>

#include "virtmem.h"
#include "physmem.h"
#include "string.h"
#include "arch/amd64-pc/cpu.h"

extern char __kernel_start[];
extern char __text_start[];
extern char __text_end[];
extern char __rodata_start[];
extern char __rodata_end[];
extern char __data_start[];
extern char __data_end[];
extern char __kernel_end[];

int virtmem_init()
{
	return 0;
}