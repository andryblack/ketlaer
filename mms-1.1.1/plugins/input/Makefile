### The directory environment:

MMSDIR = ../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

all: input

SUBDIRS = keyboard

SUBDIRS += replay

ifeq ($(LIRC),yes)
SUBDIRS += lirc
endif

ifeq ($(EVDEV),yes)
SUBDIRS += evdev
endif

SUBDIRS += rtd

SRCS = input.cpp input_device.cpp remote.cpp remote_config.cpp
OBJS = input.o input_device.o remote.o remote_config.o

CXXFLAGS += -I../ -I../../ -I../output/ -Ilirc/

input-base: $(OBJS) $(DEP)
	$(LIBTOOL) --mode=link $(LIBTOOL_OPTS) $(CXX) $(CXXFLAGS) -o input-base.o $(OBJS)

input: $(OBJS) $(DEP)
	$(DO_MAKE)
	$(LIBTOOL) --mode=link $(LIBTOOL_OPTS) $(CXX) $(CXXFLAGS) -o input-base.o $(OBJS)

install:
	@for i in $(SUBDIRS); do $(MAKE) -C $$i install; done

clean:
	@if test -f input.o ; then rm *.o ; fi
	$(DO_MAKECLEAN)

depend:
	make -C keyboard depend
	make -C replay depend
ifeq ($(LIRC),yes)
	make -C lirc depend
endif
ifeq ($(EVDEV),yes)
	make -C evdev depend
endif
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
