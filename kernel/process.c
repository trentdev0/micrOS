#include "process.h"
#include "main.h"
#include "string.h"

uint64_t pid_bump = 0, tid_bump = 0;

/* Generate a process ID by bumping `pid_bump` by 1. */
static inline uint64_t generate_pid()
{
	return pid_bump++;
}

/* Generate a thread ID by bumping `tid_bump` by 1. */
static inline uint64_t generate_tid()
{
	return tid_bump++;
}

void process_init()
{
	strcpy(process.name, "kernel");
	process.pid = generate_pid();
}