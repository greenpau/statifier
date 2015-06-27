#!/bin/sh

# Copyright (C) 2004, 2005 Valery Reznic
# This file is part of the Elf Statifier project
# 
# This project is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License.
# See LICENSE file in the doc directory.

[ $# -ne 1 -o "x$1" = "x" ] && {
	echo "Usage: $0 <input_file>" 1>&2
	exit 1
}
Input=$1
OutputDir=`dirname $Input` || exit

awk -vOutputDir="$OutputDir" '
	BEGIN {
		Output = "";
	}
	{
		if ($1 == "STATIFIER_FILE_SEPARATOR") {
			Output = OutputDir "/" $2;
			next;
		}

		if ($1 == "STATIFIER_FILE_SEPARATOR_END") {
			Output = ""
			next;
		}

		if (Output != "") {
			print $0 >>Output;
		}
	}
' < $Input || exit
exit 0
