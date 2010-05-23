### The directory environment:

MMSDIR = ../../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

SRCS = movie_player.cpp
OBJS = movie_player.o
CXXFLAGS += -I../ -I../../ -I../../../ -I../../../../ \
	-I../../../output/ -I../../../input/ -fPIC -shared

SUBDIRS += mplayer xine genericplayer rtd

all: $(OBJS)
	$(DO_MAKE)

install:
	@for i in $(SUBDIRS); do $(MAKE) -C $$i install; done

clean:
	$(DO_MAKECLEAN)
	@if test -f movie_player.o ; then rm *.o ; fi

depend:
	make -C mplayer depend
	make -C xine depend
	make -C genericplayer depend
	make -C rtd depend
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
