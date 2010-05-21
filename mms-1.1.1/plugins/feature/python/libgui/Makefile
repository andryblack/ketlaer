### The directory environment:

MMSDIR = ../../../..

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak
include ../Makefile.common

SRCS = guiwindow.cpp guicontrol.cpp guiimagecontrol.cpp guilabelcontrol.cpp \
       guirectanglecontrol.cpp guibuttoncontrol.cpp guilistitem.cpp \
       guilistcontrol.cpp guitextboxcontrol.cpp guiwindowmanager.cpp \
       guicontrolfactory.cpp guimessage.cpp guitextfieldcontrol.cpp \
       guiprogresscontrol.cpp guiimagelistcontrol.cpp guiutil.cpp guitouchmanager.cpp

CXXFLAGS += -I$(MMSDIR) -I$(MMSDIR)/plugins -I$(MMSDIR)/plugins/input -I$(MMSDIR)/plugins/output \
            -I$(MMSDIR)/plugins/feature -I$(MMSDIR)/plugins/feature/tinyxml -fPIC -shared

CXXFLAGS += $(PYTHON_CFLAGS)

all: libgui

libgui: $(OBJS)
	$(LIBTOOL) --mode=link $(LIBTOOL_OPTS) $(CXX) $(CXXFLAGS) -o libgui.o $(OBJS)

clean:
	@if test -f libgui.o ; then rm *.o ; fi
	$(DO_MAKECLEAN)
