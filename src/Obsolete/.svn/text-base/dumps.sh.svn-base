#!/bin/bash

# Copyright (C) 2004, 2005 Valery Reznic
# This file is part of the Elf Statifier project
# 
# This project is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License.
# See LICENSE file in the doc directory.

[ $# -ne 3 -o "x$1" = "x" -o "x$2" = "x" -o "x$3" = "x" ] && {
	echo "Usage: $0 <maps_file> <dir_for_dumps> <gdb_dump_commands_file>" 1>&2
	exit 1
}

Maps=$1
DumpsDir=$2
Output=$3

FileNumber=1 
while :; do
	read StartAddr EndAddr Permission Offset FileName || break
	printf                                                      \
		"my_dump %s/%.6d.dmp %s %s %s\n"                    \
		$DumpsDir $FileNumber $StartAddr $EndAddr $FileName \
	|| exit
	FileNumber=$[FileNumber + 1]
done < $Maps > $Output || exit
exit 0 
