#pragma once

#include <stdint.h>

extern uint64_t kernel_minimum, kernel_maximum, kernel_size;
extern uint64_t kernel_physical_minimum, kernel_physical_maximum, kernel_physical_size;
extern uint64_t virtual_address_minimum, virtual_address_maximum, virtual_address_size;

int range_init();