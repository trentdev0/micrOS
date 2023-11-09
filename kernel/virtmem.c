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

/**
 *	@brief Set values in a page table entry.
 *	@param qword The page table entry.
 *	@param present Is the page present?
 *	@param write Should the page have write permissons?
 *	@param access Should ring3 be able to interact with the target?
 *	@param writethrough Should the target have a writethrough caching policy?
 *	@param notcacheable Is the target not cacheable?
 */

int virtmem_init()
{
	return 0;
}