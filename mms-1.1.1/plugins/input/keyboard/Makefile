### The directory environment:

MMSDIR = ../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_ainput_keyboard.so

SRCS = keyboard.cpp keyboard_plugin.cpp
OBJS = keyboard.o keyboard_plugin.o
CXXFLAGS += -I../ -I../../ -I../../../ -I../../feature/ -I../../output/ \
	-I../../output/sdl -fPIC -shared

SUBDIRS += termio
COMMON_LIBS += -lncurses

ifeq ($(VO_SDL),yes)
EXTRA_OBJS = `sdl-config --libs`
endif

all: $(OBJS)
	$(DO_MAKE)
	$(CXX) $(CXXFLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) termio/getch2.o $(COMMON_LIBS) $(EXTRA_OBJS)

install:
	$(INSTALL) $(PLUGIN_NAME) $(DESTDIR)$(PLUGINDIR)/$(PLUGIN_NAME)

clean:
	$(DO_MAKECLEAN)
	@if test -f keyboard.o ; then rm *.o ; fi
	@if test -f $(PLUGIN_NAME) ; then rm *.so ; fi

depend:
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
