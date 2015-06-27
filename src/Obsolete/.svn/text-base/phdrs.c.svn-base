/*
 * Copyright (C) 2004 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/*
 * This program create new load segment for the "pseudo_static" exe
 * Segment contains following:
 *    - changed elf header
 *    - changed phdrs
 *    - starter program
 */

#include "./my_lib.inc.c"

/* Different common variables */
static const char *starter_segment; /* filename for starter segment */
static const char *sections;        /* filename for shdrs and nonalloc sect */
static const char *exe_in;          /* original exe's filename */
static const char *core;            /* gdb's core filename */ 
static const char *starter;         /* starter's filename */

static ElfW(Ehdr) ehdr_exe;         /* Ehdr for original exe */
static ElfW(Ehdr) ehdr_out;         /* Ehdr for oitput   exe */

static ElfW(Phdr) *phdrs_exe;       /* Phdrs for original exe */
static ElfW(Phdr) *phdrs_core;      /* Phdrs for core file */ 

static ElfW(Shdr) *shdrs_exe;       /* Shdrs for original exe */

static int is_stack_under_executable;   /* flag 0/1 which show it */
static int is_starter_under_executable; /* flag 0/1 which show it */ 
static int first_load_segment = 0;      /* first load segment 
					   in the core file */
static int arg_ind;                     /* current command line's 
					   argument index */

static size_t phdrs_size_exe;      /* size of phdrs for original exe */
static size_t phdrs_size_core;     /* size of phdrs for core file    */
static size_t shdrs_size_exe;      /* size of shdrs for original exe */

static off_t  starter_seg_size;    /* size of starter segment */

/* End of variables */

static int preparation(int argc, char *argv[])
{
	const char *s_is_stack_under_executable;   /* as string */
	const char *s_is_starter_under_executable; /* as string */
	const char *s_ignored_segments;            /* as string */

	int ignored_segments;                      /* ignored segments */ 

	size_t num_load_segment_in_core = 0;       /* num of PT_LOAD segment
						      in the core file */
	ElfW(Ehdr) ehdr_core;                      /* Ehdr for core file */ 

	/* Process command line arguments */
	arg_ind                       = 1;
	starter_segment               = argv[arg_ind++];
	sections                      = argv[arg_ind++];
	exe_in                        = argv[arg_ind++];
	core                          = argv[arg_ind++];
	starter                       = argv[arg_ind++];
	s_is_stack_under_executable   = argv[arg_ind++];
	s_is_starter_under_executable = argv[arg_ind++];
	s_ignored_segments            = argv[arg_ind++];

	is_stack_under_executable = atoi(s_is_stack_under_executable);
	if (is_stack_under_executable < 0) {
		fprintf(
			stderr,
			"%s: is_stack_under_executable='%s', should be >= 0\n",
			pgm_name, s_is_stack_under_executable
		);
		exit(1);
	}

	is_starter_under_executable = atoi(s_is_starter_under_executable);
	if (is_starter_under_executable < 0) {
		fprintf(
			stderr,
			"%s: is_starter_under_executable='%s', should be >= 0\n",
			pgm_name, s_is_starter_under_executable
		);
		exit(1);
	}

	ignored_segments = atoi(s_ignored_segments);
	if (ignored_segments <= 0) {
		fprintf(
			stderr,
			"%s: ignored_segment='%s', should be > 0\n",
			pgm_name, s_ignored_segments
		);
		exit(1);
	}

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

	/* Get ehdr, phdrs and shdrs from gdb's core */
	if ( 
		get_ehdr_phdrs_and_shdrs(
			core, 
			&ehdr_core, 
			&phdrs_core,
			&phdrs_size_core,
			NULL,
			NULL
		) == 0
	) exit(1);

	/* How many LOAD segments have we in the core ? */
	/* What's first load segment ? */
	{
		int i;
		for (i = 0; i < ehdr_core.e_phnum; i++) {
			if (phdrs_core[i].p_type == PT_LOAD) {
				if (num_load_segment_in_core == 0) {
					first_load_segment = i;
				}
				num_load_segment_in_core++;
			}
		}
	}

	/* Core file sanity */
	if (num_load_segment_in_core == 0) {
		fprintf(
			stderr,
			"%s: there are no PT_LOAD segments in the core '%s'.\n",
			pgm_name, core
		);
		exit(1);
	}

	/* Command line sanity */

	/*
	 * linux 2.5, 2.6 create one more segment
	 * gdb 6.0 save it in the core file, but gdb 6.1 not
	 * so check
	 * num_load_segment_in_core - ignored_segments) != (argc - arg_ind))
	 * not always correct.
	 * Let's try to use something less strict.
	 */
	if (
		((argc - arg_ind) >  num_load_segment_in_core) ||
		((argc - arg_ind) < (num_load_segment_in_core - ignored_segments))
	) {
		fprintf(
			stderr,
			"%s: mismatch: core file '%s' has %lu LOAD segments but command line supply ignored_segments='%d' and %d files\n",
			pgm_name,
			core,
			(unsigned long)num_load_segment_in_core,
			ignored_segments,
			argc - arg_ind
		);
		exit(1);
	}
	return 0;
}

