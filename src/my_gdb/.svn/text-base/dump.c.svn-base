/*
 * Copyright (C) 2010 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

#include "dump.h"
#include "my_gdb.h"
#include "my_ptrace.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void get_memory(pid_t pid, const void *start, const void *stop, char *buffer)
{
	size_t num_longs;
	size_t i;
	unsigned long *u_buffer;

	if ( (unsigned long)start % sizeof(long) ) {
		fprintf(
			stderr,
			"%s: start address 0x%lx is not %lu-bytes aligned\n",
			pgm_name,
			(unsigned long)start,
			(unsigned long)sizeof(long)
		);
		exit(1);
	}
	if ( (unsigned long)stop % sizeof(long) ) {
		fprintf(
			stderr,
			"%s: stop address 0x%lx is not %lu-bytes aligned\n",
			pgm_name,
			(unsigned long)stop,
			(unsigned long)sizeof(long)
		);
		exit(1);
	}
	u_buffer = (unsigned long *)buffer;
	num_longs = ((unsigned long)stop - (unsigned long)start) / sizeof(long);
	for (i = 0; i < num_longs; i++) {
		u_buffer[i] = MY_PTRACE(
			PTRACE_PEEKTEXT, 
			pid, 
			((unsigned long *)start) + i,
			NULL,
			1,
			1
		);
	}
}

void dumps(pid_t pid, const char *maps_filename, const char *output_dir)
{
	FILE *f;
	int fd;
	char *buffer;
	char file_name[4096];
	size_t length;
	unsigned long start;
	unsigned long stop;
	int res;
	int ind;
	f = fopen(maps_filename, "r");
	if (f == NULL) {
		fprintf(
			stderr,
			"%s: can't open file '%s' - %s (errno=%d)\n",
			pgm_name,
			maps_filename,
			strerror(errno),
			errno
		);
		exit(1);
	}

	for(ind = 1; ; ind++) {
		res = fscanf(f, "%lx %lx %*[^\n]\n", &start, &stop);
		if (res == EOF) {
			if (ferror(f)) {
				fprintf(
					stderr,
					"%s: error reading from file '%s' - %s (errno=%d)\n",
					pgm_name,
					maps_filename,
					strerror(errno),
					errno
				);
				exit(1);
			} else {
				break;
			}
		}

		if (res != 2) {
			fprintf(
				stderr,
				"%s: wrong line in the '%s' file\n",
				pgm_name,
				maps_filename
			);
			exit(1);
		}

		if (start >= stop) {
			fprintf(
				stderr,
				"%s: start=0x%lx >= stop=0x%lx\n",
				pgm_name,
				start,
				stop
			);	
			exit(1);
		}
		length = stop - start;
		buffer = malloc(length);
		get_memory(pid, (const void *)start, (const void *)stop, buffer);
		snprintf(
			file_name, 
			sizeof(file_name), 
			"%s/%06d.dmp", 
			output_dir, 
			ind
		);
		fd = open(file_name, O_CREAT | O_RDWR, 0644);
		if (fd == -1) {
			fprintf(
				stderr,
				"%s: can't open file '%s' - %s (errno=%d)\n",
				pgm_name,
				file_name,
				strerror(errno),
				errno
			);
			exit(1);
		}
		write(fd, buffer, length);
		close(fd);
		//fprintf(stderr, "0x%lx 0x%lx\n", start, stop);
	}
	fclose(f);
}
