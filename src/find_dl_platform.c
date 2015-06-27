/*
 * Copyright (C) 2004, 2005 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

static char *my_pattern;
#include <link.h>
static int init_my_pattern(int argc, char *argv[], char *envp[])
{
	char **env;
	ElfW(auxv_t)* auxv;
	for (env = envp; *env; env++);
	env++;
	for (auxv = (ElfW(auxv_t)*)env; auxv->a_type != AT_NULL; auxv++) {
		if (auxv->a_type == AT_PLATFORM) {
			my_pattern = (char *)auxv->a_un.a_val;
			return 0;
		}	
	}
	return 1;
}
#include "./find_dl_main.inc.c"
