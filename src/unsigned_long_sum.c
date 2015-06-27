/*
 * Copyright (C) 2010 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/*
 * This program convert argv[1] ... argv[argc-1] to the 'unsigned long'
 * find sum of all of them and print it to the stout.
 * Sum may (actuall must) wrap around MAX_ULONG if need
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int do_work(int argc, char *argv[], const char *name)
{
	int index;
	char *endptr;
	char *start;
	unsigned long sum = 0L;
	unsigned long num;

	for (index = 1; index < argc; index++) {
		start = argv[index]; 
		if (*start == (char)0) {
			fprintf(
				stderr, 
				"%s: argument %d is empty.\n",
				name, index
			);
			return 1;
		}

		errno = 0;
		num = strtoul(start, &endptr, 0);
		if (errno != 0) { /* Something wrong */
			fprintf(
				stderr, 
				"%s: can't convert '%s'. Errno=%d (%s)\n",
				name, start, errno, strerror(errno)
			);
			return 1;
		}	

		if (*endptr != (char)0) {
			fprintf(
				stderr, 
				"%s: there is illegal symbols in '%s'.\n",
				name, start
			);
			return 1;
		}	
		sum += num;
	}	

	printf("0x%lx\n", sum);
	return 0;
}

int main(int argc, char *argv[])
{
	const char *name = argv[0];
	int stat;
	if (argc < 1) {
		fprintf(
			stderr, 
			"Usage: %s <hex_num1> [<hex_num_2>...]\n", 
			name
		);
		exit(1);
	}
	stat =  do_work(argc, argv, name);
        exit(stat);	
}
