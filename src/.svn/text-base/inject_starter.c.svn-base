/*
 * Copyright (C) 2004, 2005 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/* 
 * This program "inject" starter code to the executable in
 * the place specified by the e_entry in the ehdr.
 * Destination file updated in-place via mmap
 */
#include "my_lib.inc.c"

int main(int argc, char *argv[])
{
	const char *starter_name;
	const char *exe_name;
	FILE *exe_file;
	int err;
	ElfW(Ehdr) ehdr_exe;
	ElfW(Phdr) *phdrs_exe;
	off_t offset = 0;
	off_t exe_size;
	off_t starter_size;
	unsigned char *starter_mem;
	pgm_name = argv[0];

	if (argc != 3) {
		fprintf(
			stderr,
			"Usage: %s <starter> <exe>\n",
			pgm_name
		);
		exit(1);
	}

	starter_name = argv[1];
	exe_name = argv[2];

	/* Get executable's ehdr */
	err = get_ehdr_phdrs_and_shdrs(
		exe_name,
		&ehdr_exe,
		&phdrs_exe,
		NULL,
		NULL,
		NULL
	);
	if (err == -1) exit(1);

	/* Find where in the file starter should be placed */
	{
		int i;
		for (i = 0; i < ehdr_exe.e_phnum; i++) {
			/* It should be in the PT_LOAD segment */
			if (phdrs_exe[i].p_type != PT_LOAD) continue;
			if (
				(ehdr_exe.e_entry >= phdrs_exe[i].p_vaddr) &&
				(ehdr_exe.e_entry <  phdrs_exe[i].p_vaddr + phdrs_exe[i].p_memsz)
			) {
				offset = 
					ehdr_exe.e_entry - phdrs_exe[i].p_vaddr + 
					phdrs_exe[i].p_offset;
				break;
			}
		}
	}
	if (offset == 0) {
		fprintf(
			stderr,
			"%s: can't find PT_LOAD segment with e_entry=0x%lx.\n",
			pgm_name, (unsigned long)ehdr_exe.e_entry
		);
		exit(1);
	}

	/* Get size of the exe file */
	exe_size = my_file_size(exe_name, &err);
	if (err == -1) exit(1);

	/* read starter file */
	starter_mem = my_fread_whole_file(
			starter_name, 
			"starter", 
			&starter_size
	);

	/* Sanity */
	if (exe_size < (starter_size + offset) ) {
		fprintf(
			stderr,
			"%s: mismatch: starter offset=%lu(0x%lx) + starter size=%lu > exe_size=%lu\n",
			pgm_name, offset, offset, starter_size, exe_size
		);
		exit(1);
	}

	/* Open for read/write */
	exe_file = my_fopen(exe_name, "r+");
	if (exe_file == NULL) exit(1);

	/* Position file to the place where starter should begin */
	err = my_fseek(exe_file, offset, exe_name);
	if (err == -1) exit(1);

	/* Write starter */
	err = my_fwrite(
		starter_mem, 
		starter_size, 
		exe_file, 
		exe_name, 
		"starter"
	);
	if (err == -1) exit(1);

	/* Close file */
	err = my_fclose(exe_file, exe_name);
	if (err == -1) exit(1);

	/* all done, exit */
	exit(0);
}
