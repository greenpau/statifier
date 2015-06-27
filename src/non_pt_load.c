/*
 * Copyright (C) 2004, 2005, 2008 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/*
 * This program create "non-load part" of the output executable
 * It contains following:
 *    - changed elf header
 *    - all non-allocated sections from original exe
 *    - changed shdrs from original exe
 *    - phdrs for all PT_LOAD segments 
 *    - filler to the alignment boundary. Alignment is alignment for the
 *      first orig exe's PT_LOAD segment
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

/* Different common variables */
static ElfW(Ehdr) ehdr_exe;         /* Ehdr for original exe */
static ElfW(Ehdr) ehdr_out;         /* Ehdr for oitput   exe */

static ElfW(Phdr) *phdrs_exe;       /* Phdrs for original exe */
static ElfW(Phdr) *phdrs_pt_load;   /* PT_LOAD Phdrs */
static ElfW(Phdr) *phdrs_out;       /* Phdrs for output exe */
static ElfW(Shdr) *shdrs_exe;       /* Shdrs for original exe */
static ElfW(Shdr) *shdrs_out;       /* Shdrs for original exe */

static size_t phdrs_size_exe;      /* size of phdrs for original exe */
static size_t shdrs_size_exe;      /* size of shdrs for original exe */
static size_t shdrs_size_out;      /* size of shdrs for output   exe */

static const char *exe_in;
static const char *phdrs_name;
static const char *e_entry;
static off_t phdrs_pt_load_size;
static int res;
static int pt_load_num;
static int non_pt_load_num = 0;
static off_t pt_load_offset;
static off_t non_alloc_sections_size;
static off_t end_of_non_alloc_sections;
static off_t start_of_non_alloc_sections;
static unsigned long align = 0;
static unsigned long fill_size;
static ElfW(Off) sh_offset;    /* Current section offset */
static int *map_pt_load;
/* End of variables */

