### The directory environment:

MMSDIR = ../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_feature_clock.so

SRCS = clock_config.cpp clock.cpp clock_plugin.cpp
OBJS = clock_config.o clock.o clock_plugin.o


CXXFLAGS += -I../ -I../../ -I../../../ -I../tinyxml -I../../output/ \
	-I../../input/ -fPIC -shared


ifeq ($(AUDIO),yes)
SRC += alarm_config.cpp clock_opts.cpp clock_alarms.cpp
OBJS += alarm_config.o clock_opts.o clock_alarms.o
CXXFLAGS += -I../audio/ -I../audio/players
endif

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
	@if test -f clock.o ; then rm *.o ; fi
	@if test -f $(PLUGIN_NAME) ; then rm *.so ; fi

depend:
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
