/*
 * Copyright (C) 2010 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

#include "registers.h"
#include "my_ptrace.h"
#include <sys/reg.h>
#include "processor.h"

static const char *reg_names[] = {
	#include "regs.c"
};

struct user_regs_struct get_registers(pid_t pid)
{
	struct user_regs_struct user;
	MY_PTRACE(PTRACE_GETREGS, pid, NULL, &user, 1, 1);
	return user;
}
void dump_regs(FILE *file, struct user_regs_struct *user, int hit_breakpoint)
{
	size_t i;
	signed long *registers;
	registers = (signed long *)user;
	for (i = 0; i < ARRAY_SIZE(reg_names); i++) {
		unsigned long value;
		value = (unsigned long)registers[i];
		if (hit_breakpoint && i == PC_REG) value--;
		fprintf(
			file,
			"%s 0x%lx %lu\n",
			reg_names[i],
			value,
			value
		);
	}
}

unsigned long get_register(pid_t pid, size_t reg_offset)
{
	unsigned long reg;
	reg = MY_PTRACE(
		PTRACE_PEEKUSER, 
		pid, 
		(void *)(reg_offset * sizeof(long)), 
		NULL, 
		1, 
		1
	);
	return reg;
}

unsigned long get_pc(pid_t pid)
{
	return get_register(pid, PC_REG);
}
unsigned long get_sp(pid_t pid)
{
	return get_register(pid, SP_REG);
}
void set_register(pid_t pid, unsigned long reg, size_t reg_offset)
{
	MY_PTRACE(
		PTRACE_POKEUSER, 
		pid, 
		(void *)(reg_offset * sizeof(long)),
		(void *)reg, 
		1, 
		1
	);
}
void set_pc(pid_t pid, unsigned long reg)
{
	set_register(pid, reg, PC_REG);
}
void set_sp(pid_t pid, unsigned long reg)
{
	set_register(pid, reg, SP_REG);
}
