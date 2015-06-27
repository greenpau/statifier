/*
 * Copyright (C) 2004 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/*
 * This program check if elf stripped.
 * if yes - print out "stripped".
 */

#include "./my_lib.inc.c"
#include <limits.h>

int main(int argc, char *argv[])
{
	ElfW(Ehdr) ehdr;
	ElfW(Shdr) *shdrs;
	const char *file;
	int ind;

	pgm_name = argv[0];	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file>\n", pgm_name);
		exit(1);
	}

	file = argv[1];

	/* Get ehdr + shdrs */
	if ( 
		get_ehdr_phdrs_and_shdrs(
			file,
			&ehdr,
			NULL,
			NULL,
			&shdrs,
			NULL
		) == 0
	) exit(1);

	/* Sanity. Verify that it's really ELF */
	if (
		(ehdr.e_ident[EI_MAG0] != ELFMAG0) ||
		(ehdr.e_ident[EI_MAG1] != ELFMAG1) ||
		(ehdr.e_ident[EI_MAG2] != ELFMAG2) ||
		(ehdr.e_ident[EI_MAG3] != ELFMAG3)
	) {
		fprintf(
			stderr, 
			"%s: file '%s' is not elf file.\n",
			pgm_name, file
		);
		exit(1);
	}
	for (ind = 0; ind < ehdr.e_shnum; ind++) {
		if (shdrs[ind].sh_type == SHT_SYMTAB) {
			/* File was not stripped */
			exit(0);
		}
	}

	printf("stripped\n");
 	exit(0);
	return 0;
}
