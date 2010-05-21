### The directory environment:

MMSDIR = ../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

SRCS = plugins.cpp
OBJS = plugins.o

SUBDIRS += feature output input

CXXFLAGS += -I../ -I./feature/ -I./feature/audio/players/ -I./input \
	-I./output -I../libs/libfs++/include/

all: $(OBJS)
	$(DO_MAKE)

install:
	@for i in $(SUBDIRS); do $(MAKE) -C $$i install; done

clean:
	@if test -f plugins.o ; then rm *.o ; fi
	@for i in $(SUBDIRS); do $(MAKE) -C $$i clean; done

depend:
	make -C feature depend
	make -C output depend
	make -C input depend
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
