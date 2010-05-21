### The directory environment:

MMSDIR = ../../../../..

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak
include ../../Makefile.common

PLUGIN_NAME = ../../mmsv2gui.so

SRCS =  pycontrol.cpp pyrectanglecontrol.cpp pylistitem.cpp pyimagecontrol.cpp pylabelcontrol.cpp \
	pybuttoncontrol.cpp pylistcontrol.cpp pytextboxcontrol.cpp pywindow.cpp guipywindow.cpp \
	pytextfieldcontrol.cpp pyprogresscontrol.cpp pyimagelistcontrol.cpp \
	pyaction.cpp mmsv2guimodule.cpp

DEPOBJS = ../../libgui/libgui.o ../pyutil.o

CXXFLAGS += -I$(MMSDIR) -I$(MMSDIR)/plugins -I$(MMSDIR)/plugins/input -I$(MMSDIR)/plugins/output \
	    -I$(MMSDIR)/plugins/feature -I$(PYTHON_INCLUDE_PATH) -I../../libgui -I../ \
	    -I$(MMSDIR)/plugins/feature/tinyxml -fPIC -shared

all: $(OBJS)
	$(DO_MAKE)
	$(CXX) $(CXXFLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) $(DEPOBJS) -lpython$(PYTHON_VERSION) -lm

clean:
	$(DO_MAKECLEAN)
	@if test -f mmsv2guimodule.o ; then rm -f *.o ; fi
