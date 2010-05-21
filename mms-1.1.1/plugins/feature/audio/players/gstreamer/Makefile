### The directory environment:

MMSDIR = ../../../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_feature_audio_player_gst.so

SRCS += gstreamer.cpp gstreamer_plugin.cpp
OBJS += gstreamer.o gstreamer_plugin.o

CXXFLAGS += -I../ -I../../ -I../../../ -I../../../../ -I../../../../ \
	-I../../../../../ -I../../../../output/ \
	-I../../../../input/ -fPIC -shared \
	`pkg-config --cflags gstreamer-0.10`

all: $(OBJS)
	$(CXX) $(CXXFLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) `pkg-config --libs gstreamer-0.10` $(NEEDED_PLAYER_OBJS)

install:
	$(INSTALL) $(PLUGIN_NAME) $(DESTDIR)$(PLUGINDIR)/$(PLUGIN_NAME)

clean:
	$(DO_MAKECLEAN)
	@if test -f gstreamer.o ; then rm *.o ; fi
	@if test -f $(PLUGIN_NAME) ; then rm *.so ; fi

depend:
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
