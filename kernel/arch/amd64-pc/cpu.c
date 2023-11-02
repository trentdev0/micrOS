#include "cpu.h"

/* Hang a CPU. */
void hang()
{
	cli();
	for(;;)
	{
		hlt();
	}
}