### The directory environment:

MMSDIR = ../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_feature_audio.so

SRCS_LYRIC_FETCH = lyrics_fetch.cpp lyrics_helper_leoslyrics.cpp lyrics_helper_lyricwiki.cpp
OBJS_LYRIC_FETCH = lyrics_fetch.o lyrics_helper_leoslyrics.o lyrics_helper_lyricwiki.o

SRCS = audio_config.cpp audio_s.cpp audio.cpp shuffle_list.cpp  \
	audio_opts.cpp audio_plugin.cpp \
	simple_audio.cpp lastfm.cpp CD_Tag.cpp sha1.c base64.c \
	lyrics.cpp $(SRCS_LYRIC_FETCH) radio_config.cpp shoutcast.cpp

OBJS = audio_config.o audio_s.o audio.o shuffle_list.o \
	audio_opts.o audio_plugin.o \
	simple_audio.o lastfm.o CD_Tag.o sha1.o base64.o \
	lyrics.o $(OBJS_LYRIC_FETCH) radio_config.o shoutcast.o

CXXFLAGS += -I. -I../ -I../../ -I../../../ -I../../output/ \
	-I../../input/ -I../tinyxml/ -I./players/ -fPIC

SUBDIRS += po

LINKOPTS =

ifeq ($(FAUDIO), yes)
SRCS += dbaudiofile.cpp graphical_audio.cpp
OBJS += dbaudiofile.o graphical_audio.o
CXXFLAGS += `pkg-config --cflags taglib` `pkg-config --silence-errors --cflags libccext2`
LINKOPTS += `pkg-config --libs taglib` `pkg-config --silence-errors --libs libccext2`
endif

ifeq ($(IMMS), yes)
SRCS += clientstubbase.cc
OBJS += clientstubbase.o
CXXFLAGS += `pkg-config --cflags glib-2.0`
LINKOPTS += `pkg-config glib-2.0 --libs`
endif

CFLAGS += -fPIC

SUBDIRS += players

all: $(OBJS)
	$(DO_MAKE)
	make -C po
	$(CXX) $(CXXFLAGS) -shared $(LINKOPTS) \
	-ltag -Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) players/audio_player.o $(COMMON_LIBS)

install:
	@for i in $(SUBDIRS); do $(MAKE) -C $$i install; done
	$(INSTALL) $(PLUGIN_NAME) $(DESTDIR)$(PLUGINDIR)/$(PLUGIN_NAME)
	make -C po install

clean:
	$(DO_MAKECLEAN)
	@if test -f audio_config.o ; then rm *.o ; fi
	@if test -f $(PLUGIN_NAME) ; then rm *.so ; fi

wget_wrapper.o:
	make -C ../ SUBDIRS=""

depend:
	make -C players depend
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
