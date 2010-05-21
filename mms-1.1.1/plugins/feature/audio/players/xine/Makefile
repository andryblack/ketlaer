### The directory environment:

MMSDIR = ../../../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_feature_audio_player_xine.so

SRCS += xine.cpp xine_plugin.cpp
OBJS += xine.o xine_plugin.o

CXXFLAGS += -I../ -I../../ -I../../../ -I../../../../ \
	-I../../../../ 	-I../../../../../ -I../../../../output/ \
	-I../../../../input/ -fPIC -shared

CXXFLAGS += `xine-config --cflags`

SUBDIRS += po

all: $(OBJS)
	$(DO_MAKE)
	make -C po
	$(CXX) $(CXXFLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) `xine-config --libs`  $(NEEDED_PLAYER_OBJS)

install:
	$(INSTALL) $(PLUGIN_NAME) $(DESTDIR)$(PLUGINDIR)/$(PLUGIN_NAME)
	make -C po install

clean:
	$(DO_MAKECLEAN)
	@if test -f xine.o ; then rm *.o ; fi
	@if test -f $(PLUGIN_NAME) ; then rm *.so ; fi

depend:
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
