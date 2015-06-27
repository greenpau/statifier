/*
 * Copyright (C) 2004, 2005 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/* This code is processor depended !!! */

/* Restore registers */

#include "regs.inc"	
	
	/* 
	 * I yet can't find a way to restore ALL registers on alpha.
	 * So I'll sacrify t0 register. It shoudn't hurt.
	 */

	GET_DATA_ADDR	t0
					# Let restore registers now
	ldq	v0,	V0(t0)
	ldq	t1,	T1(t0)
	ldq	t2,	T2(t0)
	ldq	t3,	T3(t0)
	ldq	t4,	T4(t0)
	ldq	t5,	T5(t0)
	ldq	t6,	T6(t0)
	ldq	t7,	T7(t0)
	ldq	s0,	S0(t0)
	ldq	s1,	S1(t0)
	ldq	s2,	S2(t0)
	ldq	s3,	S3(t0)
	ldq	s4,	S4(t0)
	ldq	s5,	S5(t0)
	ldq	fp,	FP(t0)
	ldq	a0,	A0(t0)
	ldq	a1,	A1(t0)
	ldq	a2,	A2(t0)
	ldq	a3,	A3(t0)
	ldq	a4,	A4(t0)
	ldq	a5,	A5(t0)
	ldq	t8,	T8(t0)
	ldq	t9,	T9(t0)
	ldq	t10,	T10(t0)
	ldq	t11,	T11(t0)
	ldq	ra,	RA(t0)
	ldq	t12,	T12(t0)
	ldq	at,	AT(t0)
	ldq	gp,	GP(t0)
	ldq	t0,	PC(t0)

	mb	/* wait for all operation to be finished */

	jmp	(t0)
