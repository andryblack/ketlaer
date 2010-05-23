### The directory environment:

MMSDIR = ../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_ainput_rtdinp.so

SRCS = rtdinp.cpp rtdinp_plugin.cpp
OBJS = rtdinp.o rtdinp_plugin.o

CXXFLAGS += -I$(KETLAER_DIR)/libketlaer \
	-I../ -I../../ -I../../../ -I../../feature/ -I../../output/ \
	-fPIC -shared

RTDLIBS = -lketlaer -lpthread

all: $(OBJS)
	$(DO_MAKE)
	$(CXX) $(CXXFLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) $(RTDLIBS)

install: all
	$(INSTALL) $(PLUGIN_NAME) $(DESTDIR)$(PLUGINDIR)/$(PLUGIN_NAME)

clean:
	$(DO_MAKECLEAN)
	@rm -f *.o *.so *~

depend:
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
