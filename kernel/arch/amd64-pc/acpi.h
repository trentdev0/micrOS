#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	char signature[8];
	uint8_t checksum;
	char oem_id[6];
	uint8_t revision;
	uint32_t rsdt_addr;
	uint32_t length;
	uint64_t xsdt_addr;
	uint8_t ext_checksum;
	char reserved[3];
} rsdp_t;

typedef struct
{
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oem_id[6];
	char oem_table_id[6];
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
} sdt_t;

typedef struct
{
	sdt_t sdt;
	char data[];
} rsdt_t;

extern rsdp_t * rsdp;
extern rsdt_t * rsdt;

static inline bool xsdt_enabled()
{
	return rsdp->revision >= 2 && rsdp->xsdt_addr != 0;
}

void acpi_init();
void * acpi_get_sdt(const char * signature);