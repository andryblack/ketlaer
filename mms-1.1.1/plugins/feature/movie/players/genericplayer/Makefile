### The directory environment:

MMSDIR = ../../../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_feature_movie_player_genericplayer.so

SRCS += genericplayer.cpp genericplayer_config.cpp genericplayer_plugin.cpp
OBJS += genericplayer.o genericplayer_config.o genericplayer_plugin.o

CXXFLAGS += -I../ -I../../ -I../../../ -I../../../../ -I../../../../ \
	-I../../../../../ -I../../../../output/ \
	-I../../../../input/ -fPIC -shared

ifeq ($(FFMPEG_THUMBNAILER), yes)
ifeq ($(FFMPEG_EXTERNAL), yes)
CXXFLAGS += `pkg-config --cflags libavcodec libavutil libswscale libavformat`
FFMPEG_LIBS += `pkg-config --libs libavcodec libavutil libswscale libavformat`
else
CXXFLAGS += -isystem$(MMSDIR)/libs/ffmpeg/libavcodec/ -isystem$(MMSDIR)/libs/ffmpeg/libavutil/ -isystem$(MMSDIR)/libs/ffmpeg/libswscale/ -isystem$(MMSDIR)/libs/ffmpeg/libavformat/
CXXFLAGS += -I$(MMSDIR)/libs/ffmpeg/ -I$(MMSDIR)/libs/ffmpeg/libavcodec/ -I$(MMSDIR)/libs/ffmpeg/libavutil/ -I$(MMSDIR)/libs/ffmpeg/libswscale/ -I$(MMSDIR)/libs/ffmpeg/libavformat/
endif
endif

SUBDIRS += po

all: $(OBJS)
	$(DO_MAKE)
	make -C po
	$(CXX) $(CXXFLAGS) -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) $(NEEDED_PLAYER_OBJS)

install:
	$(INSTALL) $(PLUGIN_NAME) $(DESTDIR)$(PLUGINDIR)/$(PLUGIN_NAME)
	make -C po install

clean:
	$(DO_MAKECLEAN)
	@if test -f genericplayer.o ; then rm *.o ; fi
	@if test -f $(PLUGIN_NAME) ; then rm *.so ; fi

depend:
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
