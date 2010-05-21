### The directory environment:

MMSDIR = ../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_feature_vbox.so

SRCS = vbox.cpp vbox.cpp vbox_plugin.cpp
OBJS = vbox_config.o vbox.o vbox_plugin.o vboxclient.o libvbox.o

CXXFLAGS += -I../ -I../../ -I../../../ -I../../output/ \
	-I../../input/ -I../../feature -I../../feature/audio -I../../feature/audio/players -fPIC -shared

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
	@if test -f vbox.o ; then rm *.o ; fi
	@if test -f $(PLUGIN_NAME) ; then rm *.so ; fi

depend:
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