static int create_starter_segment(int argc, char *argv[])
{
	FILE *input;
	FILE *output;

	ElfW(Phdr) *phdrs_out;  /* Phdrs for output file */
	ElfW(Phdr) *ph_starter; /* Phdr pointer for the starter segment */
	size_t phdrs_size_out;  /* size of phdrs for the output exe */
	size_t num_seg_out;     /* number of segment in the output exe */

	char *starter_segment_memory;   /* Memory to hold whole
					   starter segment */

	off_t  starter_pgm_size;  /* Size of starter program */

	int err;
	int result; /* result for my_fread/my_fwrite */

	/* Copy original ehdr to output ehdr */
	memcpy(&ehdr_out, &ehdr_exe, sizeof(ehdr_out));

	/* Segments number in the output file */
	num_seg_out = 
		  (argc - arg_ind)
		+ 1 /* My segment */
	;
	/* How much place do i need for output phdrs ? */
	phdrs_size_out = num_seg_out * ehdr_out.e_phentsize;

	/* Get place for output phdrs */
	phdrs_out = my_malloc(phdrs_size_out, "phdrs for output exe file");
	if (phdrs_out == NULL) exit(1);

	/* Fill appropriative entries in phdrs_out from the phdrs_core */ 
	{
		int i_core = first_load_segment + (is_stack_under_executable ? 1 : 0);
		int i_out = is_starter_under_executable ? 1 : 0;
		int count;

		ElfW(Phdr) *ph_core = &phdrs_core[i_core];
		ElfW(Phdr) *ph_out  = &phdrs_out [i_out];

		for (count = num_seg_out - 1; count > 0; ph_core++) {
			if (ph_core->p_type == PT_LOAD) {
				ph_out->p_type   = PT_LOAD;
				/* p_offset - to be filled later */
				ph_out->p_vaddr  = ph_core->p_vaddr;
				ph_out->p_paddr  = ph_core->p_vaddr;
				ph_out->p_flags  = ph_core->p_flags;
				/*
				 * I have no information about alignment,
				 * but because it's a dump from the memory
				 * and loader already had align it correctly,
				 * I can not 're-align' it.
				 * So, let say align = 1, i.e no align
				 */ 
				ph_out->p_align = 1; 	
				ph_out++;
				count--;
			}
		}
		/* Fill p_filesz and p_memsz entries in phdrs_out */ 
		ph_out  = &phdrs_out[i_out];
		for (; arg_ind < argc; arg_ind++, ph_out++) {
			/* get file size */
			ph_out->p_filesz = ph_out->p_memsz = 
				my_file_size(argv[arg_ind], &err);
			if (err != 0) exit(1);
		}
	}

	/* Fill data for the starter segment */
	ph_starter = &phdrs_out[is_starter_under_executable ? 0 : (num_seg_out-1) ];
	ph_starter->p_type = PT_LOAD; 

	/* Calculate starter_seg_size */
	{
		ElfW(Word) align = 0; 
		int ind;
		/* Find alignment for the executable code in the exe ehdr */
		for (ind = 0; ind < ehdr_exe.e_phnum; ind++) {
			if (phdrs_exe[ind].p_type == PT_LOAD) {
				if (phdrs_exe[ind].p_flags & PF_X) {
					align = phdrs_exe[ind].p_align;
					break;
				}
			}
		}

		if (ind == ehdr_exe.e_phnum) {
			fprintf(
				stderr,
				"%s: there is no PT_LOAD segment with PF_X flag in the '%s'.\n",
				pgm_name, exe_in
			);
			exit(1);
		}

		starter_pgm_size = my_file_size(starter, &err);
		if (err != 0) exit(1);

		/* Starter seg size is elf header size + size of all phdrs + 
	 	 * + starter program size
	 	 */
		starter_seg_size =
			sizeof(ehdr_out) + 
	       		phdrs_size_out   +
			starter_pgm_size
		;

		/* Now round it up to the align boundary if needed */
		{
			size_t rest;
			rest = starter_seg_size % align;
			if (rest) starter_seg_size += (align - rest);
		}
	}

	ph_starter->p_filesz = starter_seg_size;
	ph_starter->p_memsz  = starter_seg_size;
	ph_starter->p_flags  = PF_X | PF_R; 
	ph_starter->p_align  = 1;

	if (is_starter_under_executable) {
		/* i.e ph_starter == &phdrs_out[0] */
		ph_starter->p_vaddr = phdrs_out[1].p_vaddr - starter_seg_size;
		ph_starter->p_paddr = ph_starter->p_vaddr;
		ph_starter->p_offset = 0;
	} else {
		/* i.e ph_starter is last segment */
		ph_starter->p_vaddr = 
			phdrs_out[num_seg_out - 2].p_vaddr + 
			phdrs_out[num_seg_out - 2].p_memsz
		;
		ph_starter->p_paddr = ph_starter->p_vaddr;
		/* I guess here first exe segment contain also ehdr and phdrs */
		phdrs_out[0].p_offset = 0;
	}

	/* Fill offset field  for all but first segment */
	{
		size_t ind;
		for (ind = 1; ind < num_seg_out; ind++) {
			phdrs_out[ind].p_offset = 
				phdrs_out[ind - 1].p_offset + 
				phdrs_out[ind - 1].p_filesz
			;
		}
	}
	/* Now filling of phdrs_out is finished */

	/* Adjust Ehdr */
	ehdr_out.e_entry = 
		ph_starter->p_vaddr +
		sizeof(ehdr_out)    + 
		phdrs_size_out       
	;
	ehdr_out.e_phoff = ph_starter->p_offset + sizeof(ehdr_out);
	/* Sections header I'll always put AFTER last segment */
	ehdr_out.e_shoff = 
		phdrs_out[num_seg_out - 1].p_offset + 
		phdrs_out[num_seg_out - 1].p_filesz
	;
	ehdr_out.e_phnum = num_seg_out;
	/* Now ehdr_out is ok too */

	/* Allocate space for the starter segment */
	starter_segment_memory = my_malloc(
			starter_seg_size, 
			"starter_segment_memory"
	);
	if (starter_segment_memory == NULL) exit(1);
	memset(starter_segment_memory, 0, starter_seg_size);

	/* Fill starter_segment_memory */
	{
       		char *cur_ptr;
		size_t cur_size;

		/* Copy ehdr */
		cur_ptr  = starter_segment_memory;
		cur_size = sizeof(ehdr_out);
		memcpy(cur_ptr, &ehdr_out, cur_size);
		cur_ptr += cur_size;

		/* Copy phdrs */
		cur_size = phdrs_size_out;
		memcpy(cur_ptr, phdrs_out, cur_size);
		cur_ptr += cur_size;

		/* Open starter program */
		input = my_fopen(starter, "r");
		if (input == NULL) exit(1);

		/* Read starter program in */
		result = my_fread(
				cur_ptr, 
				starter_pgm_size, 
				input, 
				"all file", 
				starter
		);
		if (result == -1) exit(1);

		/* Close it */
		result = my_fclose(input, starter);
		if (result == -1) exit(1);
	} /* Now starter_segment_memory contains starter_segment */

	/* Open starter segment output file */
	output = my_fopen(starter_segment, "w");
	if (output == NULL) exit(1);

	/* Write it */
	result = my_fwrite(
			starter_segment_memory,
			starter_seg_size,
			output,
			"all file",
			starter_segment
	);
	if (result == -1) exit(1);
	
	/* Close it */
	result = my_fclose(output, starter_segment);
	if (result == -1) exit(1);

	return 0;
}

