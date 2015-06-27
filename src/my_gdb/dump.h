/*
 * Copyright (C) 2010 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

#ifndef dump_h
#define dump_h

#include <sys/types.h>
#include <stddef.h>

void get_memory(pid_t pid, const void *start, const void *stop, char *buffer);
void dumps(pid_t pid, const char *maps_filename, const char *output_dir);
#endif /* dump_h */
