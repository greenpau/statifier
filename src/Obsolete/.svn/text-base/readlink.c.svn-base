/*
  Copyright (C) 2004 Valery Reznic
  This file is part of the Elf Statifier project
  
  This project is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  See LICENSE file in the doc directory.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

int main(int argc, char *argv[])
{
	char buf1[PATH_MAX], buf2[PATH_MAX];
	char *src, *dst, *tmp;
	int res;
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <FileName>\n", argv[0]);
	       exit(1);	
	}
	src = buf1;
	dst = buf2;
	strncpy(src, argv[1], PATH_MAX);
	src[PATH_MAX - 1] = (char)0;
	while (1) {
		memset(dst, 0, PATH_MAX); 
		res = readlink(src, dst, PATH_MAX - 1);
		if (res == -1) {
			if (errno == EINVAL) {
				printf("%s\n", src);
				exit(0);
			} else {
				fprintf(
					stderr, 
					"%s: Can't readlink '%s' %s(%d)\n", 
					argv[0], src, strerror(errno), errno
				);
				exit(1);
			}
		} else {
			if (dst[0] != (char)'/') { /* path is relative */
				char *last_slash;
				last_slash = rindex(src, '/');
				strcpy(last_slash + 1, dst); 
			} else {
				tmp = src;
				src = dst;
				dst = tmp;
			}
		}
	}
	return 1;
}

