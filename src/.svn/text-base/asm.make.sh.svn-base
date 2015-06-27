#!/bin/sh

# Copyright (C) 2010, 2012 Valery Reznic
# This file is part of the Elf Statifier project
# 
# This project is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License.
# See LICENSE file in the doc directory.

###########################################################################
# For some unknown reason (at least) on amd64 gcc (at least gcc 3.2.2 SuSE )
# generate a huge 'dl-var' file when used 'binary'output format.
# Without output format (i.e. default elf) file is reasonable small.
# The immediate reason is the string in assemble code:
#	.section	.eh_frame,"aw",@progbits
# So, I need to remove it from the generated .s file
###########################################################################
# More trouble with amd64 gcc - 4.4.2 on Fedora 12
# Instead of .eh_frame it uses .cfi_startproc/.cfi_endproc
# With those directives in place gcc generated eh_frame code
# AND output it into resulting executable.
# So, address immediatly after end of code is not my data, but eh_frame data
# instead.
# I want to avoid this, so I have to remove those directives from the
# gernerated .s file
###########################################################################
# gcc version 3.2.3 put in the end of generated assembler code
# .section .note-GNU-stack...,
# i.e to be sure code after included file will go to the text section
# I have to add ".text" to the end.
###########################################################################
# gcc 4.6.0 Fedora-15 i386 all of sudden began to use a lot of .cfi
# directives:
# .cfi_def_cfa_offset
# .cfi_endproc
# .cfi_offset
# .cfi_remember_state
# .cfi_restore
# .cfi_restore_state
# .cfi_startproc
# I have to remove them all
# What interesting that same gcc-4.6.0 Fedora-15 for x86-64 generate only
# .cfi_endproc/.cfi_startproc
###########################################################################
awk '
	{ 
		if (                              \
			($2 !~ "^[.]eh_frame") && \
			($1 !~ "^[.]cfi_")        \
		) {
			# Normal string, print it
			print $0
		} else {
			# Problematic string, comment it out
			print "/*"
			print $0
			print "*/"
		}
	} 
	END {
		print "\t.text"
	}
' || exit
exit 0
