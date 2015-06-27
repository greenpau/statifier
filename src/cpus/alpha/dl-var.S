/*
 * Copyright (C) 2004, 2005 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/* This code is processor depended !!! */

/* Adjust some _dl variables. */

#include <dl-var.inc>

	br	after_func
	#include <dl-var.s>
after_func:

	GET_DATA_ADDR	a0	/* first argument */
	mov	sp,	a1	/* second argument */
	bsr	ra,	do_work
