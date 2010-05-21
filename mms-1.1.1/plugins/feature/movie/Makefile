### The directory environment:

MMSDIR = ../../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PLUGIN_NAME = lib_feature_movie.so
PLUGIN_COLL_NAME = lib_feature_movie_collection.so
FFMPEG_LIBS =
SRCS = movie_config.cpp movie.cpp simple_movie.cpp movie_plugin.cpp
OBJS = movie_config.o movie.o simple_movie.o movie_plugin.o

ifeq ($(FMOVIE),yes)
SRCS += movie_db.cpp imdb.cpp imdb_movie.cpp \
	graphical_movie.cpp Movie_info.cpp
OBJS += movie_db.o imdb.o imdb_movie.o \
	graphical_movie.o Movie_info.o

SRCS += movie_collection_plugin.cpp \
	collection.cpp
COLL += movie.o movie_collection_plugin.o \
	movie_db.o imdb.o imdb_movie.o \
	collection.o
endif

CXXFLAGS += -I../ -I../../ -I../../../ -I../../output/ \
	-I../../input/ -fPIC -shared

ifeq ($(FFMPEG_THUMBNAILER), yes)
SRCS += thumbnailer.cpp
OBJS += thumbnailer.o
ifeq ($(FFMPEG_EXTERNAL), yes)
CXXFLAGS += `pkg-config --cflags libavcodec libavutil libswscale libavformat`
FFMPEG_LIBS += `pkg-config --libs libavcodec libavutil libswscale libavformat`
else
CXXFLAGS += -isystem$(MMSDIR)/libs/ffmpeg/libavcodec/ -isystem$(MMSDIR)/libs/ffmpeg/libavutil/ -isystem$(MMSDIR)/libs/ffmpeg/libswscale/ -isystem$(MMSDIR)/libs/ffmpeg/libavformat/
CXXFLAGS += -I$(MMSDIR)/libs/ffmpeg/ -I$(MMSDIR)/libs/ffmpeg/libavcodec/ -I$(MMSDIR)/libs/ffmpeg/libavutil/ -I$(MMSDIR)/libs/ffmpeg/libswscale/ -I$(MMSDIR)/libs/ffmpeg/libavformat/
FFMPEG_LIBS = $(MMSDIR)/libs/ffmpeg/libavformat/libavformat.a $(MMSDIR)/libs/ffmpeg/libavcodec/libavcodec.a $(MMSDIR)/libs/ffmpeg/libavutil/libavutil.a $(MMSDIR)/libs/ffmpeg/libswscale/libswscale.a
endif
endif

SUBDIRS += po

SUBDIRS += players

all: $(OBJS) $(COLL)
	$(DO_MAKE)
	make -C po
	$(CXX) $(CXXFLAGS) \
	-Wl,-nostdlib -o $(PLUGIN_NAME) $(OBJS) players/movie_player.o  $(FFMPEG_LIBS)
ifeq ($(FMOVIE),yes)
	$(CXX) $(CXXFLAGS) \
	-Wl,-nostdlib -o $(PLUGIN_COLL_NAME) $(COLL)
endif

install:
	@for i in $(SUBDIRS); do $(MAKE) -C $$i install; done
	$(INSTALL) $(PLUGIN_NAME) $(DESTDIR)$(PLUGINDIR)/
ifeq ($(FMOVIE),yes)
	$(INSTALL) $(PLUGIN_COLL_NAME) $(DESTDIR)$(PLUGINDIR)/
endif
	make -C po install

clean:
	$(DO_MAKECLEAN)
	@if test -f movie.o ; then rm *.o ; fi
	@if test -f $(PLUGIN_NAME) ; then rm *.so ; fi

depend:
	make -C players depend
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
