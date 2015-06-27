/*
 * Copyright (C) 2010 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

#ifndef my_ptrace_h
#define my_ptrace_h

#include "my_gdb.h"

#include <sys/types.h>
#include <sys/ptrace.h>

long my_ptrace(
	enum __ptrace_request request,
	pid_t                 pid,
	void                 *addr,
	void                 *data,
	const char           *pgm_name, 
	const char           *ptrace_request,
	int                   print_error,
	int                   exit_on_fail
);

#define MY_PTRACE(request, pid, addr, data, print_error, exit_on_fail) \
	my_ptrace(                                                     \
		request,                                               \
		pid,                                                   \
		addr,                                                  \
		data,                                                  \
		pgm_name,                                              \
		#request,                                              \
		print_error,                                           \
		exit_on_fail                                           \
	)                                                              \

#endif /* my_ptrace_h */
