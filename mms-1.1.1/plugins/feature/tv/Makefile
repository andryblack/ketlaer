### The directory environment:

MMSDIR = ../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_feature_tv.so

SRCS = tv_config.cpp tv.cpp tv_plugin.cpp
OBJS = tv_config.o tv.o tv_plugin.o $(KETLAER_DIR)/vdr-ketlaerfe/libfe.o

CXXFLAGS += -I$(KETLAER_DIR)/vdr-ketlaerfe \
	-I../ -I../../ -I../../../ -I../tinyxml -I../../output/ \
	-I../../input/ -fPIC -shared

SUBDIRS += po

all: $(OBJS)
	$(DO_MAKE)
	make -C po
	$(CXX) $(CXXFLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) $(COMMON_LIBS) -lketlaer -lpthread

install:
	$(INSTALL) $(PLUGIN_NAME) $(DESTDIR)$(PLUGINDIR)/$(PLUGIN_NAME)
	make -C po install

clean:
	$(DO_MAKECLEAN)
	@rm -f *.o *.so *~

depend:
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
