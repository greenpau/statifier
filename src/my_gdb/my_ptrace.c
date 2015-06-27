/*
 * Copyright (C) 2010 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

#include "my_ptrace.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long my_ptrace(
	enum __ptrace_request request,
	pid_t                 pid,
	void                 *addr,
	void                 *data,
	const char           *pgm_name, 
	const char           *ptrace_request,
	int                   print_error,
	int                   exit_on_fail
)
{
	long result;
	errno = 0;
	result = ptrace(request, pid, addr, data);
	if (errno) {
		if (print_error) {
			int save_errno;
			save_errno = errno;
			fprintf(
				stderr,
				"%s: can't %s - '%s' (errno=%d)\n",
				pgm_name,
				ptrace_request,
				strerror(errno),
				errno
			);
			errno = save_errno;
		}
		if (exit_on_fail) {
			exit(1);
		}
	}
	return result;
}
