/*
 * Copyright (C) 2004 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/* 
 * This program copy Ehdr from the source file to destination.
 * Destination file updated inplace via mmap
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <link.h>

int main(int argc, char *argv[])
{
	char *pgm_name = argv[0];
	char *ehdr_name;
	char *exe_name;
	int  fd;
	ElfW(Ehdr) *ehdr_ehdr;
	ElfW(Ehdr) *exe_ehdr;
	size_t page_size;

	if (argc != 3) {
		fprintf(
			stderr,
			"Usage: %s <ehdr> <exe>\n",
			pgm_name
		);
		exit(1);
	}

	ehdr_name = argv[1];
	exe_name = argv[2];
	page_size = getpagesize();

	fd = open(ehdr_name, O_RDONLY);
	if (fd == -1) {
		fprintf(
			stderr,
			"%s: can't open '%s'. Errno = %d (%s)\n",
			pgm_name, ehdr_name, errno, strerror(errno)
		);
		exit(1);
	}

	ehdr_ehdr = mmap(0, page_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (ehdr_ehdr == MAP_FAILED) {
		fprintf(
			stderr,
			"%s: can't mmap '%s'. Errno = %d (%s)\n",
			pgm_name, ehdr_name, errno, strerror(errno)
		);
		exit(1);
	}

	if (close(fd) == -1) {
		fprintf(
			stderr,
			"%s: can't close '%s'. Errno = %d (%s)\n",
			pgm_name, ehdr_name, errno, strerror(errno)
		);
		exit(1);
	}

	fd = open(exe_name, O_RDWR);
	if (fd == -1) {
		fprintf(
			stderr,
			"%s: can't open '%s'. Errno = %d (%s)\n",
			pgm_name, exe_name, errno, strerror(errno)
		);
		exit(1);
	}

	exe_ehdr = mmap(0, page_size, PROT_WRITE, MAP_SHARED, fd, 0);
	if (exe_ehdr == MAP_FAILED) {
		fprintf(
			stderr,
			"%s: can't mmap '%s'. Errno = %d (%s)\n",
			pgm_name, exe_name, errno, strerror(errno)
		);
		exit(1);
	}

	memcpy(exe_ehdr, ehdr_ehdr, sizeof(ElfW(Ehdr)));
	if (munmap(exe_ehdr, page_size) == -1) {
		fprintf(
			stderr,
			"%s: can't munmap '%s'. Errno = %d (%s)\n",
			pgm_name, exe_name, errno, strerror(errno)
		);
		exit(1);
	}

	if (close(fd) == -1) {
		fprintf(
			stderr,
			"%s: can't close '%s'. Errno = %d (%s)\n",
			pgm_name, exe_name, errno, strerror(errno)
		);
		exit(1);
	}
	exit(0);
	return 0;
}
