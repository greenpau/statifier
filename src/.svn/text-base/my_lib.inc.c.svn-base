/*
 * Copyright (C) 2004, 2005 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/* This file provide some usefull functions */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <elf.h>
#include <link.h> /* I need it for define ElfW() */

static const char *pgm_name;	/* name of the program. 
				   to be used in error messages */

/* "Library" functions */
static FILE *my_fopen(const char *path, const char *mode)
{
	FILE *file;
	file = fopen(path, mode);
	if (file == NULL) {
		fprintf(
			stderr,
			"%s: Can't open '%s' file. Errno = %d (%s)\n",
		        pgm_name, path, errno, strerror(errno)
		);	
		return NULL;
	}
	return file;
}

static int my_fclose(FILE *stream, const char *path)
{
	int result;
	result = fclose(stream);
	if (result != 0) {
		fprintf(
			stderr,
			"%s: Can't close '%s' file. Errno = %d (%s)\n",
		        pgm_name, path, errno, strerror(errno)
		);	
		return -1;
	}
	return 0;
}

static size_t my_fread(
		void *      ptr, 
		size_t      nmemb, 
		FILE *      file, 
		const char *item,
		const char *file_name
)
{
	size_t result;
	result = fread(ptr, 1, nmemb, file);
	if (result != nmemb) {
		fprintf(
			stderr, 
			"%s: can't read '%s' from file '%s'. Errno=%d, (%s).\n",
			pgm_name, item, file_name, errno, strerror(errno)
		);
		return -1;
	}	
	return result;
}

/*static*/ size_t my_fwrite(
		void *      ptr, 
		size_t      nmemb, 
		FILE *      file, 
		const char *item,
		const char *file_name
)
{
	size_t result;
	result = fwrite(ptr, 1, nmemb, file);
	if (result != nmemb) {
		fprintf(
			stderr, 
			"%s: can't write '%s' from file '%s'. Errno=%d, (%s).\n",
			pgm_name, item, file_name, errno, strerror(errno)
		);
		return -1;
	}	
	return result;
}

static int my_fseek(FILE *file, long offset, const char *file_name)
{
	int result;
	result = fseek(file, offset, SEEK_SET);
	if (result == -1) {
		fprintf(
			stderr,
			"%s: Can't fseek in the file '%s' to the pos=%ld. Errno = %d (%s)\n",
			pgm_name, file_name, offset, errno, strerror(errno)
		);
		return -1;
	}
	return result;
}

void *my_malloc(size_t size, const char *item)
{
	void *result;
	result = malloc(size);
	if (result == NULL) {
		fprintf(
			stderr,
			"%s: Can't malloc %lu byte for '%s'.\n",
			pgm_name, (unsigned long)size, item
		);
		return NULL;
	}
	return result;
}

/*static*/ int get_ehdr_phdrs_and_shdrs(
	const char *path,
	ElfW(Ehdr) *ehdr,
	ElfW(Phdr) **phdrs,
	size_t     *phdrs_size,
	ElfW(Shdr) **shdrs,
	size_t     *shdrs_size
)
{
	FILE *file;
	size_t result;
	size_t my_phdrs_size, my_shdrs_size;

	file = my_fopen(path, "r");
	if (file == NULL) return 0;

	result = my_fread(ehdr, sizeof(*ehdr), file, "ehdr", path);
	if (result == -1) return 0;

	if ( ehdr->e_phentsize == 0) {
		fprintf(
			stderr,
			"%s: in the file '%s' e_phentsize == 0\n",
			pgm_name, path
		);
		return 0;
	}

	if ( ehdr->e_phnum == 0) {
		fprintf(
			stderr,
			"%s: in the file '%s' e_phnum == 0\n",
			pgm_name, path
		);
		return 0;
	}
	my_phdrs_size = ehdr->e_phentsize * ehdr->e_phnum;
	if (phdrs != NULL) {
		*phdrs = my_malloc(my_phdrs_size, "phdrs");
		if (*phdrs == NULL) return 0;

		if (my_fseek(file, ehdr->e_phoff, path) == -1) return 0;
		if (my_fread(*phdrs, my_phdrs_size, file, "phdrs", path) == -1) return 0;
	}
	if (phdrs_size != NULL) *phdrs_size = my_phdrs_size;

	my_shdrs_size = ehdr->e_shentsize * ehdr->e_shnum;
	if (shdrs != NULL) {
		*shdrs = my_malloc(my_shdrs_size, "shdrs");
		if (*shdrs == NULL) return 0;

		if (my_fseek(file, ehdr->e_shoff, path) == -1) return 0;
		if (my_fread(*shdrs, my_shdrs_size, file, "shdrs", path) == -1) return 0;
	}
	if (shdrs_size != NULL) *shdrs_size = my_shdrs_size;

	if (my_fclose(file, path) == -1) return 0;
	return 1;
}

/*static*/ off_t my_file_size(const char *path, int *err)
{
	struct stat buf;
	int result;
	result = stat(path, &buf);
	if (result == -1) {
		*err = -1;
		fprintf(
			stderr,
			"%s: can't fstat file '%s'. Errno = %d (%s).\n",
			pgm_name, path, errno, strerror(errno)
		);
		return -1;
	}
	*err = 0;
	return buf.st_size;
}

/*static*/ unsigned char *my_fread_from_position(
		const char *path,
		long        offset, 
		size_t      size,
		const char *item
)
{
	FILE *input;
	int result;
	unsigned char *data;

	data = my_malloc(size, item);
	if (data == NULL) exit(1);

	input = my_fopen(path, "r");
	if (input == NULL) exit(1);

	result = my_fseek(input, offset, path);
	if (result == -1) exit(1);

	result = my_fread(data, size, input, item, path);
	if (result == -1) exit(1);

	result = my_fclose(input, path);
	if (result == -1) exit(1);

	return data;
}
/*static*/ unsigned char *my_fread_whole_file(
		const char *path,
		const char *item,
		off_t      *size
)
{
	int err;
	*size = my_file_size(path, &err);
	if (err == -1) exit(1);
	return my_fread_from_position(path, 0, *size, item);
}
