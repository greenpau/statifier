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
MY_DATA ENTRY_NUMBER
MY_DATA BASE
MY_DATA LIMIT
MY_DATA FLAGS

	GET_DATA_ADDR	t0

	ldq	v0,	SYSCALL_NUM(t0)
	ldq	a0,	ENTRY_NUMBER(t0)
	ldq	a1,	BASE(t0)
	ldq	a2,	LIMIT(t0)
	ldq	a3,	FLAGS(t0)
	callsys
