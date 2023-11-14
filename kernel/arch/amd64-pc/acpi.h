#pragma once

#include <stdint.h>
#include <stdbool.h>

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
	char signature[8];
	uint8_t checksum;
	char oem_id[6];
	uint8_t revision;
	uint32_t rsdt_address;
	uint32_t length;
	uint64_t xsdt_address;
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
	char data[];
} rsdt_t;

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
	uint32_t local_controller_addr;
	uint32_t flags;
	char entries_data[];
} madt_t;

typedef struct
{
	uint8_t id;
	uint8_t length;
} __attribute__((packed)) madt_header_t;

typedef struct
{
	uint8_t id;
	uint8_t length;
	uint8_t processor_id;
	uint8_t apic_id;
	uint32_t flags;
} __attribute__((packed)) madt_lapic_t;

typedef struct
{
	uint8_t id;
	uint8_t length;
	uint8_t apic_id;
	uint8_t reserved;
	uint32_t address;
	uint32_t gsib;
} __attribute__((packed)) madt_ioapic_t;

typedef struct
{
	uint8_t id;
	uint8_t length;
	uint8_t bus_source;
	uint8_t irq_source;
	uint32_t gsi;
	uint16_t flags;
} __attribute__((packed)) madt_iso_t;

typedef struct
{
	uint8_t id;
	uint8_t length;
	uint8_t processor;
	uint16_t flags;
	uint8_t lint;
} __attribute__((packed)) madt_nmi_t;

extern rsdp_t *rsdp;
extern rsdt_t *rsdt;

static inline bool xsdt_enabled() { return rsdp->revision >= 2 && rsdp->xsdt_address != 0; }

int acpi_init();
void *acpi_find(const char signature[static 4], uint64_t index);