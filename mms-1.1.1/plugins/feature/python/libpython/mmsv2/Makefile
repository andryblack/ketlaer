### The directory environment:

MMSDIR = ../../../../..

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak
include ../../Makefile.common

PLUGIN_NAME = ../../mmsv2.so

SRCS = audioplayer.cpp videoplayer.cpp playlist.cpp pyplayer.cpp \
       pyplaylist.cpp playlistthread.cpp mmsv2module.cpp

DEPOBJS = ../pyutil.o

CXXFLAGS += -I$(MMSDIR) -I$(MMSDIR)/plugins -I$(MMSDIR)/plugins/input -I$(MMSDIR)/plugins/output \
	    -I$(MMSDIR)/plugins/feature -I$(MMSDIR)/plugins/feature/audio -I../ -I../../libgui \
	    -I$(MMSDIR)/plugins/feature/audio/players -I$(MMSDIR)/plugins/feature/movie \
	    -I$(MMSDIR)/plugins/feature/movie/players -I$(MMSDIR)/plugins/feature/tinyxml \
	    -I$(PYTHON_INCLUDE_PATH) -fPIC -shared

all: $(OBJS)
	$(DO_MAKE)
	$(CXX) $(CXXFLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) $(DEPOBJS) $(PYTHON_LIB) -lm

clean:
	$(DO_MAKECLEAN)
	@if test -f mmsv2module.o ; then rm -f *.o ; fi
