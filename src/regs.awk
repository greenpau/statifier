# Copyright (C) 2004, 2005 Valery Reznic
# This file is part of the Elf Statifier project
# 
# This project is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License.
# See LICENSE file in the doc directory.

{
	R[$1] = $2; 
}
END {
	for (ind = 0; ind < reg_max; ind++) {
		reg_name = P[ind];
		reg_value = R[reg_name];
		if (reg_value == "") reg_value = "0x0";
		printf("%s:\t.long %s\n", reg_name, reg_value);
	}
}
