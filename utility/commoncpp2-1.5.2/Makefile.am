# Copyright (C) 1999-2005 Open Source Telecom Corporation.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AUTOMAKE_OPTIONS = no-dependencies dist-shar dist-zip
ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = reconfig TODO commoncpp2.spec commoncpp2.spec.in \
	commoncpp2.list.in commoncpp2.lsm.in autoconf/* \
	commoncpp2.list COPYING.addendum INSTALL.w32

MAINTAINERCLEANFILES = configure aclocal.m4 libtool Makefile.in Makefile \
	config/* commoncpp2.list CommonC++.spec INSTALL

DIST_SUBDIRS = src w32 m4 doc demo tests include
if WIN32
SUBDIRS = include src doc
else
if MSWIN32
SUBDIRS = include w32 doc
else
SUBDIRS = include src doc
endif
endif

macosx:
	(cd src ; make macosx)

darwin:
	(cd src ; make macosx)

install-data-local:
	@if test -f /etc/ld.so.conf \
	  -a `echo $(libdir) | grep ^/usr`XXX != "XXX" ; then \
		if ! grep -q "$(libdir)" /etc/ld.so.conf; then \
			echo "------------------------------------------------------------------------------" ;\
			echo "Hmm, looks like you forgot to add $(libdir) to /etc/ld.so.conf." ;\
			echo "I'll try to fix it for you (but pay attention to libtool's output" ;\
			echo "next time, ok?)" ;\
			echo "$(libdir)" >> /etc/ld.so.conf && \
				echo "Ok, all fixed." \
				|| \
				(echo "" ;\
				echo "Well, that didn't work.  Remember to add $(libdir) to" ;\
				echo "/etc/ld.so.conf if you want to use this library.") ;\
			echo "------------------------------------------------------------------------------" ;\
			ldconfig ;\
		fi ;\
	fi


