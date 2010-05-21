### The directory environment:

MMSDIR = ../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

SRCS = common-feature.cpp module.cpp gsimplefile.cpp wget_wrapper.cpp playlist_parser.cpp
OBJS = common-feature.o module.o gsimplefile.o wget_wrapper.o playlist_parser.o

CXXFLAGS += -I../ -I../../  -I../input -I../output

ifeq ($(EPG), yes)
XML_DIR += tinyxml
XML_LIB += tinyxml/tinyxml.o tinyxml/tinyxmlparser.o tinyxml/tinyxmlerror.o tinyxml/tinystr.o
else
ifeq ($(AUDIO), yes)
XML_DIR += tinyxml
XML_LIB += tinyxml/tinyxml.o tinyxml/tinyxmlparser.o tinyxml/tinyxmlerror.o tinyxml/tinystr.o
else
ifeq ($(WEATHER), yes)
XML_DIR += tinyxml
XML_LIB += tinyxml/tinyxml.o tinyxml/tinyxmlparser.o tinyxml/tinyxmlerror.o tinyxml/tinystr.o
else
ifeq ($(PYTHON), yes)
XML_DIR += tinyxml
XML_LIB += tinyxml/tinyxml.o tinyxml/tinyxmlparser.o tinyxml/tinyxmlerror.o tinyxml/tinystr.o
endif
endif
endif
endif

all: feature

ifeq ($(MUSIC),yes)
SUBDIRS += audio
INSTALLDIRS += audio
endif

ifeq ($(MOVIE),yes)
SUBDIRS += movie
INSTALLDIRS += movie
endif

ifeq ($(PICTURE),yes)
SUBDIRS += picture
INSTALLDIRS += picture
endif

ifeq ($(CLOCK),yes)
SUBDIRS += clock
INSTALLDIRS += clock
endif

ifeq ($(WEATHER),yes)
SUBDIRS += weather
INSTALLDIRS += weather
endif

ifeq ($(VBOX),yes)
SUBDIRS += vbox
INSTALLDIRS += vbox
endif

ifeq ($(EPG),yes)
SUBDIRS += epg
INSTALLDIRS += epg
endif

ifeq ($(TV),yes)
SUBDIRS += tv
INSTALLDIRS += tv
endif

ifeq ($(GAME),yes)
SUBDIRS += game
INSTALLDIRS += game
endif

ifeq ($(PYTHON),yes)
SUBDIRS += python
INSTALLDIRS += python
endif

feature-base: $(OBJS) $(DEP)
ifdef XML_DIR 
	make -C $(XML_DIR)
endif
	$(LIBTOOL) --mode=link $(LIBTOOL_OPTS) $(CXX) $(CXXFLAGS) -o feature.o $(OBJS) $(XML_LIB)

feature: $(OBJS) $(DEP)
	$(DO_MAKE)
ifdef XML_DIR
	make -C $(XML_DIR)
endif
	$(LIBTOOL) --mode=link $(LIBTOOL_OPTS) $(CXX) $(CXXFLAGS) -o feature.o $(OBJS) $(XML_LIB)

install:
	@for i in $(INSTALLDIRS); do $(MAKE) -C $$i install; done

clean:
	@if test -f common-feature.o ; then rm *.o ; fi
	$(DO_MAKECLEAN)

depend:
	make -C audio depend
	make -C movie depend
	make -C picture depend
	make -C epg depend
	make -C weather depend
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
