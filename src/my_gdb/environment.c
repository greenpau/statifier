/*
 * Copyright (C) 2010 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

#include "environment.h"
#include "my_gdb.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

void update_environment()
{
	char *value;
	int res;
	char buffer[1000];
	size_t num_var;
	size_t count;
	const char *set_env   = "set environment ";
	const char *unset_env = "unset environment ";
	size_t     set_env_len;
	size_t     unset_env_len;

	set_env_len   = strlen(set_env);
	unset_env_len = strlen(unset_env);

	value = getenv("opt_loader_num_var");
	if (value == NULL) return; /* no set/unset environment */
	num_var = strtoul(value, NULL, 0);
	for (count = 1; count <= num_var; count++) {
		snprintf(
			buffer,
			sizeof(buffer), 
			"opt_loader_var_%lu", 
			(unsigned long)count
		);
		value = getenv(buffer);
		if (value == NULL) {
			fprintf(
				stderr,
				"%s [CHILD]: no env variable '%s' found\n",
				pgm_name,
				buffer
			);
			exit(1);
		}
		if (memcmp(value, set_env, set_env_len) == 0) {
			res = putenv(value + set_env_len);
			if (res) {
				fprintf(
					stderr,
					"%s [CHILD]: can't set env variable '%s' - '%s' (errno=%d)\n",
					pgm_name,
					value + set_env_len,
					strerror(errno),
					errno
				);
				exit(1);
			}
			continue;
		}
		if (memcmp(value, unset_env, unset_env_len) == 0) {
			res = unsetenv(value + unset_env_len);
			if (res) {
				fprintf(
					stderr,
					"%s [CHILD]: can't unset env variable '%s' - '%s' (errno=%d)\n",
					pgm_name,
					value + unset_env_len,
					strerror(errno),
					errno
				);
				exit(1);
			}
			continue;
			
		}
		fprintf(
			stderr,
			"%s [CHILD]: unknown form of env variable: '%s'\n",
			pgm_name,
			value
		);
		exit(1);
	}
}
