#!/bin/bash

# Copyright (C) 2004, 2005, 2010 Valery Reznic
# This file is part of the Elf Statifier project
# 
# This project is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License.
# See LICENSE file in the doc directory.

# This script have to detect kernel/loader properties
# 

function Main
{
	set -e
		source $OPTION_SRC || return
	set +e

	local val_interpreter
	val_interpreter=`$D/$elf_class/elf_data -i $opt_orig_exe` || return

	$D/$elf_class/elf_symbols $val_interpreter > $LOADER_SYMBOLS || return

	(
		set -e
		val_interpreter_file_entry=`$D/$elf_class/elf_data -e $val_interpreter`
		echo "val_elf_class=$val_elf_class"
		echo "val_interpreter='$val_interpreter'"
		echo "val_interpreter_file_entry='$val_interpreter_file_entry'"
		$D/$elf_class/elf_data                       \
			-T val_interpreter_has_symtab=       \
			-B val_interpreter_file_base_addr=   \
                   	-W val_interpreter_file_rw_seg_addr= \
			-S val_interpreter_rw_seg_size=      \
		$val_interpreter
	) || return 
	return 0
}

#################### Main Part ###################################

# Where Look For Other Programs
D=`dirname $0`               || exit
source $D/statifier_lib.src  || exit

[ $# -ne 2 -o "x$1" = "x" -o "x$2" = "x" ] && {
	Echo "Usage: $0 <work_dir> <elf_class>"
	exit 1
}

WORK_DIR=$1
val_elf_class=$2

SetVariables $WORK_DIR || exit
Main > $COMMON_SRC     || exit
exit 0
