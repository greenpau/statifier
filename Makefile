# Copyright (C) 2004, 2005, 2010 Valery Reznic
# This file is part of the Elf Statifier project
# 
# This project is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License.
# See LICENSE file in the doc directory.

SUBDIRS  = src man rpm 

SOURCES =           \
   configure        \
   Makefile         \
   Makefile.common  \
   Makefile.include \
   Makefile.top     \
   VERSION          \
   RELEASE          \
   $(DOCS)          \
   $(CONFIGS)       \

DOCS =       \
   AUTHORS   \
   ChangeLog \
   FAQ       \
   INSTALL   \
   LICENSE   \
   NEWS      \
   README    \
   THANKS    \
   TODO      \
   $(addprefix doc/,$(DOC_DOCS))

DOC_1_5_0_DOCS :=            \
   StatifiedLayout.alpha.txt \
   StatifiedLayout.x86.txt   \
   StatifiedLayout.txt       \
   StarterLayout.txt         \

DOC_1_5_0_DOCS :=  $(addprefix 1.5.0/, $(DOC_1_5_0_DOCS))

DOC_1_6_15_DOCS =            \
   DataFlow.txt              \
   Implementation.txt        \
   Porting.txt               \

DOC_1_6_15_DOCS :=  $(addprefix 1.6.15/, $(DOC_1_6_15_DOCS))

DOC_DOCS =                                \
   README                                 \
   Background.txt                         \
   DataFlow.txt                           \
   Implementation.txt                     \
   MoreDetails.txt                        \
   MoreProblems.txt                       \
   Porting.txt                            \
   StatifiedLayout.txt                    \
   $(DOC_1_5_0_DOCS)                      \
   $(DOC_1_6_15_DOCS)                     \


CONFIGS = $(addprefix configs/config.,$(SUPPORTED_CPU_LIST))

all: config

dist-list-for-tar: config

# It is simpler always re-make config and do not check dependencies.
# Configure care not change config's timestamp if content was not changed
.PHONY: config
config: configure
	/bin/sh ./configure

TOP_DIR := .
include Makefile.top
