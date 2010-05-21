### The directory environment:

MMSDIR = ../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

LIBTOOL_OPTS = --tag=CXX

PRG = mms-movie-library
FFMPEG_LIBS=

CXXFLAGS= -I$(KETLAER)/include -I../ $(BOOST_INCLUDE) -I../plugins/input -I../plugins/output -I../plugins/feature/ -I../plugins/feature/movie/ -I../plugins/feature/movie/players/ -I../plugins/feature/movie/players/mplayer -I../plugins/ -pipe -O0 -g -DDEBUG -DSTANDALONE

OBJS = movie-library.o

OBJS += ../plugins/plugins.o ../plugins/output/output.o ../plugins/input/input-base.o ../plugins/feature/feature.o ../plugins/feature/movie/movie_config.o ../plugins/feature/movie/players/movie_player.o 

ifeq ($(FFMPEG_THUMBNAILER), yes)
  OBJS += ../plugins/feature/movie/thumbnailer.o
  ifeq ($(FFMPEG_EXTERNAL), yes)
    CXXFLAGS += `pkg-config --cflags libavcodec libavutil libswscale libavformat`
    FFMPEG_LIBS += `pkg-config --libs libavcodec libavutil libswscale libavformat`
  else
  CXXFLAGS += -isystem$(MMSDIR)/libs/ffmpeg/libavcodec/ -isystem$(MMSDIR)/libs/ffmpeg/libavutil/ -isystem$(MMSDIR)/libs/ffmpeg/libswscale/ -isystem$(MMSDIR)/libs/ffmpeg/libavformat/
  CXXFLAGS += -I$(MMSDIR)/libs/ffmpeg/ -I$(MMSDIR)/libs/ffmpeg/libavcodec/ -I$(MMSDIR)/libs/ffmpeg/libavutil/ -I$(MMSDIR)/libs/ffmpeg/libswscale/ -I$(MMSDIR)/libs/ffmpeg/libavformat/
  FFMPEG_LIBS = $(MMSDIR)/libs/ffmpeg/libavformat/libavformat.a $(MMSDIR)/libs/ffmpeg/libavcodec/libavcodec.a $(MMSDIR)/libs/ffmpeg/libavutil/libavutil.a $(MMSDIR)/libs/ffmpeg/libswscale/libswscale.a
  endif
  OBJS += $(FFMPEG_LIBS)

else
  OBJS += ../plugins/feature/movie/players/mplayer/*.o
endif

SUBDIRS += ../plugins/

DEP_OBJS = ../*.o

LIBS += `$(KETLAER)/bin/imlib2-config --libs`

CXXFLAGS += `pkg-config --cflags taglib`
LIBS += `pkg-config --libs taglib`

ifeq ($(INOTIFY),yes)
LIBS += -linotifytools
endif

ifeq ($(SQLITE),yes)
LIBS += -lsqlite3
endif

LIBS += $(COMMONCPP2_LIBS)
CXXFLAGS += $(COMMONCPP2_CFLAGS)

LIBS += $(KETLAER_DIR)/utility/libm2/libm2.a -lgettextlib

all: $(PRG)

$(PRG): movie-library.o
	@if test -f ../mms.o; then rm ../mms.o; fi
	$(DO_MAKE)
	$(LIBTOOL) --mode=link $(LIBTOOL_OPTS) $(CXX) $(CXXFLAGS) -o $(PRG) $(OBJS) $(DEP_OBJS) $(LIBS) -L/usr/lib -lz -ltag
	@rm -r .libs

install: $(PRG)
	$(MAKE) install -C man;
	$(INSTALL) -m 755 $(INSTALLSTRIP) $(PRG) $(DESTDIR)$(BINDIR)/$(PRG)

clean:
	@if test -f movie-library.o ; then rm *.o ; fi
	@if test -f $(PRG) ; then rm $(PRG) ; fi

distclean:
	$(MAKE) clean
