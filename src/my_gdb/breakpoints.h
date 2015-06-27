/*
 * Copyright (C) 2010 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

#ifndef breakpoints_h
#define breakpoints_h

#include <stddef.h>
#include <sys/types.h>

typedef struct {
	unsigned long addr;
	unsigned long aligned_addr;
	long          old_value;
	long          new_value;
	unsigned char active;
} breakpoint_t;

#define MAX_BREAKPOINTS 1000

extern breakpoint_t breakpoints[MAX_BREAKPOINTS];
extern size_t num_breakpoints;

void breakpoint_add(pid_t pid, unsigned long addr);
void breakpoint_do(pid_t pid, size_t idx);
void breakpoint_activate(pid_t pid, size_t idx);
void breakpoint_deactivate(pid_t pid, size_t idx);

#endif /* breakpoints_h */