int main(int argc, char *argv[])
{
	ElfW(Phdr) *pe;
	ElfW(Phdr) *pl;
	ElfW(Shdr) *se;
	ElfW(Shdr) *so;
	size_t page_size;

	pgm_name = argv[0];	
	if (argc != 4) {
		fprintf(
			stderr, 
			"Usage: %s <orig_exe> <phdrs> <e_entry>\n",
		       	pgm_name
		);
		exit(1);
	}

	exe_in     = argv[1];
	phdrs_name = argv[2];
	e_entry    = argv[3];

	page_size  = getpagesize();
	

	/* Get ehdr, phdrs and shdrs from original exe */
	if ( 
		get_ehdr_phdrs_and_shdrs(
			exe_in, 
			&ehdr_exe,
			&phdrs_exe,
			&phdrs_size_exe,
			&shdrs_exe,
			&shdrs_size_exe
		) == 0
	) exit(1);

	/* Find executable's PT_LOAD segment align */
	{
		int i;
		for (i = 0; i < ehdr_exe.e_phnum; i++) {
			if (phdrs_exe[i].p_type == PT_LOAD) {
				align = phdrs_exe[i].p_align;
				if (align > page_size) align = page_size;
				break;
			}
		}
		if (align == 0) {
			fprintf(
				stderr,
				"%s: can't find PT_LOAD segment align for '%s'\n",
				pgm_name, exe_in
			);
			exit(1);
		}	
	}

	/* Find number of non pt_load segments in the exe */
	{
		int i = 0;
		non_pt_load_num = 0;
		for (i = 0; i < ehdr_exe.e_phnum; i++) {
			switch(phdrs_exe[i].p_type) {
				case PT_LOAD:
				break;

				case PT_INTERP: 
					/* no one need it in the statified exe*/
				break;

				case PT_PHDR:
					/* Elf spec say:
					 * PT_PHDR 
					 * The array element, if present,
					 * specifies the location and size of
					 * the program header table itself,
					 * both in the file and in the memory
					 * image of the program.
					 *
					 * Kernel (linux-2.4.18 and many (all ?)
					 * other use 
					 * ehdr.e_phoff and ehdr.e_phnum 
					 * to locate the Phdrs.
					 * So, I need no PT_PHDR segment
					 */
				break;

				default:
					non_pt_load_num++;
				break;
			}
		}
	}
	/* Read file with PT_LOAD segments */
	phdrs_pt_load = (ElfW(Phdr)*)my_fread_whole_file(
			phdrs_name, 
			"PT_LOAD phdrs", 
			&phdrs_pt_load_size
	);

	/* Sanity */
	if (phdrs_pt_load_size == 0) {
		fprintf(
			stderr,
			"%s: size of the file '%s' is 0.\n",
			pgm_name, phdrs_name
		);
		exit(1);
	}

	if (phdrs_pt_load_size % sizeof(ElfW(Phdr)) != 0) {
		fprintf(
			stderr,
			"%s: size=%ld of the file '%s' is not multiple of phdr size=%lu\n",
			pgm_name,
			phdrs_pt_load_size,
			phdrs_name,
			(unsigned long)sizeof(ElfW(Phdr))
		);
		exit(1);
	}

	/* Calculate number of PT_LOAD segments */ 
	pt_load_num = phdrs_pt_load_size / sizeof(ElfW(Phdr));

	/* Fill in output elf header */
	memcpy(&ehdr_out, &ehdr_exe, sizeof(ehdr_out)); /* copy from orig */
	/* If input exe was build as PIC executable convert type to
	 * regular exe */
	if (ehdr_exe.e_type == ET_DYN) ehdr_out.e_type = ET_EXEC;

	/* e_entry we got as parameter from the command line */
	ehdr_out.e_entry = my_strtoul(e_entry);

	/* Let us put Programs headers just after ehdr */
	ehdr_out.e_phoff = sizeof(ehdr_out);

	/* Phdrs num */
	ehdr_out.e_phnum = pt_load_num + non_pt_load_num;

	/* Sections header just after phdrs */
	ehdr_out.e_shoff = 
		ehdr_out.e_phoff + 
		sizeof(ElfW(Phdr)) * ehdr_out.e_phnum
	;
	/* Sections number from original exe  - do nothing */

	/* Calculate start of the nonallocated sections in the output file */  
	start_of_non_alloc_sections = 
		ehdr_out.e_shoff                      +
       		sizeof(ElfW(Shdr)) * ehdr_out.e_shnum
	;

	/* Duplicate shdrs_size_exe to shdrs_size_out */ 
	shdrs_size_out = shdrs_size_exe;

	/* Duplicate shdrs_exe to shdrs_out */ 
	shdrs_out = my_malloc(shdrs_size_out, "shdrs output size");
	if (shdrs_out == NULL) exit(1);
	memcpy(shdrs_out, shdrs_exe, shdrs_size_exe);

	sh_offset = start_of_non_alloc_sections; /* here first non-alloc section
						  * will be placed
						  */
	/* Adjust/calculate non-alloc sections offsets */
	{
		int i;
		for (i = 0; i < ehdr_exe.e_shnum; i++) {
			/* Ignore allocated sections */
			if ( (shdrs_out[i].sh_flags & SHF_ALLOC) != 0) continue;

			/* Do nothing for NULL section */
			if (shdrs_out[i].sh_type == SHT_NULL) continue;

			shdrs_out[i].sh_offset = sh_offset;
			sh_offset += shdrs_out[i].sh_size;
		}
	}

	end_of_non_alloc_sections = sh_offset;
	non_alloc_sections_size = end_of_non_alloc_sections - start_of_non_alloc_sections;

	/* 
	 * Calculate begin of the PT_LOAD segments in the file:
	 * it should be on the align boundary.
	 */
	{
		unsigned long rest;
		rest = end_of_non_alloc_sections % align;
		pt_load_offset = end_of_non_alloc_sections;
		if (rest) pt_load_offset += (align - rest);
	}

	fill_size = pt_load_offset - end_of_non_alloc_sections;

	/* Update p_offset field for the pt_load segmnets */
	{
		int i;
		phdrs_pt_load[0].p_offset = pt_load_offset;
		for (i = 1; i < pt_load_num; i++) {
			phdrs_pt_load[i].p_offset = 
				phdrs_pt_load[i-1].p_offset + 
				phdrs_pt_load[i-1].p_filesz
			;
		}
	}

	/* 
	 * Map PT_LOAD segments from exe to the segments in pt_load file.
	 * To map it I use v_addr field.
	 * Note: It will not work for PIC executable
	 */
	map_pt_load = my_malloc(ehdr_exe.e_phnum * sizeof(int), "map_pt_load");
	if (map_pt_load == NULL) exit(1);
	{
		int i, j;
		for (i = 0, pe = phdrs_exe; i < ehdr_exe.e_phnum; i++, pe++) {
			unsigned long min_align = 1;
			if (pe->p_type != PT_LOAD) continue;
			for (
				j = 0, pl = phdrs_pt_load; 
				j < pt_load_num; 
				j++, pl++
			) {
				min_align = (pe->p_align < page_size) 
					? pe->p_align
					: page_size
				;
				if ((pe->p_vaddr & ~(min_align - 1)) == pl->p_vaddr) {
					/* Find it ! */
					map_pt_load[i] = j;
					break;
				}
			}
			if (j == pt_load_num) {
				fprintf(
					stderr,
					"%s: warning: can't find segment with v_addr=0x%lx\n",
					pgm_name, 
					(unsigned long)(pe->p_vaddr & ~(min_align - 1))
				);
				map_pt_load[i] = -1;
			}
		}
	}

	/* Update offset for allocated sections */
	{
		int i;
		int j;
		for (
			i = 0, se = shdrs_exe, so = shdrs_out; 
			i < ehdr_exe.e_shnum; 
			i++, se++, so++
		) {
			unsigned long min_align;
			/* Ignore non-allocated sections */
			if ( (so->sh_flags & SHF_ALLOC) == 0) continue;
			/* 
			 * Find to which PT_LOAD segment current section 
			 * is belong 
			 */
			for (
				j = 0, pe = phdrs_exe; 
				j < ehdr_exe.e_phnum; 
				j++, pe++
			) {
				/* Ignore non PT_LOAD segments */
				if (pe->p_type != PT_LOAD) continue;
				min_align = (pe->p_align < page_size) 
					? pe->p_align
					: page_size
				;
				if (
					(se->sh_offset >= pe->p_offset) && 
					(se->sh_offset < (pe->p_offset + pe->p_filesz) ) ) {
					/* Section belong to this segment ! */
					if (map_pt_load[j] == -1) break;
					pl = &phdrs_pt_load[map_pt_load[j]];
					so->sh_offset = 
						pl->p_offset +
						se->sh_offset - pe->p_offset +
						(pe->p_vaddr % min_align)
					;
				}
			}
		}
	}

	/* Update offset for non PT_LOAD segments */
	{
		int i, j;
		ElfW(Phdr) *pe1;
		for (i = 0, pe = phdrs_exe; i < ehdr_exe.e_phnum; i++, pe++) {
			/*
			 * Segments with filesz == 0 are somehow special
			 * (like PT_STACK segment)
			 * For segment like this nothing should be changed
			 */
			if (pe->p_filesz == 0) continue;

			switch (pe->p_type) {
				case PT_LOAD:
				continue;

				case PT_INTERP:
				continue;

				case PT_PHDR:
				continue;

				default:
				break;
			}

			/* Deal with rest of segments here */
			for (
				j = 0, pe1 = phdrs_exe; 
				j < ehdr_exe.e_phnum; 
				j++, pe1++
			) {
				unsigned long min_align;

				if (pe1->p_type != PT_LOAD) continue;

				if (
					(pe->p_offset >= pe1->p_offset) && 
					(pe->p_offset < (pe1->p_offset + pe1->p_filesz))
				) {
					/* This segment is part of PT_LOAD */
					if (map_pt_load[j] == -1) break;
					pl = &phdrs_pt_load[map_pt_load[j]];
					min_align = (pe1->p_align < page_size) 
						? pe1->p_align
						: page_size
					;
					pe->p_offset = 
						pl->p_offset +
						pe->p_offset - pe1->p_offset +
						(pe1->p_vaddr % min_align)
					;
					break;
				}
			}

			if (j < ehdr_exe.e_phnum) break; /* Segmen part of PT_LOAD */ 
			/* This segment is not part of PT_LOAD */
			/* Is it such thing exist ? */
			for (
				j = 0, se = shdrs_exe; 
				j < ehdr_exe.e_shnum; 
				j++, se++
			) {
				if (pe->p_offset == se->sh_offset) {
					pe->p_offset = shdrs_out[j].sh_offset;
					break;
				}
			}
		}
	}

	/* Combine pt_load and non pt_load segments */
	phdrs_out = my_malloc(ehdr_out.e_phnum * sizeof(ElfW(Phdr)), "all phdrs");
	if (phdrs_out == NULL) exit(1);

	{
		int i, i_out;
		int first_pt_load = 1;
		for (i = 0, i_out = 0; i < ehdr_exe.e_phnum; i++) {
			switch (phdrs_exe[i].p_type) {
				case PT_INTERP:
					/* Do nothing */
				break;

				case PT_PHDR:
					/* Do nothing */
				break;

				case PT_LOAD:
					if (first_pt_load == 0) break;
					first_pt_load = 0;
					memcpy(
						&phdrs_out[i_out], 
						phdrs_pt_load,
						phdrs_pt_load_size
					);
					i_out += pt_load_num;
				break;

				default:
					memcpy(
						&phdrs_out[i_out],
						&phdrs_exe[i],
						sizeof(ElfW(Phdr))
					);
					i_out++;
				break;
			}
		}
	}

	/* Ok, everything prepared. Let us write output file */
	/* Ehdr */
	res = my_fwrite(&ehdr_out, sizeof(ehdr_out), stdout, "ehdr", "stdout");
	if (res == -1) exit(1);

	/* Phdrs */
	res = my_fwrite(phdrs_out, ehdr_out.e_phnum * sizeof(ElfW(Phdr)), stdout, "phdrs", "stdout");
	if (res == -1) exit(1);

	/* Shdrs */
	if (ehdr_out.e_shnum != 0) {
		res = my_fwrite(shdrs_out, ehdr_out.e_shnum * sizeof(ElfW(Shdr)), stdout, "shdrs", "stdout");
		if (res == -1) exit(1);
	}

	/* Copy Non-allocated sections */
	if (non_alloc_sections_size > 0) {
		/* copy non-allocated sections from orig exe.*/
		int i;
		unsigned char *buf;
		for (i = 0; i < ehdr_out.e_shnum; i++) {
			/* Ignore allocated section */
			if ((shdrs_out[i].sh_flags & SHF_ALLOC) != 0) continue;
			/* Ignore section with size == 0 */
			if (shdrs_out[i].sh_size == 0) continue;

			/* Copy section */
			buf = my_fread_from_position(
				exe_in,
				shdrs_exe[i].sh_offset,
				shdrs_out[i].sh_size,
				"Section"
			);
			res = my_fwrite(
				buf, 
				shdrs_out[i].sh_size, 
				stdout, 
				"section", 
				"stdout"
			);
			if (res == -1) exit(1);
		}
	}

	if (fill_size > 0) {
		char *fill;
		fill = my_malloc(fill_size, "filler");
		if (fill == NULL) exit(1);
		/* Clean it */
		memset(fill, 0, fill_size);
		res = my_fwrite(fill, fill_size, stdout, "filler", "stdout");
	}
 	exit(0);
}
