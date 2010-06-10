## Makefile.am for DirectFB/systems/rtd

RTD_LIBS = -lketlaer -lpthread -ldl

INCLUDES = \
	-I$(top_srcdir)/include		\
	-I$(top_builddir)/include	\
	-I$(top_builddir)/lib		\
	-I$(top_srcdir)/lib		\
	-I$(top_srcdir)/src		\
	$(VNC_CFLAGS)

systemsdir = $(MODULEDIR)/systems

if BUILD_STATIC
systems_DATA = libdirectfb_rtd.o
endif

systems_LTLIBRARIES = libdirectfb_rtd.la

inputdriversdir = $(MODULEDIR)/inputdrivers

if BUILD_STATIC
inputdrivers_DATA = libdirectfb_rtdinput.o
endif
inputdrivers_LTLIBRARIES = libdirectfb_rtdinput.la

libdirectfb_rtd_la_LDFLAGS = \
	$(RTD_LIBS)	\
	-avoid-version	\
	-module

libdirectfb_rtd_la_SOURCES = \
	primary.c	\
	primary.h	\
	rtd.c		\
	rtd.h		\
	libk.cpp        

libdirectfb_rtd_la_LIBADD = \
	$(top_builddir)/lib/direct/libdirect.la \
	$(top_builddir)/lib/fusion/libfusion.la \
	$(top_builddir)/src/libdirectfb.la

libdirectfb_rtdinput_la_LDFLAGS = \
	$(RTD_LIBS)	\
	-avoid-version	\
	-module

libdirectfb_rtdinput_la_SOURCES = \
	rtdinput.c	\
	libk.cpp

libdirectfb_rtdinput_la_LIBADD = \
	$(top_builddir)/src/libdirectfb.la


include $(top_srcdir)/rules/libobject.make

