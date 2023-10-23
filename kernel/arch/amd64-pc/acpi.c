#include <stddef.h>

#include "acpi.h"
#include "../../limine.h"
#include "../../stream.h"
#include "../../string.h"
#include "cpu.h"

static volatile struct limine_rsdp_request rsdp_request = {
	.id = LIMINE_RSDP_REQUEST,
	.revision = 0
};

volatile struct limine_hhdm_request hhdm_request = {
	.id = LIMINE_HHDM_REQUEST,
	.revision = 0
};

rsdp_t * rsdp;
rsdt_t * rsdt;

sdt_t * acpi_get_sdt(const char * signature)
{
	size_t entry_count = (rsdt->sdt.length - sizeof(sdt_t)) / (xsdt_enabled() ? 8 : 4);

	for(size_t i = 0; i < entry_count; i++)
	{
		sdt_t * sdt = NULL;
		if(xsdt_enabled())
		{
			sdt = (sdt_t *)(*((uint64_t *)rsdt->data + i) + hhdm_request.response->offset);
		}
		else
		{
			sdt = (sdt_t *)(*((uint32_t *)rsdt->data + i) + hhdm_request.response->offset);
		}

		if(memcmp(sdt->signature, signature, 4) != 0)
		{
			continue;
		}

		return sdt;
	}
	return NULL;
}

int acpi_init()
{
	struct limine_rsdp_response * rsdp_response = rsdp_request.response;

	if(rsdp_response == NULL)
	{
		return -1;
	}

	if(rsdp_response->address == NULL)
	{
		return -2;
	}

	rsdp = rsdp_response->address;

	if(xsdt_enabled())
	{
		rsdt = (rsdt_t *)(rsdp->xsdt_addr + hhdm_request.response->offset);
	}
	else
	{
		rsdt = (rsdt_t *)((uint64_t)rsdp->rsdt_addr + hhdm_request.response->offset);
	}

	madt_t * madt = (madt_t *)acpi_get_sdt("APIC");
	if(madt == NULL)
	{
		return -3;
	}

	size_t offset = 0;
	for(;;)
	{
		if(madt->sdt.length - sizeof(madt_t) - offset < 2)
		{
			break;
		}

		header_t * header = (header_t *)(madt->entries_data + offset);

		switch(header->id)
		{
		case 0: {
				lapic_t * lapic = (lapic_t *)header;
				(void)lapic;
				stream_printf(current_stream, "[ACPI]:\033[15GDetected an LAPIC!\r\n");
			}
			break;
		case 1: {
				ioapic_t * ioapic = (ioapic_t *)header;
				(void)ioapic;
				stream_printf(current_stream, "[ACPI]:\033[15GDetected an IOAPIC!\r\n");
			}
			break;
		default: {
				stream_printf(current_stream, "[ACPI]:\033[15GDetected an unknown device, got type %d.\r\n", header->id);
			}
		}

		offset += header->length;
	}

	return 0;
}