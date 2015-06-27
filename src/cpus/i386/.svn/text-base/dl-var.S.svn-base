/*
 * Copyright (C) 2004, 2005, 2010 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/* This code is processor depended !!! */

/* Adjust some _dl variables. */

#include <dl-var.inc>

	nop
	jmp		after_func
	#include <dl-var.s>
after_func:

	GET_DATA_ADDR	%eax
	push		%esp	/* second argument */
	push		%eax    /* first argument  */
	call		do_work
	pop		%eax
	pop		%eax
