/*
 * Copyright (C) 2005 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/*
 * This program is print out symbol table from the file
 * like 'readelf --syms' do
 *
 * Notes:
 *   Other part of statifier which searching symbols in the symbol table
 *   not interesting in the symbols' version information. 
 *   Additionally, code for getting this information is looks like a bit
 *   of problem to me.
 *   So "production" version of the program don't try to get version 
 *   information, but it may be changed if defined 'USE_VERSION'
 *   When USE_VERSION is defined outpt of this program should be 
 *   exactly same as 'readelf --syms -W' (readelf version 2.15.91.0.2 20040727)
 *   readelf's -W flag prevent it from stripping symbols' line 
 *   to 80-byte lengths.
 */

/* Uncomment this if you want version information in the symbol table */
/* #define USE_VERSION */

#include "./my_lib.inc.c"

static const char *get_st_bind(unsigned char val) 
{
	static const char *const binds[] = {
		"LOCAL",
		"GLOBAL",
		"WEAK",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"10",
		"11",
		"12",
		"13",
		"14",
		"15"
	};
	return binds[ELF32_ST_BIND(val)];
}

static const char *get_st_type(unsigned char val)
{
	static const char *const types[] = {
		"NOTYPE",
		"OBJECT",
		"FUNC",
		"SECTION",
		"FILE",
		"COMMON",
		"TLS",
		"7",
		"8",
		"9",
		"10",
		"11",
		"12",
		"13",
		"14",
		"15"
	};
	return types[ELF32_ST_TYPE(val)];
}

static const char *get_st_visibility(unsigned char val)
{
	static const char *const visibilities[] = {
		"DEFAULT",
		"INTERNAL",
		"HIDDEN",
		"PROTECTED"
	};
	return visibilities[ELF32_ST_VISIBILITY(val)];
}
static const char *get_st_index(ElfW(Section) val)
{
	static char buffer[6];
	switch(val) {
		case SHN_UNDEF:
		return " UND";

		case SHN_ABS:
		return " ABS";

		default:
			snprintf(buffer, sizeof(buffer), "%4u", (unsigned int)val);
		return buffer;
	}
	/* Can't get here */
}

static ElfW(Ehdr) ehdr;
static ElfW(Phdr) *phdrs;
static ElfW(Shdr) *shdrs;
static const char *file_name;
#ifdef USE_VERSION
	static ElfW(Half) *memory_versym = NULL;
	static char **versions = NULL;
	static ElfW(Word) verdefnum = 0;
#endif /* USE_VERSION */

#ifdef USE_VERSION
void find_version_info()
{
	size_t count;
	size_t dynamic_num = (size_t)-1;
	size_t verdef_num  = (size_t)-1;
	size_t versym_num  = (size_t)-1;
	size_t strings_num;
	char *memory_verdef;
	char *memory_strings;
	ElfW(Dyn) *memory_dynamic;
	ElfW(Addr) verdef = 0;
	ElfW(Addr) versym = 0;
	ElfW(Verdef) *c_verdef;
	ElfW(Verdaux) *c_verdaux;
	
	for (count = 0; count < ehdr.e_shnum; count++) {
		if (shdrs[count].sh_type == SHT_DYNAMIC) {
			dynamic_num = count;
			break; 
		}
	}

	if (dynamic_num == (size_t)-1) return; /* dynamic section not found */ 

	memory_dynamic = (ElfW(Dyn) *) my_fread_from_position(
		file_name,
		shdrs[dynamic_num].sh_offset,
		shdrs[dynamic_num].sh_size,
		"dynamic section"
	);
	
	for (
		count = 0; 
		count < shdrs[dynamic_num].sh_size / sizeof(ElfW(Dyn)); 
		count++
	) {
		switch(memory_dynamic[count].d_tag) {
			case DT_VERDEF:
				verdef = memory_dynamic[count].d_un.d_ptr;
			break;

			case DT_VERDEFNUM:
				verdefnum = memory_dynamic[count].d_un.d_val;
			break;

			case DT_VERSYM:
				versym = memory_dynamic[count].d_un.d_ptr;
			break;

			default:
				/* do nothing */
			break;
		}
	}

	if (verdef    == 0) return;
	if (verdefnum == 0) return;
	if (versym    == 0) return;

	for (count = 0; count < ehdr.e_shnum; count++) {
		if (shdrs[count].sh_addr == verdef) {
			verdef_num = count;
			strings_num = shdrs[count].sh_link;
			break;
		}
	}
	if (verdef_num == (size_t)-1) return;

	for (count = 0; count < ehdr.e_shnum; count++) {
		if (shdrs[count].sh_addr == versym) {
			versym_num = count;
			break;
		}
	}
	if (versym_num == (size_t)-1) return;

	memory_strings = my_fread_from_position(
		file_name,
		shdrs[strings_num].sh_offset,
		shdrs[strings_num].sh_size,
		"strings section"
	);

	memory_verdef = my_fread_from_position(
		file_name,
		shdrs[verdef_num].sh_offset,
		shdrs[verdef_num].sh_size,
		"verdef section"
	);

	memory_versym = (ElfW(Half) *)my_fread_from_position(
		file_name,
		shdrs[versym_num].sh_offset,
		shdrs[versym_num].sh_size,
		"versym section"
	);
	
	
	versions = calloc( (verdefnum + 1), sizeof(char *) );
	for (
		count = 0, c_verdef = (ElfW(Verdef) *)memory_verdef;
		count < verdefnum; 
		count++, 
		c_verdef = (ElfW(Verdef) *)((char *)c_verdef + c_verdef->vd_next)
	) {
		if (c_verdef->vd_cnt) {
			c_verdaux = (ElfW(Verdaux) *) ((char *)c_verdef + c_verdef->vd_aux);
			versions[c_verdef->vd_ndx] = c_verdaux->vda_name + memory_strings;
		}
	}

	versions[1] = NULL;
}

