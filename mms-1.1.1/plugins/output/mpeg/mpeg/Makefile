### The directory environment:

MMSDIR = ../../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_output_mpeg.so

SRCS = mpeg_out.cpp mpegout_plugin.cpp
OBJS = mpeg_out.o mpegout_plugin.o
CXXFLAGS += -I../ -I../../ -I../../../ -I../../../../ \
	-I../../../feature/ -I../../../input/ -fPIC -shared

SUBDIRS += po

ifeq ($(FFMPEG_EXTERNAL), yes)
FFMPEG_FLAGS += `pkg-config --cflags libavcodec libavutil libswscale libavformat`
FFMPEG_LIBS += `pkg-config --libs libavcodec libavutil libswscale libavformat`
else
FFMPEG_FLAGS += -isystem$(MMSDIR)/libs/ffmpeg/libavcodec/ -isystem$(MMSDIR)/libs/ffmpeg/libavutil/ -isystem$(MMSDIR)/libs/ffmpeg/libswscale/ -isystem$(MMSDIR)/libs/ffmpeg/libavformat/
FFMPEG_FLAGS += -I$(MMSDIR)/libs/ffmpeg/ -I$(MMSDIR)/libs/ffmpeg/libavcodec/ -I$(MMSDIR)/libs/ffmpeg/libavutil/ -I$(MMSDIR)/libs/ffmpeg/libswscale/ -I$(MMSDIR)/libs/ffmpeg/libavformat/
FFMPEG_LIBS =  $(MMSDIR)/libs/ffmpeg/libavformat/libavformat.a $(MMSDIR)/libs/ffmpeg/libavcodec/libavcodec.a $(MMSDIR)/libs/ffmpeg/libavutil/libavutil.a $(MMSDIR)/libs/ffmpeg/libswscale/libswscale.a
endif

all: $(OBJS)
	$(DO_MAKE)
	make -C po
	$(CXX) $(CXXFLAGS) $(FFMPEG_FLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) ../mpeg.o $(COMMON_LIBS) $(FFMPEG_LIBS)

install:
	$(INSTALL) $(PLUGIN_NAME) $(DESTDIR)$(PLUGINDIR)/$(PLUGIN_NAME)
	make -C po install

clean:
	$(DO_MAKECLEAN)
	@if test -f mpeg_out.o ; then rm *.o ; fi
	@if test -f $(PLUGIN_NAME) ; then rm *.so ; fi

depend:
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
