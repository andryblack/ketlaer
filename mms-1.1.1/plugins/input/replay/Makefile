### The directory environment:

MMSDIR = ../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_ainput_replay.so

SRCS = replay_device.cpp replay_device_plugin.cpp
OBJS = replay_device.o replay_device_plugin.o

CXXFLAGS += -I../ -I../../ -I../../../ -I../../feature/ -I../../output/ \
	-fPIC -shared

all: $(OBJS)
	$(DO_MAKE)
	$(CXX) $(CXXFLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS)

install:
	$(INSTALL) $(PLUGIN_NAME) $(DESTDIR)$(PLUGINDIR)/$(PLUGIN_NAME)

clean:
	$(DO_MAKECLEAN)
	@if test -f replay_device.o ; then rm *.o ; fi
	@if test -f $(PLUGIN_NAME) ; then rm *.so ; fi

depend:
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
