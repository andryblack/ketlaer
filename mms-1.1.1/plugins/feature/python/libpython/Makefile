### The directory environment:

MMSDIR = ../../../..

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak
include ../Makefile.common

SRCS = mmspython.cpp mmspythread.cpp pyutil.cpp

CXXFLAGS += -I$(MMSDIR) -I$(MMSDIR)/plugins -I$(MMSDIR)/plugins/input -I$(MMSDIR)/plugins/output \
	    -I$(MMSDIR)/plugins/feature -I$(PYTHON_INCLUDE_PATH) -fPIC -shared

CXXFLAGS += $(PYTHON_CFLAGS)

all:	libpython deps

libpython:	$(OBJS)
		$(LIBTOOL) --mode=link $(LIBTOOL_OPTS) $(CXX) $(CXXFLAGS) -o libpython.o $(OBJS)

deps:
	make -C mmsv2
	make -C mmsv2gui

clean:
	$(DO_MAKECLEAN)
	@if test -f libpython.o ; then rm *.o ; fi
	@rm -f *~
