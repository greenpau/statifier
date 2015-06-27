/*
 * Copyright (C) 2005 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/* 
 * define TLS_SYSCALL to x86_64's thread local storage syscall - arch_prctl
 */
#ifdef __NR_arch_prctl
	#define TLS_SYSCALL  __NR_arch_prctl
	#define TLS_SYSCALL_NAME "arch_prctl" 
#endif
