### The directory environment:

MMSDIR = ../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

LIBTOOL_OPTS = --tag=CXX

PRG = mms-audio-library

CXXFLAGS= -I$(KETLAER)/include -I../ $(BOOST_INCLUDE) -I../plugins/output -I../plugins/feature/ -I../plugins/feature/audio/ -pipe -O0 -g -DDEBUG -DSTANDALONE

OBJS = audio-library.o

OBJS += ../plugins/plugins.o ../plugins/output/output.o ../plugins/input/input-base.o ../plugins/feature/feature.o ../plugins/feature/audio/audio_config.o

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

LIBS += $(KETLAER_DIR)/utility/libm2/libm2.a -lgettextpo -lgettextlib

all: $(PRG)

$(PRG): audio-library.o
	@if test -f ../mms.o; then rm ../mms.o; fi
	$(DO_MAKE)
	$(LIBTOOL) --mode=link $(LIBTOOL_OPTS) $(CXX) $(CXXFLAGS) -o $(PRG) $(OBJS) $(DEP_OBJS) $(LIBS) -L/usr/lib -lsqlite3 -lz -ltag
	@rm -r .libs

install: $(PRG)
	$(MAKE) install -C man;
	$(INSTALL) -m 755 $(INSTALLSTRIP) $(PRG) $(DESTDIR)$(BINDIR)/$(PRG)

clean:
	@if test -f audio-library.o ; then rm *.o ; fi
	@if test -f $(PRG) ; then rm $(PRG) ; fi

distclean:
	$(MAKE) clean
