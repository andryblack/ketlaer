### The directory environment:

MMSDIR = ../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_output_rtdout.so

SRCS = rtdout.cpp rtdout_plugin.cpp
OBJS = rtdout.o rtdout_plugin.o

CXXFLAGS += -O3 -I$(KETLAER_DIR)/libketlaer \
            -I../ -I../../ -I../../../ -I../../feature/ -I../../input/ \
	    -fPIC -shared

RTDLIBS = -lketlaer -lpthread


all: $(OBJS)
	$(DO_MAKE)
	$(CXX) $(CXXFLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) $(COMMON_LIBS) $(RTDLIBS)

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
