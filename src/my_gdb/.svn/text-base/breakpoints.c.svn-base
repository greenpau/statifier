/*
 * Copyright (C) 2010 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

#include "breakpoints.h"
#include "registers.h"
#include "my_ptrace.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

breakpoint_t breakpoints[MAX_BREAKPOINTS];
size_t num_breakpoints = 0;

void breakpoint_add(pid_t pid, unsigned long addr)
{
	long result;
	size_t offset;
	unsigned char *str;
	unsigned long aligned_addr;

	if (num_breakpoints + 1 == MAX_BREAKPOINTS) {
		fprintf(
			stderr,
			"%s: already have maximum number of breakpoints\n",
			pgm_name
		);
		exit(1);
	}
	offset = addr % sizeof(long);
	aligned_addr = addr - offset;
	result = MY_PTRACE(
		PTRACE_PEEKTEXT, 
		pid, 
		(void *)aligned_addr, 
		NULL, 
		1, 
		1
	);
	breakpoints[num_breakpoints].old_value = result;

	str = (unsigned char *)&result;
	str[offset] = 0xCC;
	breakpoints[num_breakpoints].new_value = result;
	breakpoints[num_breakpoints].addr = addr;
	breakpoints[num_breakpoints].aligned_addr = aligned_addr;
	MY_PTRACE(
		PTRACE_POKETEXT, 
		pid, 
		(void *)aligned_addr, 
		(void *)result, 
		1, 
		1
	);
	breakpoint_activate(pid, num_breakpoints);
	num_breakpoints++;
}

void breakpoint_do(pid_t pid, size_t idx)
{
	int status;

	if (! breakpoints[idx].active) return;

	breakpoint_deactivate(pid, idx);

	set_pc(pid, get_pc(pid) - 1);

	MY_PTRACE(PTRACE_SINGLESTEP, pid, NULL, NULL, 1, 1);
	waitpid(pid, &status, 0);
	breakpoint_activate(pid, idx);
}

void breakpoint_deactivate(pid_t pid, size_t idx)
{
	if (! breakpoints[idx].active) return;

	MY_PTRACE(
		PTRACE_POKETEXT, 
		pid, 
		(void *)breakpoints[idx].aligned_addr, 
		(void *)breakpoints[idx].old_value, 
		1, 
		1
	);
	breakpoints[idx].active = 0;
}

void breakpoint_activate(pid_t pid, size_t idx)
{
	if (breakpoints[idx].active) return;

	MY_PTRACE(
		PTRACE_POKETEXT, 
		pid, 
		(void *)breakpoints[idx].aligned_addr, 
		(void *)breakpoints[idx].new_value, 
		1, 
		1
	);
	breakpoints[idx].active = 1;
}
