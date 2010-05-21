### The directory environment:

MMSDIR = ../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_feature_game.so

SRCS = game_config.cpp game.cpp game_plugin.cpp
OBJS = game_config.o game.o game_plugin.o

CXXFLAGS += -I../ -I../../ -I../../../ -I../tinyxml -I../../output/ \
	-I../../input/ -fPIC -shared

SUBDIRS += po

all: $(OBJS)
	$(DO_MAKE)
	make -C po
	$(CXX) $(CXXFLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) $(COMMON_LIBS)

install:
	$(INSTALL) $(PLUGIN_NAME) $(DESTDIR)$(PLUGINDIR)/$(PLUGIN_NAME)
	make -C po install

clean:
	$(DO_MAKECLEAN)
	@if test -f game.o ; then rm *.o ; fi
	@if test -f $(PLUGIN_NAME) ; then rm *.so ; fi

depend:
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
