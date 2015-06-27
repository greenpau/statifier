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

	GET_DATA_ADDR	%eax
					# Let restore registers now
	movl	EBX(%eax),	%ebx	
	movl	ECX(%eax),	%ecx	
	movl	EDX(%eax),	%edx	
	movl	ESI(%eax),	%esi	
	movl	EDI(%eax),	%edi	
	movl	EBP(%eax),	%ebp	
					# %eax should be restored last
	/*
	 * It's looks like i shouldn't restore segments registers:
	 * when running statified executable on the same system it's ok,
	 * but if it moved to another system sometimes statified exe
	 * segfault here.
	 * DS and ES. As long as i can see on all x86
	 * (and x86_64 for 32-bit executables) kernel pass to the program
	 * DS = ES = SS.
	 * in the _dl_start_user point these registers are same, i.e 
	 * loader do nothing with it.
	 * FS = 0 and loader not changed it.
	 * GS = 0 and it set by loader after set_thread_area
	 */
	#if 0
	movl	DS(%eax),	%ds	
	movl	ES(%eax),	%es	
	movl	FS(%eax),	%fs	
	
	# With RH9.0 (i.e with TLS I got segfault here)
	movl	GS(%eax),	%gs	
	#endif
					# orig_eax - what is it ?
	pushl	EIP(%eax)		# It is impossible put value to the %eip
					# by the normal way (movl), so
					# I have to use pushl/ret

					# Because we somehow get here I hope
					# %cs is ok and do not restore it.

	pushl	EFLAGS(%eax)		# save it on the stack
					# before return to %eip I will 'popf' it
					# I think flags here pretty useless,
					# but I want be on the safe side...

					# %esp and %ss - stack pointer and stack
					# segment - our stack already set
					# somehow, live it as is

	movl	EAX(%eax),	%eax	# Only register left to resore 
	popfl				# restore processors flags

	ret				# make two things:
					# - restore %esp to the value which it
					#   has on the entry to the program
					# - transfer control to the 
					#   run-time loader shared library
					#   initialization.
					# And (I hope) do not change too mach
					# eflags
