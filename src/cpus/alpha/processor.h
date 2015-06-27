/*
 * Copyright (C) 2004, 2005 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/* This file define per-processor things for alpha */

#ifndef processor_h
#define processor_h

	#define REGISTER_SIZE 8

	#define SYSCALL_REG   (EF_V0)
	#define PC_REG        (EF_PC)
	#define PC_OFFSET_AFTER_SYSCALL 4

	#ifdef __ASSEMBLER__
		/* Definition for registers' softname */
		#include "/usr/include/alpha/regdef.h"
		/*
		 * There is a bug in regdef (or gcc)
		 * AT defined as $at, but $at unknown for assembler.
		 * anyway, I need AT as my label for register's data
		 *  So:
		 */
		#undef AT
		#define at $28

		.set	noat
		.set	noreorder

		.macro GET_DATA_ADDR reg
			bsr	\reg, addr
		addr:
			mb
			lda	\reg,	(data-addr)(\reg)
			mb
		.endm

		#define MY_JUMP br
	#endif /* __ASSEMBLER__ */ 
#endif /* processor_h */
