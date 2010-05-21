### The directory environment:

MMSDIR = ../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

SRCS = mpeg.cpp
OBJS = mpeg.o
CXXFLAGS += -I../ -I../../ -I../../../ -fPIC -shared

ifeq ($(FFMPEG_EXTERNAL), yes)
CXXFLAGS += `pkg-config --cflags libavcodec libavutil libswscale libavformat`
else
CXXFLAGS += -isystem$(MMSDIR)/libs/ffmpeg/libavcodec/ -isystem$(MMSDIR)/libs/ffmpeg/libavutil/ -isystem$(MMSDIR)/libs/ffmpeg/libswscale/ -isystem$(MMSDIR)/libs/ffmpeg/libavformat/
CXXFLAGS +=-I$(MMSDIR)/libs/ffmpeg/ -I$(MMSDIR)/libs/ffmpeg/libavcodec/ -I$(MMSDIR)/libs/ffmpeg/libavutil/ -I$(MMSDIR)/libs/ffmpeg/libswscale/ -I$(MMSDIR)/libs/ffmpeg/libavformat/
endif

all: $(OBJS)
	$(DO_MAKE)

clean:
	$(DO_MAKECLEAN)
	@if test -f mpeg.o ; then rm *.o ; fi

distclean:
	$(MAKE) clean

depend:
ifeq ($(VO_DXR3),yes)
	make -C dxr3 depend
endif
ifeq ($(VO_DVB),yes)
	make -C dvb depend
endif
ifeq ($(VO_MPEG),yes)
	make -C mpeg depend
endif
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
