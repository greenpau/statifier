# Copyright (C) 2004, 2005 Valery Reznic
# This file is part of the Elf Statifier project
# 
# This project is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License.
# See LICENSE file in the doc directory.

TOP_DIR := ..

SOURCES =       \
   Makefile     \
   $(MAN_1_SRC) \
	
MAN_1 =        \
   statifier.1 \

MAN_1_SRC := $(addsuffix .in,$(MAN_1))

MAN_DIR_DEFAULT := /usr/share/man
MAN_DIR_RPM     := $(shell rpm --eval %_mandir 2>/dev/null || echo $(MAN_DIR_DEFAULT))

MAN_DIR := $(DESTDIR)/$(MAN_DIR_RPM)


MAN_1_DIR := $(MAN_DIR)/man1

all:

all-local: $(MAN_1)

$(MAN_1): %: %.in $(TOP_DIR)/VERSION
	$(RM) $@
	sed -e "s#@VERSION@#$(VERSION)#g" < $< > $@ || { $(RM) $@; exit 1; }

install-local:
	$(MKDIR)               $(MAN_1_DIR)
	$(INSTALL_RO) $(MAN_1) $(MAN_1_DIR)

clean-local:
	$(RM) $(MAN_1)

include $(TOP_DIR)/Makefile.common
