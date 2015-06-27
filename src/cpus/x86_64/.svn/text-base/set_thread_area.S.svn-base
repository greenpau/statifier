/*
 * Copyright (C) 2004, 2005 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/* This code is processor depended !!! */

/* RH9.0 used TLS - thread local storage. */

MY_DATA SYSCALL_NUM
MY_DATA FUNCTION
MY_DATA ADDRESS

	GET_DATA_ADDR %rax

	mov	ADDRESS(%rax),     %rsi
	mov	FUNCTION(%rax),    %rdi
	mov 	SYSCALL_NUM(%rax), %rax
	syscall
