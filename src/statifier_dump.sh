#!/bin/bash

# Copyright (C) 2004-2007, 2010 Valery Reznic
# This file is part of the Elf Statifier project
# 
# This project is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License.
# See LICENSE file in the doc directory.

# Dump all data from the statified process

function DumpRegistersAndMemory
{
	$D/my_gdb                                  \
   		$opt_orig_exe                      \
   		$val_breakpoint_start              \
   		$val_interpreter_file_entry        \
   		$MISC_SRC                          \
   		$WORK_GDB_OUT_DIR/regs_from_kernel \
   		$INIT_MAPS_FILE                    \
   		$REGISTERS_FILE                    \
   		$MAPS_FILE                         \
   		$WORK_GDB_OUT_DIR/set_thread_area  \
   		$WORK_DUMPS_DIR                    \
	|| return
	return 0
}

function Main
{
	set -e
		set -a 
			source $OPTION_SRC || return
		set +a
		source $COMMON_SRC || return
		source $LOADER_SRC || return
	set +e

	DumpRegistersAndMemory || return
	return 0
}

#################### Main Part ###################################

# Where Look For Other Programs
D=`dirname $0`              || exit
source $D/statifier_lib.src || exit

[ $# -ne 1 -o "x$1" = "x" ] && {
	Echo "Usage: $0 <work_dir>"
	exit 1
}

WORK_DIR=$1

SetVariables $WORK_DIR || exit
Main                   || exit
exit 0
