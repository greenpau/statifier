#!/bin/sh 

# Copyright (C) 2004, 2005 Valery Reznic
# This file is part of the Elf Statifier project
# 
# This project is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License.
# See LICENSE file in the doc directory.

# This script do following:
#   1) Convert output of the gdb's command 'info registers'
#      to the valid asseble file, which fill memory with
#      registers values.
#      Output looks like following:
#      ebx: .long 0x12345678
#      ecx: .long 0x12345678
#    Registers' order - is as described in the struct user_regs_struct
#    from the  /usr/include/sys/user.h
#    Really order is not importent, only importent thing is to save
#    registers' order syncronized with code in the starter.S
#    I think, user_regs_struct is a good reference. 
#
#   2) Convert it to the binary file. Originally I did it with gcc.
#      Now it's done with awk + strtoul 
#
#     If one want check strtoul (or gcc) correctness one can use
#     both of then and compare results :) 

[ $# -ne 2 -o "x$1" = "x" -o "x$2" = "x" ] && {
	echo "Usage: $0 <input_file> <output_file>" 1>&2
	exit 1
}

Input=$1
Output=$2
TmpFile=$Output.S
D=`dirname $0` || exit

rm -f $TmpFile || exit
awk -f $D/regs.awk < $Input > $TmpFile || exit

Regs="`awk '{print $3}' < $TmpFile`" || exit
rm -f $Output || exit
$D/strtoul $Regs > $Output || exit

# For gcc / strtoul cross-check 
#gcc $TmpFile -o $Output.1 -Wl,--oformat,binary,--entry,0x0 -nostdlib || exit

exit 0
