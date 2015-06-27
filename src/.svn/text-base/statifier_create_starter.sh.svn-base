#!/bin/bash

# Copyright (C) 2004, 2005, 2010 Valery Reznic
# This file is part of the Elf Statifier project
# 
# This project is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License.
# See LICENSE file in the doc directory.

# Create starter

function CreateStarter
{
	[ $# -ne 1 -o "x$1" = "x" ] && {
		Echo "$0: Usage: CreateStarter <Starter>"
		return 1
	}

	set -e
		source $COMMON_SRC || return
		source $LOADER_SRC || return
		source $MISC_SRC   || return
	set +e
	local Starter="$1"

	local REGS=$D/regs
	local REGS_BIN=$WORK_OUT_DIR/regs
	local DL_VAR=$D/dl-var
	local DL_VAR_BIN=$WORK_OUT_DIR/dl-var
	local STA=$D/set_thread_area
	local STA_BIN=$WORK_OUT_DIR/set_thread_area
	local TLS_LIST=

	[ -f $WORK_GDB_OUT_DIR/set_thread_area ] && {
		# Create binary file with set_thread_area parameters
		$D/set_thread_area.sh $WORK_GDB_OUT_DIR/set_thread_area $STA_BIN || return
		TLS_LIST="$STA $STA_BIN"
	}

	# Create binary file with dl-var variables
	rm -f $DL_VAR_BIN || return
	full_dl_list=`
		$D/unsigned_long_sum $val_interpreter_file_base_addr $val_offset&&
		for i in $val_dl_list; do
			case "$i" in
				0 | 0x0)
					echo "0x0"
				;;
				*)
					$D/unsigned_long_sum $i $val_offset || exit
				;;
			esac
		done
	` || return
	$D/strtoul $full_dl_list > $DL_VAR_BIN || return
	# Create binary file with registers' values
	$D/regs.sh $REGISTERS_FILE $REGS_BIN || return
	cat $DL_VAR $DL_VAR_BIN $TLS_LIST $REGS $REGS_BIN > $Starter || return
	return 0 
}

function Main
{
	CreateStarter $WORK_OUT_DIR/starter || return
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
exit 
