/*
 * Copyright (C) 2004, 2005 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/*
 * readelf substitution.
 * print out different information from elf file
 * in more convinience to scripts format 
 */

#include "./my_lib.inc.c"

static void usage()
{
	fprintf(
		stderr,
		"Usage: %s [OPTION]... FILE.\n",
		pgm_name
	);
}

static int b_flag = 0;
static int e_flag = 0;
static int i_flag = 0;
static int s_flag = 0;
static int t_flag = 0;
static int w_flag = 0;

#define hex_format "0x%lx"
#define str_format "%s"

static const char *b_string = "";
static const char *e_string = "";
static const char *i_string = "";
static const char *s_string = "";
static const char *t_string = "";
static const char *w_string = "";

static int need_phdrs = 0;
static int need_shdrs = 0;

static char *filename = NULL;

static void parse(int argc, char *const argv[])
{
	const char *optstring = "hbB:eE:iI:sS:tT:wW:";
	int res;
	int option_found = 0;
	while (1) {
		res = getopt(argc, argv, optstring);
		if (res == -1) break;
		option_found = 1;
		switch (res) {
			case 'h':
				usage();
			exit(0);

			case 'B': /* base address */
				b_string = optarg;
			case 'b':
				b_flag = 1;
				need_phdrs = 1;
			break;

			case 'E': /* entry_point */
				e_string = optarg;
			case 'e':
				e_flag = 1;
			break;

			case 'I': /* interpreter */
				i_string = optarg;
			case 'i':
				i_flag = 1;
				need_phdrs = 1;
			break;

			case 'S': /* read-write segment's size */ 
				s_string = optarg;
			case 's':
				s_flag = 1;
				need_phdrs = 1;
			break;

			case 'W': /* read-write segment's address */ 
				w_string = optarg;
			case 'w':
				w_flag = 1;
				need_phdrs = 1;
			break;

			case 'T': /* has symbol table ? */ 
				t_string = optarg;
			case 't':
				t_flag = 1;
				need_shdrs = 1;
			break;
		}
	}

	if (option_found == 0) {
		fprintf(
			stderr,
			"%s: no options specified.\n",
			pgm_name
		);
		exit(1);
	}
	switch (argc - optind) {
		case 0:
			fprintf(
				stderr,
				"%s: no file argument specified.\n",
				pgm_name
			);
		exit(1);

		case 1: /* ok, should be exactly one */
			filename = argv[optind];
		break;

		default:
			fprintf(
				stderr,
				"%s: too much arguments specified.\n",
				pgm_name
			);
		exit(1);
	}
}	

static ElfW(Ehdr) ehdr;
static ElfW(Phdr) *phdrs;
static ElfW(Shdr) *shdrs;
static int ind_first_load = -1;
static int ind_rw = -1;
static char *file_has_symbol_table = "no";
static char *interpreter = NULL;

static void b_func()
{
	/*
	 * Elf spec say, that PT_LOAD segment's should be sorted
	 * by v_addr. Hope, real file follow elf spec
	 */
	int ind;
	for (ind = 0; ind < ehdr.e_phnum; ind++) {
		if (phdrs[ind].p_type == PT_LOAD) {
			ind_first_load = ind;
			return;
		}
	}

	fprintf(
		stderr,
		"%s: No PT_LOAD segment found in the '%s'.\n",
		pgm_name,
		filename
	);	
	exit(1);
}

