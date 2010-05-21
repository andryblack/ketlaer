### The directory environment:

MMSDIR = ../../../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_feature_audio_player_alsaplayer.so

SRCS += alsaplayer_config.cpp alsaplayer.cpp alsaplayer_plugin.cpp

OBJS += alsaplayer_config.o alsaplayer.o alsaplayer_plugin.o

CXXFLAGS += -I../ -I../../ -I../../../ -I../../../../ -I../../../../ \
	-I../../../../../ -I../../../../output/ \
	-I../../../../input/ -fPIC -shared

all: $(OBJS)
	$(CXX) $(CXXFLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) -lalsaplayer $(NEEDED_PLAYER_OBJS)

install:
	$(INSTALL) $(PLUGIN_NAME) $(DESTDIR)$(PLUGINDIR)/$(PLUGIN_NAME)

clean:
	$(DO_MAKECLEAN)
	@if test -f alsaplayer.o ; then rm *.o ; fi
	@if test -f $(PLUGIN_NAME) ; then rm *.so ; fi

depend:
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