static int create_sections()
{
	/* Adjust shdrs */
	/* Sections which are not allocated, should be copied from
	 * the original executable and appended to the end of 
	 * statified exe and thiir offset should be calculated.
	 * Sections which are allocated already present in the statified
	 * exe (as part of PT_LOAD segments).
	 * If starter is under executable, their offset should be increased
	 * by starter_seg_size.
	 * If starter is above executable nothing should be done for
	 * allocated sections.
	 * Here I assume that ALL not allocated sections after ALL allocated
	 * ones. It's looks like reasonable from the linker/loader
	 * points of view, but to be sure I'll need to do more 
	 * accurate chech here.
	 * NOTE: for now I'll trust allocation flag in the p_flags.
	 *       but may be I'll need to verify section offset and
	 *       PT_LOAD segments offset
	 */
	FILE *input;            /* input File for original exe */
	FILE *output;           /* output file for sections */

	ElfW(Off) sh_offset;    /* Current section offset 
				   for non-allocateid sections */
	ElfW(Shdr) *shdrs_out;  /* Shdrs for output exe */
	ElfW(Shdr) *shdr;       /* Pointer to the current shdr */

	char *section_buf;      /* Buffer to hold non-allocated section */

	int result;             /* result for my_fread/my_fwrite */
	size_t ind;             /* loop index */
	size_t shdrs_size_out;  /* shdrs size for the output file */

	/* Duplicate shdrs_size_exe to shdrs_size_out */ 
	shdrs_size_out = shdrs_size_exe;
	/* Duplicate shdrs_exe to shdrs_out */ 
	shdrs_out = my_malloc(shdrs_size_out, "shdrs output size");
	if (shdrs_out == NULL) exit(1);
	memcpy(shdrs_out, shdrs_exe, shdrs_size_exe);

	sh_offset = ehdr_out.e_shoff + shdrs_size_out; /* here first section
							  to be copied begin */
	/* Adjust/calculate sections offsets */
	for (ind = 0, shdr = shdrs_out; ind < ehdr_exe.e_shnum; ind++, shdr++) {
		if (shdr->sh_flags & SHF_ALLOC) { /* Allocated section */
			if (is_starter_under_executable) { 
				shdr->sh_offset += starter_seg_size;
			}
		} else { /* Not allocated section */
			shdr->sh_offset = sh_offset;
			sh_offset += shdr->sh_size;
		}
	}

	/* Open orig exe */
	input = my_fopen(exe_in, "r");
	if (input == NULL) exit(1);

	/* Open sections for output */
	output = my_fopen(sections, "w");
	if (output == NULL) exit(1);

	/* Write shdrs to output */
	result = my_fwrite(shdrs_out, shdrs_size_out, output, "shdrs", sections);
	if (result == -1) exit(1);

	/* Copy all non-allocated sections from orig exe to sections file */
	for (ind = 0, shdr = shdrs_out; ind < ehdr_exe.e_shnum; ind++, shdr++) {
		if (shdr->sh_flags & SHF_ALLOC) continue;
		if (shdr->sh_size == 0) continue;

		/* Locate section in the input file */
		result = my_fseek(input, shdrs_exe[ind].sh_offset, exe_in);
		if (result == -1) exit(1);

		/* Allocate space for it */
		section_buf = my_malloc(shdr->sh_size, "section buffer");
		if (section_buf == NULL) exit(1);

		/* Read section */
		result = my_fread(section_buf, shdr->sh_size, input, "section", exe_in);
		/* Write section */
		if (result == -1) exit(1);
		result = my_fwrite(section_buf, shdr->sh_size, output, "section", sections);
		if (result == -1) exit(1);

		/* Free buffer */
		free(section_buf);
	}
	/* Close output */
	result = my_fclose(output, sections);
	if (result == -1) exit(1);
 	return 0;
}

int main(int argc, char *argv[])
{
	pgm_name = argv[0];	
	if (argc < 9) {
		fprintf(
			stderr, 
			"Usage: %s <starter_seg> <sections> <exe_in> <gdb_core_file> <starter_program> <is_stack_under_executable> <is_starter_under_executable> <ignored_seg> <seg_file_1> [<seg_file_2>...]\n",
		       	pgm_name
		);
		exit(1);
	}

	preparation(argc, argv);
	create_starter_segment(argc, argv);	
	create_sections();
 	exit(0);
	return 0;
}
