#!/bin/sh

# Copyright (C) 2004-2007 Valery Reznic
# This file is part of the Elf Statifier project
# 
# This project is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License.
# See LICENSE file in the doc directory.

# This script try to find place for starter and print
# starter's entry point to the stdout

[ $# -ne 2 -o "x$1" = "x" -o "x$2" = "x" ] && {
	echo "Usage: $0 <map_file> <starter>" 1>&2
	exit 1
}

MapFile=$1
Starter=$2
D=`dirname $0` || exit

# Get from the MapFile peers "FileName StartAddr" 
FileAddr="`awk '
	BEGIN {
		ind = 0;
	}
	{
		Start = $1
		Name = $5
		if (Name == "") {
			# No file here
			next;
		}
		if (Name ~ ".*]") {
			# dummies, like [vdso], [stack], [heap]
			next;
		}
		if ( (Name in Names)) {
			# I already meet this Name
			next;
		}
		Names[Name] = ""
		Output[ind++] = Name " " Start
	}
	END {
		for (i = 0; i < ind; i++) print Output[i];	
	}
' < $MapFile
`" || exit

set -- $FileAddr

[ $# -eq 0 ] && {
	echo "$0: nothing found in the '$MapFile'" 1>&2
	exit 1
}

while [ $# -ne 0 ]; do
	File=$1;  shift || exit
	Start=$1; shift || exit
	Entry=`$D/fep $File $Start $Starter` || exit
	# Found ?
	[ "x$Entry" = "x" ] || {
		# Yes !
		echo $Entry || exit
		exit 0
	}
done || exit

[ "x$Entry" = "x" ] && {
	echo "$0: Can't find room for the starter" 1>&2
	exit 1
}

exit 0
