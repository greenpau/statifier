/*
 * Copyright (C) 2004, 2005 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/*
 * This program print out "linux-gate" if this file is linux-gate
 * shared object and nothing otherwise
 */
#include "./my_lib.inc.c"

int main(int argc, char *argv[])
{
	ElfW(Ehdr) *maybe_ehdr;
	ElfW(Ehdr) ehdr;
	ElfW(Phdr) *phdrs;
	ElfW(Phdr) *dyn_phdr = NULL;
	ElfW(Shdr) *shdrs;
	ElfW(Shdr) *str_shdr = NULL;
	ElfW(Dyn) *dyn_seg;
	int result;
	const char *file_name;
	off_t file_size;
	unsigned char *strtab;

	pgm_name = argv[0];	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file_name>\n", pgm_name);
		exit(1);
	}

	file_name = argv[1];

	maybe_ehdr = (ElfW(Ehdr) *)my_fread_from_position(
			file_name, 
			0L, 
			sizeof(ElfW(Ehdr)), 
			"ehdr"
	);

	if (
		(maybe_ehdr->e_ident[EI_MAG0] != ELFMAG0) ||
		(maybe_ehdr->e_ident[EI_MAG1] != ELFMAG1) ||
		(maybe_ehdr->e_ident[EI_MAG2] != ELFMAG2) ||
		(maybe_ehdr->e_ident[EI_MAG3] != ELFMAG3)
	) {
		/* It's not ELF */
		exit(0);
	}

	if (maybe_ehdr->e_type != ET_DYN) {
		/* It's not Shared object */
 		exit(0);
	}

	if (maybe_ehdr->e_phnum == 0 || maybe_ehdr->e_shnum == 0) {
		/* It's got some strange header */
		exit(0);
	}

	/* Get file size */
	{
		int err;
		file_size = my_file_size(file_name, &err);
		if (err == -1) exit(1);
	}

	if ( 
		maybe_ehdr->e_phoff + (maybe_ehdr->e_phnum * sizeof(ElfW(Ehdr))) > file_size
	 ) {
		/* Phdrs is out of this file */
		exit(0);
	}

	if ( 
		maybe_ehdr->e_shoff + (maybe_ehdr->e_shnum * sizeof(ElfW(Shdr))) > file_size
	 ) {
		/* Shdrs is out of this file */
		exit(0);
	}
	result = get_ehdr_phdrs_and_shdrs(
		file_name,
		&ehdr,
		&phdrs,
		NULL,
		&shdrs,
		NULL
	);
	if (result == -1) exit(1);

	/* Try to find dynamic segment */
	{
		int i;
		ElfW(Phdr) *phdr;
		for (i = 0, phdr = phdrs; i < ehdr.e_phnum; i++, phdr++) {
			if (phdr->p_type == PT_DYNAMIC) {
				dyn_phdr = phdr;
				break;
			}
		}
	}
	if (dyn_phdr == NULL) {
		/* No dynamic segment found */
		exit(0);
	}

	if ( (dyn_phdr->p_offset + dyn_phdr->p_filesz) > file_size) {
		/* No, dynamic segment is out of this dump file */
		exit(0);
	}
		
	/* Try to find strtab for dynamic section */
	{
		int i;
		ElfW(Shdr) *shdr;
		ElfW(Word) sh_link;
		for (i = 0, shdr = shdrs; i < ehdr.e_shnum; i++, shdr++) {
			if (shdr->sh_type == SHT_DYNAMIC) {
				/* Where is string table ? */
				sh_link = shdr->sh_link;
				if ((sh_link < 0) || (sh_link >= ehdr.e_shnum)){
					/* Problem with sh_link */
					exit(0);
				}
				str_shdr = &shdrs[sh_link];
				break;
			}
		}
	}

	if (str_shdr == NULL) {
		/* Not found */
		exit(0);
	}
	if ( (str_shdr->sh_offset + str_shdr->sh_size) > file_size) {
		/* No, strtab section is out of this dump file */
		exit(0);
	}
		
	strtab = my_fread_from_position(
		file_name,
		str_shdr->sh_offset,
		str_shdr->sh_size,
		"strtab section"
	);

	dyn_seg = (ElfW(Dyn) *)my_fread_from_position(
		file_name,
		dyn_phdr->p_offset,
		dyn_phdr->p_filesz,
		"dynamic segment"
	);

	{
		ElfW(Word) soname_offset = -1;
		ElfW(Dyn) *dyn;
		for(dyn = dyn_seg; dyn->d_tag != DT_NULL; dyn++) {
			if (dyn->d_tag == DT_SONAME) {
				soname_offset = dyn->d_un.d_val;
				break;
			}
		}
		if (soname_offset == -1) {
			/* Not found */
			exit(0);
		}
		printf("%s\n", strtab + soname_offset);
	}

	exit(0);
	return 0;

	
}
