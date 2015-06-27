/*
 * Copyright (C) 2010 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

void dump_tls_data(pid_t pid, FILE *f)
{
	unsigned long ebx;
	unsigned long entry_number;
	unsigned long base;
	unsigned long limit;
	unsigned long flags;
	ebx = get_register(pid, EBX);
	get_memory(
		pid, 
		(const void *)(ebx + sizeof(long) * 0),
		(const void *)(ebx + sizeof(long) * 1),
		(char *)&entry_number
	);
	get_memory(
		pid, 
		(const void *)(ebx + sizeof(long) * 1),
		(const void *)(ebx + sizeof(long) * 2),
		(char *)&base
	);
	get_memory(
		pid, 
		(const void *)(ebx + sizeof(long) * 2),
		(const void *)(ebx + sizeof(long) * 3),
		(char *)&limit
	);
	get_memory(
		pid, 
		(const void *)(ebx + sizeof(long) * 3),
		(const void *)(ebx + sizeof(long) * 4),
		(char *)&flags
	);

	fprintf(f, "syscall_number: 0x%x %u\n", TLS_SYSCALL, TLS_SYSCALL);
	fprintf(f, "entry_number:   0x%lx %lu\n", entry_number, entry_number);
	fprintf(f, "base:           0x%lx %lu\n", base        , base        );
	fprintf(f, "limit:          0x%lx %lu\n", limit       , limit       );
	fprintf(f, "flags:          0x%lx %lu\n", flags       , flags       );
}
