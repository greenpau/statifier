/*
 * Copyright (C) 2004, 2005, 2008 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/*
 * This file try find place for starter in the specified file.
 * On success starter's entry point is printed to stdout.
 */

#include "./my_lib.inc.c"
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


int main(int argc, char *argv[])
{
	const char *elf_name;
	const char *s_start_addr;
	const char *starter_name;
	off_t starter_size;
	int err;
	int i;
	int first = 1;
	unsigned long start_addr;
	unsigned long rest;
	ElfW(Ehdr) ehdr;         /* Ehdr */
	ElfW(Phdr) *phdrs;       /* Phdrs */
	size_t page_size;

	pgm_name = argv[0];	
	if (argc != 4) {
		fprintf(
			stderr, 
			"Usage: %s <elf_file> <start_addr> <starter>\n",
		       	pgm_name
		);
		exit(1);
	}

	elf_name     = argv[1];
	s_start_addr = argv[2];
	starter_name = argv[3];

	start_addr = my_strtoul(s_start_addr);

	/* Get ehdr, phdrs and shdrs from elf_file */
	if ( 
		get_ehdr_phdrs_and_shdrs(
			elf_name,
			&ehdr,
			&phdrs,
			NULL,
			NULL,
			NULL
		) == 0
	) exit(1);

	/* Get starter's size */
	starter_size = my_file_size(starter_name, &err);
	if (err == -1) exit(1);

	page_size = getpagesize();

	/* Try to find segment, which has enought room to host starter */
	for (i = 0; i < ehdr.e_phnum; i++) {
		unsigned long total_space;
		unsigned long used_space;
		unsigned long unused_space;
		unsigned long file_start_addr = 0;
		unsigned long e_entry;
		unsigned long align;

		/* Look for PT_LOAD segment */
		if (phdrs[i].p_type != PT_LOAD) continue;

		if (first) {
			/* Save v_addr for the first PT_LOAD segment */
			first = 0;
			file_start_addr = phdrs[i].p_vaddr;
		}
		/* Look for segment with PF_X permissions */
		if ( (phdrs[i].p_flags & PF_X) == 0) continue;

		/* Sanity */
		/* If memsz and filesz are different I don't like it.
		 * It's looks like data segment occassionly has 
		 * execute permission. I don't touch it - 
		 * my code may destroy .bss data
		 */
		if (phdrs[i].p_filesz != phdrs[i].p_memsz) continue;

		/* Have many space used in this segment ? */ 
		used_space = phdrs[i].p_memsz; /* 
						* ok, usually for exec seg
						* p_memsz == p_filesz.
						* But I want to be on the 
						* safe side...
						*/ 
		/* 
	 	 * I want starter aligned on the 16 boundary,
	 	 * For some (perfomance ? ) reason so do kernel with a stack
		 * One more reason alpha (at least ) need proper instruction
		 * alignment
	 	 * So, let us round up used_space
	  	 */ 
		rest = used_space % 16;
		if (rest) used_space += (16 - rest);

		/* Segment's total space */
		/*
		 * If p_align != getpagesize() kernel/loader (alpha, amd64)
		 * will do funny things:
		 * - non-used pages may be marked us non-accessable
		 *   (permission ---) or unmmapped at all.
		 * So, in order to find room for starter I use not
		 * p_align, but getpagesize().
		 * Ok, really I use mimimum of both of them.
		 * I can't see how p_align may be < then getpagesize(),
		 * just in case...
		 */
		align = (phdrs[i].p_align < page_size) ? phdrs[i].p_align : page_size;

		total_space = used_space;
		rest = used_space % align;
		if (rest) total_space += (align - rest);

		/* How many unused space left here ? */
		unused_space = total_space - used_space;

		/* Have we got enougth unused space here ? */ 
		if (unused_space < starter_size) continue;

		/* Ok, got it ! */
		 e_entry = 
			 /* base addr */
			 start_addr + 
			 /* Offset to the needed segment */
			 (phdrs[i].p_vaddr - file_start_addr) + 
			 /* offset in the segment to starter */
			 used_space
		;

		printf("0x%lx\n", e_entry);
		break;
	}	
 	exit(0);
}
