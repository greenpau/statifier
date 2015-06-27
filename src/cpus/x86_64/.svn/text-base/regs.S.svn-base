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

	GET_DATA_ADDR %rax
					# Let restore registers now
	mov 	R15(%rax),	%r15
	mov 	R14(%rax),	%r14
	mov 	R13(%rax),	%r13
	mov 	R12(%rax),	%r12
	mov 	RBP(%rax),	%rbp	
	mov 	RBX(%rax),	%rbx	
	mov 	R11(%rax),	%r11
	mov 	R10(%rax),	%r10
	mov 	R9(%rax),	%r9
	mov 	R8(%rax),	%r8
	mov 	RCX(%rax),	%rcx	
	mov 	RDX(%rax),	%rdx	
	mov 	RSI(%rax),	%rsi	
	mov 	RDI(%rax),	%rdi	
					# %rax should be restored last
	#mov 	FS_BASE(%rax),	%fs_base # no idea what is fs_base
	#mov 	GS_BASE(%rax),	%gs_base # no idea what is gs_base
	/*
	 * It's looks like i shouldn't restore segments registers:
	 * when running statified executable on the same system it's ok,
	 * but if it moved to another system sometimes statified exe
	 * segfault here.
	 * As long as i can see on x86_64 segment registers,
	 * i.e DS, ES, FS, GS are cleared by kernel, and not changed
	 * by loader (at least till _dl_start_user)
	 * So let us left them untouched.
	 */
	#if 0
	mov 	DS(%rax),	%ds	
	mov 	ES(%rax),	%es	
	mov 	FS(%rax),	%fs	
	
	# With RH9.0 (i.e with TLS) I got segfault here
	mov 	GS(%rax),	%gs	
	#endif
					# orig_rax - what is it ?
	push 	RIP(%rax)		# It is impossible put value to the %rip
					# by the normal way (movl), so
					# I have to use push/ret

					# Because we somehow get here I hope
					# %cs is ok and do not restore it.

	push 	EFLAGS(%rax)		# save it on the stack
					# before return to %rip I will 'popf' it
					# I think flags here pretty useless,
					# but I want be on the safe side...

					# %rsp and %ss - stack pointer and stack
					# segment - our stack already set
					# somehow, leave it as is

	mov 	RAX(%rax),	%rax	# Only register left to resore 
	popf 				# restore processors flags

	ret				# make two things:
					# - restore %rsp to the value which it
					#   has on the entry to the program
					# - transfer control to the 
					#   run-time loader shared library
					#   initialization.
					# And (I hope) do not change too mach
					# eflags
