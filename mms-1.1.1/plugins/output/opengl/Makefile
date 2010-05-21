### The directory environment:

MMSDIR = ../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_output_opengl.so

SRCS = SDL_GLout.cpp  openglout_plugin.cpp opengl_config.cpp opengl.cpp
OBJS = SDL_GLout.o openglout_plugin.o opengl_config.o opengl.o

CXXFLAGS += -I../ -I../../ -I../../../ -I../../feature/ -I../../input/ \
	 -fPIC -shared

all: $(OBJS)
	$(DO_MAKE)
	$(CXX) $(CXXFLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) $(COMMON_LIBS) `sdl-config --libs`  -lGL

install:
	$(INSTALL) $(PLUGIN_NAME) $(DESTDIR)$(PLUGINDIR)/$(PLUGIN_NAME)

clean:
	$(DO_MAKECLEAN)
	@if test -f opengl.o ; then rm *.o ; fi
	@if test -f $(PLUGIN_NAME) ; then rm *.so ; fi

depend:
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
