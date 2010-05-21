### The directory environment:

MMSDIR = ../../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

SRCS = audio_player.cpp
OBJS = audio_player.o

CXXFLAGS += -I../ -I../../ -I../../../ -I../../../../ \
	-I../../../output/ -I../../../input/ -fPIC -shared

ifeq ($(APLAYER_XINE),yes)
SUBDIRS += xine
endif

ifeq ($(APLAYER_ALSA),yes)
SUBDIRS += alsaplayer
endif

ifeq ($(APLAYER_GST),yes)
SUBDIRS += gstreamer
endif

SUBDIRS += rtd

all: $(OBJS)
	$(DO_MAKE)

install:
	@for i in $(SUBDIRS); do $(MAKE) -C $$i install; done

clean:
	$(DO_MAKECLEAN)
	@if test -f audio_player.o ; then rm *.o ; fi

depend:
ifeq ($(APLAYER_ALSA),yes)
	make -C alsaplayer depend
endif
ifeq ($(APLAYER_GST),yes)
	make -C gstreamer depend
endif
ifeq ($(APLAYER_XINE),yes)
	make -C xine depend
endif
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
