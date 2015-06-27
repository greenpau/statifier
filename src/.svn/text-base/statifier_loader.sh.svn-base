#!/bin/bash

# Copyright (C) 2004, 2005, 2013 Valery Reznic
# This file is part of the Elf Statifier project
# 
# This project is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License.
# See LICENSE file in the doc directory.

# This script have to detect properties/addresses
# which I need in order to dump statified process.
# (now I am looking only for _dl_start_user address)
# and
# different addresses I need to create starter.

function GetSymbol
{
	# This function print out address of 'Symbol' in hex.
	# I am looking for symbol in the output of 
	# readelf --syms $Interpreter.
	# (now readelf replaced with statifier's elf_symbols, but output
	#  is same) 
	# Function work as following:
	#  1. Symbol NOT FOUND.
	#  1.1. If 'IsMandatory'=1, error message will be printed 
	#       and return status 1
	#  1.2. If 'IsMandatory' != 1, i.e Symbol is optional,
	#       value '0x0' will be prined and return status 0.
	# 2. Symbol WAS FOUND.
	#    return status - 0  and it's value printed out
	[ $# -ne 3 -o "x$1" = "x" -o "x$2" = "x" -o "x$3" = "x" ] && {
		Echo "$0: Usage: GetSymbol <Symbol> <IsMandatory> <IsGlobal>"
		return 1
	}
	local Symbol=$1
	local IsMandatory=$2
	local IsGlobal="$3"

	local MsgNotFound="$0: Symbol '$Symbol' not found in the interpreter '$val_interpreter'"
	local MsgNoIdea="$0: internal error: no idea how to find '$Symbol' in the interpreter '$val_interpreter' without symtab."

	local Value=""
	if [ "X$val_interpreter_has_symtab" = "Xyes" -o "X$IsGlobal" = "Xyes" ]; then
		# Interpreter has symtab. or symbol is global.
		# Good. Just try to find symbol in.
		Value=`awk -vSymbol="$Symbol" '{
			if ($NF == Symbol) {
				print "0x" $2; 
				exit 0;
			}
		}' < $LOADER_SYMBOLS` || return
	fi

	if [ "x$Value" = "x" ]; then
		# No symtab in the interpreter.
		# Or may be symbol was not found in symtab.
		# Not so good.
		# I'll need for each symbol run it's autodetect test.
		local PgmName
		case "$Symbol" in
			
			_dl_platformlen)
				Value=""  # No way to find it.
			;;

			_dl_start_user)
				PgmName="$D/elf_find_pattern"
				Value=`                             \
					$PgmName                    \
					$val_interpreter            \
					$prop_pattern_dl_start_user \
				` || return
			;;


			_dl_argc     | \
			_dl_argv     | \
			_dl_auxv     | \
			_dl_platform | \
			_environ) 
				PgmName="$D/find$Symbol"
				local Found=-1
				Value=`                                   \
					$PgmName                          \
					$Found                            \
					$val_interpreter_file_base_addr   \
					$val_interpreter_file_rw_seg_addr \
					$val_interpreter_rw_seg_size      \
				` || return
			;;

			*)
				Echo "$MsgNoIdea"
				return 1
			;;
		esac
	fi

	if [ "x$Value" = "x" ]; then
		# Symbol not found
		if [ "x$IsMandatory" = "x1" ]; then
			Echo "$MsgNotFound"
			return 1 
		else
			Value="0x0"
		fi
	fi

	printf "0x%x" $Value || return
	return 0
}

function ForDump
{
	local val_breakpoint_start

	val_breakpoint_start=`GetSymbol _dl_start_user 1 no` || return
	echo "val_breakpoint_start='$val_breakpoint_start'"  || return
	return 0
}

function ForStarter
{
	local Value 
	local Var
	local val_dl_list=""

	Var="_dl_argc"
	Value=`GetSymbol $Var 1 no` || return
	echo "val_$Var=$Value"      || return
	val_dl_list="$val_dl_list $Value"

	Var="_dl_argv"
	Value=`GetSymbol $Var 1 no` || return
	echo "val_$Var=$Value"      || return
	val_dl_list="$val_dl_list $Value"

	Var="_environ"
	Value=`GetSymbol $Var 1 no` || return
	echo "val_$Var=$Value"      || return
	val_dl_list="$val_dl_list $Value"

	Var="_dl_auxv"
	Value=`GetSymbol $Var 1 no` || return
	echo "val_$Var=$Value"      || return
	val_dl_list="$val_dl_list $Value"

	Var="_dl_platform"
	Value=`GetSymbol $Var 0 no` || return
	echo "val_$Var=$Value"      || return
	val_dl_list="$val_dl_list $Value"

	Var="_dl_platformlen"
	Value=`GetSymbol $Var 0 no` || return
	echo "val_$Var=$Value"      || return
	val_dl_list="$val_dl_list $Value"

	Var="__libc_stack_end"
	Value=`GetSymbol $Var 0 yes` || return
	echo "val_$Var=$Value"       || return
	val_dl_list="$val_dl_list $Value"

	echo "val_dl_list='$val_dl_list'" || return
	return 0
}

function Main
{
	set -e
		source $COMMON_SRC || return
	set +e
	ForDump    || return
	ForStarter || return
	return 0
}
#################### Main Part ###################################

# Where Look For Other Programs
D=`dirname $0`              || exit
source $D/statifier_lib.src || exit
source $D/properties.src    || exit

[ $# -ne 1 -o "x$1" = "x" ] && {
	Echo "Usage: $0 <work_dir>"
	exit 1
}

WORK_DIR=$1

SetVariables $WORK_DIR || exit
Main > $LOADER_SRC     || exit
exit 0
