/*
 * Copyright (C) 2005 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/* 
 * define TLS_SYSCALL to i386's thread local storage syscall - set_thread_area
 */
#ifdef __NR_set_thread_area
	#define TLS_SYSCALL  __NR_set_thread_area
	#define TLS_SYSCALL_NAME "set_thread_area" 
#endif
