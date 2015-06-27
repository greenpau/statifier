/*
 * Copyright (C) 2005 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/*
 * This program is print elf type of the file '32' or '64'
 */

#include <stdio.h>
#include <stdlib.h>
#include <elf.h>

int main(int argc, char *argv[])
{
	FILE *input;
	char *pgm_name;
	char *file_name;
	size_t count;
	unsigned char buffer[EI_CLASS + 1];
	pgm_name = argv[0];	
	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file>\n", pgm_name);
		exit(1);
	}
	
	file_name = argv[1];

	input = fopen(file_name, "r");
	if (input == NULL) {
		fprintf(
			stderr,
			"%s: can't open file '%s'\n", 
			pgm_name, 
			file_name
		);
		exit(1);
	}
	count = fread(buffer, 1, sizeof(buffer), input);
	if (count != sizeof(buffer) ) {
		fprintf(
			stderr,
			"%s: can't read ELF header from file '%s'\n",
			pgm_name, 
			file_name
		);
		exit(1);
	}

	if (
		(buffer[0] != ELFMAG0) ||
		(buffer[1] != ELFMAG1) ||
		(buffer[2] != ELFMAG2) ||
		(buffer[3] != ELFMAG3)
	) {
		fprintf(
			stderr,
			"%s: file '%s' is not ELF file.\n",
			pgm_name, 
			file_name
		);
		exit(1);
	}

	switch(buffer[EI_CLASS]) {
		case ELFCLASS32:
			printf("32\n");
		break;

		case ELFCLASS64:
			printf("64\n");
		break;

		default:
			fprintf(
				stderr,
				"%s: file '%s' has unknown elf class (%d)\n",
				pgm_name,
				file_name,
				buffer[EI_CLASS]
			);
		exit(1);
	}
	exit(0);
}
