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
	unsigned long rdi;
	unsigned long rsi;

	rdi = get_register(pid, RDI);
	rsi = get_register(pid, RSI);

	fprintf(f, "syscall_number: 0x%x %u\n", TLS_SYSCALL, TLS_SYSCALL);
	fprintf(f, "func_number:    0x%lx %lu\n", rdi, rdi);
	fprintf(f, "address:        0x%lx %lu\n", rsi, rsi);
}
