### The directory environment:

MMSDIR = ../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_output_sdl.so

SRCS = sdlout.cpp sdlout_plugin.cpp
OBJS = sdlout.o sdlout_plugin.o

CXXFLAGS += -I../ -I../../ -I../../../ -I../../feature/ -I../../input/ \
	 -fPIC -shared

SUBDIRS += po

all: $(OBJS)
	$(DO_MAKE)
	make -C po
ifeq ($(X11),yes)
	$(CXX) $(CXXFLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) $(COMMON_LIBS) `sdl-config --libs` -lXfixes
else
	$(CXX) $(CXXFLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) $(COMMON_LIBS) `sdl-config --libs`
endif

install:
	$(INSTALL) $(PLUGIN_NAME) $(DESTDIR)$(PLUGINDIR)/$(PLUGIN_NAME)
	make -C po install

clean:
	$(DO_MAKECLEAN)
	@if test -f sdlout.o ; then rm *.o ; fi
	@if test -f $(PLUGIN_NAME) ; then rm *.so ; fi

depend:
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
