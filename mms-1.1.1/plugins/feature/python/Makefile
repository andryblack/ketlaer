### The directory environment:

MMSDIR = ../../..

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak
include Makefile.common

CONFIGDIR = /etc/mms

PLUGIN_NAME = lib_feature_python.so

SRCS = python.cpp python_plugin.cpp guiwindowscripts.cpp python_config.cpp

DEPOBJS = libgui/libgui.o libpython/libpython.o

CXXFLAGS += -I$(MMSDIR) -I$(MMSDIR)/plugins/output -Ilibgui -Ilibpython -I$(MMSDIR)/plugins/input \
	    -I$(MMSDIR)/plugins/feature -I$(MMSDIR)/plugins -I$(MMSDIR)/plugins/feature/tinyxml \
	    -I$(PYTHON_INCLUDE_PATH) -fPIC -shared

CXXFLAGS += $(PYTHON_CFLAGS)

SUBDIRS += po

all: deps python

deps:
	make -C libgui
	make -C libpython

python:	$(OBJS)
	$(DO_MAKE)
	make -C po
	$(CXX) $(CXXFLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) $(DEPOBJS) -lpython$(PYTHON_VERSION) -lm

install: all
	$(INSTALL) $(PLUGIN_NAME) $(DESTDIR)$(PLUGINDIR)/$(PLUGIN_NAME)
	@if test ! -d $(DESTDIR)$(PYTHON_INSTALL) ; then mkdir -p $(DESTDIR)$(PYTHON_INSTALL) ; fi
	$(INSTALL) mmsv2.so $(DESTDIR)$(PYTHON_INSTALL)
	$(INSTALL) mmsv2gui.so $(DESTDIR)$(PYTHON_INSTALL)
	make -C po install
	cp -r scripts $(DESTDIR)$(CONFIGDIR)

clean:
	$(DO_MAKECLEAN)
	@if test -f python.o ; then rm -f *.o ; fi
	@if test -f $(PLUGIN_NAME) ; then rm *.so ; fi
