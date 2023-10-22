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

void acpi_init()
{
	struct limine_rsdp_response * rsdp_response = rsdp_request.response;

	if(rsdp_response == NULL)
	{
		stream_printf(current_stream, "RSDP response is set to null!\r\nHanging...\r\n");
		hang();
	}

	if(rsdp_response->address == NULL)
	{
		stream_printf(current_stream, "RSDP address is set to null!\r\nHanging...\r\n");
		hang();
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
}

void * acpi_get_sdt(const char * signature)
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