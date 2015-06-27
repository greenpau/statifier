#!/bin/bash

# Copyright (C) 2004, 2005 Valery Reznic
# This file is part of the Elf Statifier project
# 
# This project is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License.
# See LICENSE file in the doc directory.

# It's main script

function PrepareDirectoryStructure
{
	mkdir -p $WORK_COMMON_DIR  || return
	mkdir -p $WORK_GDB_CMD_DIR || return
	mkdir -p $WORK_GDB_OUT_DIR || return
	mkdir -p $WORK_DUMPS_DIR   || return
	mkdir -p $WORK_OUT_DIR     || return
	return 0
}

function Sanity
{
	local Func=Sanity
	[ $# -ne 1 -o "x$1" = "x" ] && {
		Echo "$0: Usage $Func <OrigExe>"
		return 1
	}

	local OrigExe=$1
	[ -f $OrigExe ] || {
   		Echo "$0: '$OrigExe' not exsist or not regular file."
   		return 1
	}

	[ -x $OrigExe ] || {
   		Echo "$0: '$OrigExe' have not executable permission."
   		return 1
	}

	[ -r $OrigExe ] || {
   		Echo "$0: '$OrigExe ' have not read permission."
   		return 1
	}
	return 0
}

function Main
{
	set -e
		source $OPTION_SRC || return
	set +e
	Sanity $opt_orig_exe || return

	local ElfClass
	ElfClass=`$D/elf_class $opt_orig_exe` || return

	D=$D/$ElfClass
	[ -d $D ] || {
		Echo "$0: ElfClass '$ElfClass' do not supported on this system."
		return 1
	}

	local SH_VERBOSE=${opt_verbose:+/bin/bash -x}
	# Do it
	$SH_VERBOSE $D/statifier_common.sh         $WORK_DIR $ElfClass || return
	$SH_VERBOSE $D/statifier_loader.sh         $WORK_DIR           || return
	$SH_VERBOSE $D/statifier_dump.sh           $WORK_DIR           || return
	$SH_VERBOSE $D/statifier_create_starter.sh $WORK_DIR           || return
	$SH_VERBOSE $D/statifier_create_exe.sh     $WORK_DIR           || return
	return 0
}

#################### Main Part ###################################

# Where Look For Other Programs
D=`dirname $0`                 || exit
source $D/statifier_lib.src    || exit
source $D/statifier_parser.src || exit

CommandLineParsing "$@"        || exit
[ "x$NEED_EXIT" = "x" ] || exit $NEED_EXIT

# Temporary Work Directory
if [ "x$opt_keep_working_directory" = "x" ]; then
	WORK_DIR="${TMPDIR:-/tmp}/statifier.tmpdir.$$"
else
	WORK_DIR="./.statifier"
fi

SetVariables $WORK_DIR         || exit 
rm -rf $WORK_DIR               || exit
PrepareDirectoryStructure      || exit
SaveOptions > $OPTION_SRC      || exit

${opt_verbose:+set -x}
Main # Do not check status here, only save it
st=$? 
[ "x$opt_keep_working_directory" = "x" ] && {
	rm -rf $WORK_DIR || exit
}
exit $st

