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

typedef struct
{
	sdt_t sdt;
	uint32_t local_controller_addr;
	uint32_t flags;
	char entries_data[];
} madt_t;

typedef struct
{
	uint8_t id;
	uint8_t length;
} __attribute__((packed)) header_t;

typedef struct
{
	uint8_t id;
	uint8_t length;
	uint8_t processor_id;
	uint8_t apic_id;
	uint32_t flags;
} __attribute__((packed)) lapic_t;

extern rsdp_t * rsdp;
extern rsdt_t * rsdt;

static inline bool xsdt_enabled()
{
	return rsdp->revision >= 2 && rsdp->xsdt_addr != 0;
}

int acpi_init();
sdt_t * acpi_get_sdt(const char * signature);