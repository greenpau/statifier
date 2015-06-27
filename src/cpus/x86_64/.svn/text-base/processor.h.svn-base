/*
 * Copyright (C) 2004, 2005, 2010 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/* This file define per-processor things for x86_84 (amd) */

#ifndef processor_h
#define processor_h

	#define REGISTER_SIZE 8
	#define SYSCALL_REG   (ORIG_RAX)
	#define PC_REG        (RIP)
	#define SP_REG        (RSP)
	#define PC_OFFSET_AFTER_SYSCALL 2

	#ifdef __ASSEMBLER__
		.macro GET_DATA_ADDR reg
 			call	addr
		addr:
 			pop	\reg
 			add	$(data - addr),	\reg
		.endm

		#define MY_JUMP jmp
	#endif /* __ASSEMBLER__ */ 
#endif /* processor_h */

