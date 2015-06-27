/*
 * Copyright (C) 2004, 2005 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

#define MY_DATA unsigned long *
struct dl_var_data {
	#include "./dl-var.inc"
};

/*
 * When program invoked stack has following layout:
 * argc        <- stack pointer
 * argv[0]
 * argv[1]
 * ...
 * argv[argc-1]
 * 0
 * env 1
 * env 2
 * ...
 * env last
 * 0
 * auxv[0]
 * auxv[1]
 * ...
 * auxv[n], with auxv[n].a_type = AT_NULL 
 */
#include <elf.h>
#include <link.h> /* for ElfW */
/*
 * Really this function need not to be global - this file will be
 * converted to assembler and included from another assembler file.
 * But gcc have no idea about this and give warning if function declared
 * 'static'
 */
void do_work(struct dl_var_data *data, unsigned long *stack)
{
	unsigned long *argc = stack;
	unsigned long *argv = argc + 1;
	unsigned long *envp = argv + (*argc) + 1;
	unsigned long *temp = envp;
	ElfW(auxv_t) *auxv;
	char *platform;
	unsigned long platform_len = 0;
	/* Find end of environment - it'll be next to the start of auxv data; */
	while (*temp != 0) temp++; 
	auxv = (ElfW(auxv_t)*)(temp + 1); 

	/* 
	 * Alpha for some reason don't like when I change LIBC_STACK_END.
	 * Let's leave it as is.
	 */
	#ifndef  __alpha__
		if (data->LIBC_STACK_END) {
			*(data->LIBC_STACK_END) = (unsigned long)stack;
		}
	#endif
	/* 
	 * Adjust _dl_argc, _dl_argv, _environ and _dl_auxv 
	 * loader's variables
	 */
	*(data->DL_ARGC) = *argc;
	*(data->DL_ARGV) = (unsigned long)argv;
	*(data->ENVIRON) = (unsigned long)envp;
	*(data->DL_AUXV) = (unsigned long)auxv;

	/*
	 * Process auxv vector.
	 * I am looking for AT_BASE and AT_PLATFORM
	 * Exists loaders which have _dl_platform and _dl_platformlen (RH7.2)
	 * and exist loaders without it (RH9.0)
	 * if DL_PLATFORM or DL_PLATFORMLEN is 0, current loader have no
	 * appropriative variable and it will not be set.
	 */ 
	for (; auxv->a_type != AT_NULL; auxv++) {
		switch (auxv->a_type) {
			case AT_BASE:
				/*
				 * When invoked dynamically linked executable
				 * this field contains ral-time loader base 
				 * address.
				 * When invoked statically linked (or statified)
				 * exe this field contains 0.
				 * Let's set it. Just in case
				 *
				 * Note. threre are loader/kernel combination
				 * when loader has fixed load address and
				 * kernel pass AT_BASE equial 0 for 
				 * dynamically linked exe too.
				 * In this case I anyway put to AT_BASE real
				 * load address and hope it'll not hurt.
				 */
				auxv->a_un.a_val = (signed long)data->BASE;
			break;

			case AT_PLATFORM:
				platform = (char *)auxv->a_un.a_val;
				if ( data->DL_PLATFORM != 0) {
					*(data->DL_PLATFORM) = (unsigned long)platform;
				}
				if ( data->DL_PLATFORMLEN != 0) {
					for (; *platform; platform++) {
						platform_len++;
					}
					*(data->DL_PLATFORMLEN) = platform_len;
				}
			break;

			default:
				/* do nothing */
			break;
		}
	}
}
