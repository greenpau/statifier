# Copyright (C) 2004, 2005 Valery Reznic
# This file is part of the Elf Statifier project
# 
# This project is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License.
# See LICENSE file in the doc directory.

# Debuggers command for dumping set_thread_area parameters for alpha
define set_thread_area
	# Here syscall's number
	info register v0
	# Here parameters for syscall
	info register a0
	info register a1
	info register a2
	info register a3
end
