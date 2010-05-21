### The directory environment:

MMSDIR = ../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

SRCS = common-output.cpp graphic_object.cpp image.cpp \
	pic_object.cpp rectangle_object.cpp text_object.cpp \
	movie_object.cpp renderer.cpp render_device.cpp SSaverobj.cpp

OBJS = common-output.o graphic_object.o image.o \
	pic_object.o rectangle_object.o text_object.o \
	movie_object.o renderer.o render_device.o SSaverobj.o

CXXFLAGS += -I../ -I../../ -I../input -I../feature

all: output

ifeq ($(VO_DXR3),yes)
SUBDIRS += mpeg mpeg/dxr3
endif

ifeq ($(VO_DVB),yes)
SUBDIRS += mpeg mpeg/dvb
endif

ifeq ($(VO_MPEG),yes)
SUBDIRS += mpeg mpeg/mpeg
endif

ifeq ($(VO_SDL),yes)
SUBDIRS += sdl
endif

ifeq ($(VO_OPENGL),yes)
SUBDIRS += opengl
endif

ifeq ($(VO_LCD),yes)
SUBDIRS += lcd
endif

SUBDIRS += rtd

output-base: $(OBJS) $(DEP)
	$(LIBTOOL) --mode=link $(LIBTOOL_OPTS) $(CXX) $(CXXFLAGS) -o output.o $(OBJS)

output: $(OBJS) $(DEP)
	$(DO_MAKE)
	$(LIBTOOL) --mode=link $(LIBTOOL_OPTS) $(CXX) $(CXXFLAGS) -o output.o $(OBJS)

install:
	@for i in $(SUBDIRS); do $(MAKE) -C $$i install; done

clean:
	@if test -f common-output.o ; then rm *.o ; fi
	$(DO_MAKECLEAN)

depend:
ifeq ($(VO_DXR3),yes)
	make -C mpeg depend
endif
ifeq ($(VO_DVB),yes)
	make -C mpeg depend
endif
ifeq ($(VO_MPEG),yes)
	make -C mpeg depend
endif
ifeq ($(VO_SDL),yes)
	make -C sdl depend
endif
ifeq ($(VO_OPENGL),yes)
	make -C opengl depend
endif
ifeq ($(VO_LCD),yes)
	make -C lcd depend
endif
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
