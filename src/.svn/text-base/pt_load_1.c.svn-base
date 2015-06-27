/*
 * Copyright (C) 2004, 2005, 2008 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/*
 * This program print to the stdout one PT_LOAD segment.
 * p_offset member is set to 0   
 */ 
#include "my_lib.inc.c"

unsigned long my_strtoul(const char *string)
{
	unsigned long result;
	char *endptr;

	errno = 0;
	result = strtoul(string, &endptr, 0);
	if ( (*endptr != 0) || *string == 0) {
		fprintf(
			stderr,
			"%s: '%s' can't be converted with strtoul\n",
			pgm_name,
			string
		);
		exit(1);
	}
	if (errno != 0) {
		fprintf(
			stderr,
			"%s: '%s' - overflow in strtoul\n",
			pgm_name,
			string
		);
		exit(1);
	}
	return result;
}

static ElfW(Word) convert_permission(const char *permission)
{
	ElfW(Word) p_flags;
	if (strlen(permission) < 3) {
		fprintf(
			stderr,
			"%s: permission='%s' is too short. Should be at least 3 character.\n",
			pgm_name, permission
		);
		exit(1);
	}
	p_flags = 0;
	if (permission[0] == (char)'r') p_flags |= PF_R;
	if (permission[1] == (char)'w') p_flags |= PF_W;
	if (permission[2] == (char)'x') p_flags |= PF_X;
	return p_flags;
}

int main(int argc, char *argv[])
{
	const char *s_start_addr, *s_stop_addr, *s_permission;
	unsigned long start_addr, stop_addr;
	ElfW(Phdr) phdr;
	int result;

	pgm_name = argv[0];
	if (argc != 4) {
		fprintf(
			stderr, 
			"Usage: %s <start_addr> <stop_addr> <permission>\n", 
			pgm_name
		);
		exit(1);
	}
	s_start_addr = argv[1];
	s_stop_addr  = argv[2];
	s_permission = argv[3];

	start_addr = my_strtoul(s_start_addr);
	stop_addr  = my_strtoul(s_stop_addr );
	if (stop_addr < start_addr) {
		fprintf(
			stderr,
			"%s: start_addr='%s' is bigger then stop_addr='%s'\n",
			pgm_name, s_start_addr, s_stop_addr
		);
		exit(1);
	}
	phdr.p_type   = PT_LOAD;
	phdr.p_offset = 0;
	phdr.p_vaddr  = start_addr;
	phdr.p_paddr  = phdr.p_vaddr;
	phdr.p_filesz = stop_addr - start_addr;
	phdr.p_memsz  = phdr.p_filesz;
	phdr.p_flags  = convert_permission(s_permission);
	phdr.p_align  = 1;

	result = my_fwrite(&phdr, sizeof(phdr), stdout, "PT_LOAD segment", "stdout");
	if (result == -1) exit(1);
        exit(0);	
}