char *get_version_string(size_t index, const char *name)
{
	size_t len;
	ElfW(Half) num;
	char *buffer;
	num = memory_versym[index];
	if ( (num < 2) || (num > verdefnum) ) return strdup("");
	if (versions[num] == NULL) return strdup("");

	if (strlen(name) == 0) return strdup("");
	if (strcmp(versions[num], name) == 0) return strdup("");

	len = strlen(versions[num]);
	if (len == 0) return strdup("");
	buffer = (char *)malloc(len + 2 /* @@ */ + 1 /* 0 */);
	sprintf(buffer, "@@%s", versions[num]);
	return buffer;
}
#endif /* USE_VERSION */

int main(int argc, char *argv[])
{
	int err;
	size_t count;
	size_t current;
	size_t link;
	ElfW(Sym) *syms;
	ElfW(Sym) *sym;
	char *names;
	char *section_names = NULL;

	pgm_name = argv[0];	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file_name>\n", pgm_name);
		exit(1);
	}

	file_name = argv[1];

	err = get_ehdr_phdrs_and_shdrs(
		file_name,
		&ehdr,
		&phdrs,
		NULL,
		&shdrs,
		NULL
	);
	if (err == -1) exit(1);

	#ifdef USE_VERSION
		find_version_info();
	#endif /* USE_VERSION */

	if (ehdr.e_shstrndx != SHN_UNDEF) {
		section_names = (char *)my_fread_from_position(
			file_name,
			shdrs[ehdr.e_shstrndx].sh_offset,
			shdrs[ehdr.e_shstrndx].sh_size,
			"section names"
		);
	}
	for (count = 0; count < ehdr.e_shnum; count++) {
		switch (shdrs[count].sh_type) {
			size_t ent_num;
			case SHT_SYMTAB:
			case SHT_DYNSYM:
				link = shdrs[count].sh_link;
				syms = (ElfW(Sym) *) my_fread_from_position(
					file_name,
					shdrs[count].sh_offset,
					shdrs[count].sh_size,
					"symtab/dynsym section"
				);
				names    = (char *)my_fread_from_position(
					file_name,
					shdrs[link].sh_offset,
					shdrs[link].sh_size,
					"strtab section"
				);
				ent_num = shdrs[count].sh_size / sizeof(ElfW(Sym));
				if (section_names) {
					printf(
						"\nSymbol table '%s' contains %lu entries:\n",
						section_names + shdrs[count].sh_name, 
						(unsigned long)ent_num
					);
				} else {
					printf(
						"\nSymbol table '%lu' contains %ld entries:\n",
						(unsigned long)count, 
						(unsigned long)ent_num
					);
				}
				printf(
					"%6s: %-*s %5s %-7s %-6s %-7s %4s %s\n",
					"Num",
					(int) (sizeof(long) * 2),
					"   Value",
					"Size",
					"Type",
					"Bind",
					"Vis",
					"Ndx",
					"Name"
				);
				for (
					current = 0, sym = syms;
					current < ent_num;
					current++, sym++
				) {
					printf(
						"%6lu: %.*lx %5lu %-7s %-6s %s %s %s%s\n",
						(unsigned long)current,
						(int) (sizeof(long) * 2),
						(unsigned long)sym->st_value,
						(unsigned long)sym->st_size,
						get_st_type(sym->st_info),
						get_st_bind(sym->st_info),
						get_st_visibility(sym->st_other),
						get_st_index(sym->st_shndx),
						names + sym->st_name,
						#ifdef USE_VERSION
						get_version_string(
							(shdrs[count].sh_type == SHT_DYNSYM) ? current : 0,
							names + sym->st_name
						)
						#else
						""
						#endif /* USE_VERSION */
					);
				}
			break;
	
			default:
			break;
		}
	}

	exit(0);
}
