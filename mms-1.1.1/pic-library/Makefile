### The directory environment:

MMSDIR = ../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

LIBTOOL_OPTS = --tag=CXX

PRG = mms-pic-library

CXXFLAGS= -I$(KETLAER)/include -I../ $(BOOST_INCLUDE) -I../plugins/output -I../plugins/feature/picture/ -pipe -O0 -g -DDEBUG -DSTANDALONE

OBJS = pic-library.o

OBJS += ../plugins/plugins.o ../plugins/output/output.o ../plugins/input/input-base.o ../plugins/feature/feature.o ../plugins/feature/picture/picture_config.o

SUBDIRS += ../plugins/

DEP_OBJS = ../*.o

LIBS += `$(KETLAER)/bin/imlib2-config --libs`

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

$(PRG): pic-library.o
	@if test -f ../mms.o; then rm ../mms.o; fi
	$(DO_MAKE)
	$(LIBTOOL) --mode=link $(LIBTOOL_OPTS) $(CXX) $(CXXFLAGS) -o $(PRG) $(OBJS) $(DEP_OBJS) $(LIBS) -L/usr/lib -lz -ltag
	@rm -r .libs

install: $(PRG)
	$(MAKE) install -C man;
	$(INSTALL) -m 755 $(INSTALLSTRIP) $(PRG) $(DESTDIR)$(BINDIR)/$(PRG)

clean:
	@if test -f pic-library.o ; then rm *.o ; fi
	@if test -f $(PRG) ; then rm $(PRG) ; fi

distclean:
	$(MAKE) clean