static void rw_func()
{
	/*
	 * Really, I hope there is only one writable PT_LOAD segment
	 * in the loader.
	 * But still, if I have two, I make following guess:
	 * segment with RWE permission (if any) got Write permission
	 * occasionly and I select another one.
	 * If both of them hav or don't have execute permission
	 * I'll give error.
	 */

	int rw_num = 0;
	int rw_array[2]; /* No more then 2 writable segments in the loader */
	int ind;
	int ind_rw_1, ind_rw_2;

	for (ind = 0; ind < ehdr.e_phnum; ind++) {
		if (phdrs[ind].p_type == PT_LOAD) {
			if (phdrs[ind].p_flags & PF_W) {
				if (rw_num < 2) rw_array[rw_num] = ind;
				rw_num++;
			}
		}
	}
	switch (rw_num) {
		case 0:
			fprintf(
				stderr,
				"%s: no writable PT_LOAD segments found in the '%s'.\n",
				pgm_name,
				filename
			);
		exit(1);


		default: /* > 2 */
			fprintf(
				stderr,
				"%s: Found '%d', supported not more than 2 writable PT_LOAD segments in the '%s'.\n",
				pgm_name,
				rw_num,
				filename
			);
		exit(1);

		case 2:
			ind_rw_1 = rw_array[0];
			ind_rw_2 = rw_array[1];
			if ( /* First has PF_X, second - not */
				(phdrs[ind_rw_1].p_flags & PF_X) &&
				((phdrs[ind_rw_2].p_flags & PF_X) == 0)
			) {
				ind_rw = ind_rw_2;
				break;
			}
			if ( /* Second has PF_X, first - not */
				(phdrs[ind_rw_2].p_flags & PF_X) &&
				((phdrs[ind_rw_1].p_flags & PF_X) == 0)
			) {
				ind_rw = ind_rw_2;
				break;
			}
			/* Both have or have not exe permission */
			fprintf(
				stderr,
				"%s: can't select from two writable PT_LOAD segments in the '%s'.\n",
				pgm_name,
				filename
			);
		exit(1);

		case 1: /* ok, found and excactly 1 */
			ind_rw = rw_array[0];
		break;
	}
}

static void i_func()
{
	int ind;
	int ind_interp = -1;
	for (ind = 0; ind < ehdr.e_phnum; ind++) {
		if (phdrs[ind].p_type == PT_INTERP) {
			if (ind_interp != -1) {
				fprintf(
					stderr,
					"%s: more than one PT_INTERP segment in the '%s'.\n",
					pgm_name,
					filename
				);
				exit(1);
			}
			ind_interp = ind;
		}
	}
	if (ind_interp == -1) {
		fprintf(
			stderr,
			"%s: no PT_INTERP segment in the '%s'.\n",
			pgm_name,
			filename
		);
		exit(1);
	}
	interpreter = (char *)my_fread_from_position(
			filename,
			phdrs[ind_interp].p_offset,
			phdrs[ind_interp].p_filesz,
			"interpreter"
	);
	if (interpreter == NULL) exit(1);
}

static void t_func()
{
	int ind;
	for (ind = 0; ind < ehdr.e_shnum; ind++) {
		if (shdrs[ind].sh_type == SHT_SYMTAB) {
			/* File was not stripped */
			file_has_symbol_table = "yes";
			return;
		}
	}
}

int main(int argc, char *argv[])
{
	pgm_name = argv[0];	
	parse(argc, argv);

	/* Get ehdr + phdrs + shdrs */
	if ( 
		get_ehdr_phdrs_and_shdrs(
			filename, 
			&ehdr,
			need_phdrs ? &phdrs : NULL,
			NULL,
			need_shdrs ? &shdrs : NULL,
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
			pgm_name,
			filename	
		);
		exit(1);
	}

	if (b_flag) b_func();
	/* e_flag - nothing to do - anyway I got ehdr */
	if (i_flag) i_func();
	if (s_flag || w_flag) rw_func();
	if (t_flag) t_func();

	if (b_flag) {
		printf("%s0x%lx\n", b_string, (unsigned long)phdrs[ind_first_load].p_vaddr);
	}

	if (e_flag) {
		printf("%s0x%lx\n", e_string, (unsigned long)ehdr.e_entry);
	}

	if (i_flag) {
		printf("%s%s\n"   , i_string, interpreter);
	}

	if (s_flag) {
		printf("%s0x%lx\n", s_string, (unsigned long)phdrs[ind_rw].p_memsz);
	}

	if (w_flag) {
		printf("%s0x%lx\n", w_string, (unsigned long)phdrs[ind_rw].p_vaddr);
	}

	if (t_flag) {
		printf("%s%s\n"   , t_string, file_has_symbol_table);
	}
 	exit(0);
	return 0;
}
