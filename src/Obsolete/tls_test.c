/*
 * Copyright (C) 2004, 2005 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/*
 * It's a test program to be run by 'set_thread_area_addr'
 * to find 'set_thread_area' syscall address.
 * Pay attention: i hope this program use SAME interpreter as statified
 * executable.
 */
#include <stdlib.h>

int main(int argc, char *argv[])
{
	exit(0);
}

