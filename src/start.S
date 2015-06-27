/*
 * Copyright (C) 2004, 2005 Valery Reznic
 * This file is part of the Elf Statifier project
 * 
 * This project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 * See LICENSE file in the doc directory.
 */

/* Include file for all .S sources. */

CURRENT_OFFSET = 0
.macro MY_DATA Name
	\Name = CURRENT_OFFSET
	CURRENT_OFFSET = CURRENT_OFFSET + REGISTER_SIZE
.endm

