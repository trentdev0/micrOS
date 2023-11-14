#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "acpi.h"
#include "cpu.h"
#include "../../heap.h"
#include "../../string.h"
#include "../../ansi.h"
#include "../../terminal.h"
#include "../../physmem.h"
#include "../../thirdparty/limine.h"

static volatile struct limine_rsdp_request rsdp_request = {
	.id = LIMINE_RSDP_REQUEST,
	.revision = 0
};

rsdp_t * rsdp = NULL;
rsdt_t * rsdt = NULL;

uint64_t madt_lapic_size = 0, madt_ioapic_size = 0, madt_iso_size = 0, madt_nmi_size = 0;
uint64_t madt_lapic_bump = 0, madt_ioapic_bump = 0, madt_iso_bump = 0, madt_nmi_bump = 0;
madt_lapic_t *madt_lapic = NULL;
madt_ioapic_t *madt_ioapic = NULL;
madt_iso_t *madt_iso = NULL;
madt_nmi_t *madt_nmi = NULL;

int acpi_init()
{
	struct limine_rsdp_response *rsdp_response = rsdp_request.response;

	if(rsdp_response == NULL || rsdp_response->address == NULL)
	{
		terminal_printf(current_terminal, "[" BOLD_RED "ACPI" RESET "]:" ALIGN "ACPI isn't supported in this machine!\r\n");
		return -1;
	}

	rsdp = rsdp_response->address;

	if(xsdt_enabled())
	{
		rsdt = (rsdt_t *)(rsdp->xsdt_address + hhdm_request.response->offset);
	}
	else
	{
		rsdt = (rsdt_t *)((uint64_t)rsdp->rsdt_address + hhdm_request.response->offset);
	}

	sdt_t *fadt = acpi_find("FACP", 0);

	if(fadt != NULL && fadt->length >= 116)
	{
		uint32_t fadt_flags = *((uint32_t *)fadt + 28);

		if((fadt_flags & (1 << 20)) != 0)
		{
			terminal_printf(current_terminal, "[" BOLD_RED "ACPI" RESET "]:" ALIGN "This system is not supported.\r\n");
			return -2;
		}
	}

	madt_t * madt = acpi_find("APIC", 0);
	if(madt == NULL)
	{
		terminal_printf(current_terminal, "[" BOLD_RED "ACPI" RESET "]:" ALIGN "This system is not supported.\r\n");
		return -3;
	}

	size_t offset = 0;
	for(;;)
	{
		if(madt->length - sizeof(madt_t) - offset < 2)
		{
			break;
		}

		madt_header_t * madt_header = (madt_header_t *)(madt->entries_data + offset);

		terminal_printf(current_terminal, "[" BOLD_RED "ACPI" RESET "]:" ALIGN "Detected a device (id=" BOLD_WHITE "%u" RESET ")!\r\n", madt_header->id);

		switch(madt_header->id)
		{
		case 0:
			madt_lapic_size += sizeof(madt_lapic_t);
			madt_lapic = krealloc(madt_lapic, madt_lapic_size);
			madt_lapic_bump++;
			break;
		case 1:
			madt_ioapic_size += sizeof(madt_ioapic_t);
			madt_ioapic = krealloc(madt_ioapic, madt_ioapic_size);
			madt_ioapic_bump++;
			break;
		case 2:
			madt_iso_size += sizeof(madt_iso_t);
			madt_iso = krealloc(madt_iso, madt_iso_size);
			madt_iso_bump++;
			break;
		case 4:
			madt_nmi_size += sizeof(madt_nmi_t);
			madt_nmi = krealloc(madt_nmi, madt_nmi_size);
			madt_nmi_bump++;
			break;
		}

		offset += madt_header->length > 2 ? madt_header->length : 2;
	}

	return 0;
}

void *acpi_find(const char signature[static 4], uint64_t index)
{
	uint64_t entry_count = (rsdt->length - sizeof(sdt_t)) / (xsdt_enabled() ? 8 : 4);

	for(uint64_t i = 0; i < entry_count; i++)
	{
		sdt_t *sdt = NULL;
		if(xsdt_enabled())
		{
			sdt = (sdt_t *)(*((uint64_t*)rsdt->data + i) + hhdm_request.response->offset);
		}
		else
		{
			sdt = (sdt_t *)(*((uint32_t*)rsdt->data + i) + hhdm_request.response->offset);
		}

		if(memcmp(sdt->signature, signature, 4) != 0)
		{
			continue;
		}

		if(index > 0)
		{
			index--;
			continue;
		}

		return sdt;
	}

	return NULL;
}