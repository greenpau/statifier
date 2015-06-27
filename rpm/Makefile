# Copyright (C) 2004, 2005 Valery Reznic
# This file is part of the Elf Statifier project
# 
# This project is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License.
# See LICENSE file in the doc directory.

TOP_DIR = ..
include $(TOP_DIR)/config

PACKAGE := statifier

SOURCES =                  \
   $(BUILD_FILES)          \
   $(PACKAGE).spec.in      \
   $(PACKAGE_FILES)        \

BUILD_FILES =    \
   Makefile      \
   files.make.sh \
   spec.make.sh  \

PACKAGE_FILES =               \
   $(PACKAGE).files.other.in  \
   $(PACKAGE).files.common.in \
   $(PACKAGE).files.elf.in    \

EXTRA_DIST = $(PACKAGE).spec

all: 

# Dependecies should be in the 'script invoce order'
$(PACKAGE).files: files.make.sh $(PACKAGE_FILES)
$(PACKAGE).spec: spec.make.sh $(PACKAGE).spec.in $(PACKAGE).files $(TOP_DIR)/VERSION $(TOP_DIR)/RELEASE $(TOP_DIR)/ChangeLog

$(PACKAGE).files $(PACKAGE).spec:
	$(RM) $@
	/bin/sh $^ > $@ || { $(RM) $@; exit 1; }

clean-local:
	$(RM) $(EXTRA_DIST) $(PACKAGE).files

include $(TOP_DIR)/Makefile.common
