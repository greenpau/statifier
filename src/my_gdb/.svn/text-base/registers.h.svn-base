/*
 * Copyright (C) 2010 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

#ifndef registers_h
#define registers_h

#include <sys/types.h>
#include <sys/user.h>
#include <stdio.h>

struct user_regs_struct get_registers(pid_t pid);
unsigned long get_register(pid_t pid, size_t reg_offset);
void          set_register(pid_t pid, unsigned long reg, size_t reg_offset);
void dump_regs(FILE *file, struct user_regs_struct *user, int hit_breakpoint);
unsigned long get_pc(pid_t pid);
unsigned long get_sp(pid_t pid);
void set_pc(pid_t pid, unsigned long reg);
void set_sp(pid_t pid, unsigned long reg);

#endif  /* registers_h */
