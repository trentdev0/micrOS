#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	/* Is the target present? */
	uint64_t present : 1;
	/* Should the target have write permissions? */
	uint64_t write : 1;
	/* Should ring3 be able to interact with the target? */
	uint64_t access : 1;
	/* Should the target have a writethrough caching policy? */
	uint64_t writethrough : 1;
	/* Is the target not cacheable? */
	uint64_t notcacheable : 1;
	uint64_t accessed : 1;
	uint64_t unused0 : 1;
	uint64_t global : 1;
	uint64_t unused1 : 51;
	uint64_t memprotkey : 4;
	uint64_t noexec : 1;
} pagetable_lowest_t;

typedef struct
{
	/* Is the target present? */
	uint64_t present : 1;
	/* Should the target have write permissions? */
	uint64_t write : 1;
	/* Should ring3 be able to interact with the target? */
	uint64_t access : 1;
	/* Should the target have a writethrough caching policy? */
	uint64_t writethrough : 1;
	/* Is the target not cacheable? */
	uint64_t notcacheable : 1;
	uint64_t unused0 : 1;
	
	/*
	 *	It's actually used, we just set it to zero in order to allow
	 *	4KiB pages.
	 */
	uint64_t unused1 : 1;
	uint64_t unused1 : 52;
	uint64_t memprotkey : 4;
	uint64_t noexec : 1;
} pagetable_directory_t;

int virtmem_init();