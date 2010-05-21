### Includes and Defines

MMSDIR=.

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

PRG = mms

SRCS = common.cpp config.cpp global_options.cpp mms.cpp option.cpp options.cpp \
	rand.cpp simplefile.cpp startmenu.cpp theme.cpp themes.cpp \
	updater.cpp cd.cpp print.cpp extra_menu.cpp global.cpp libfspp.cpp \
	general.cc file_t.cc file_monitor.cc search.cpp replay.cpp \
	busy_indicator.cpp shutdown.cpp touch.cpp resolution.cpp regextools.cpp

OBJS = common.o config.o global_options.o mms.o option.o options.o \
	rand.o simplefile.o startmenu.o theme.o themes.o updater.o \
	cd.o print.o extra_menu.o global.o libfspp.o general.o \
	file_t.o file_monitor.o search.o replay.o busy_indicator.o \
	shutdown.o touch.o resolution.o regextools.o

ifeq ($(INOTIFY),yes)
SRCS += notify.cpp
OBJS += notify.o
COMMON_LIBS += -linotifytools
endif

ifeq ($(SQLITE),yes)
SRCS += cpp_sqlitepp.cpp libbitap.c
OBJS += cpp_sqlitepp.o libbitap.o
COMMON_LIBS += -lsqlite3
endif

ifeq ($(BENCH),yes)
SRCS += bench.cpp
OBJS += bench.o
endif

ifeq ($(FFMPEG_EXTERNAL), no)
ifeq ($(FFMPEG_THUMBNAILER), yes)
SUBDIRS += libs/ffmpeg
else
ifeq ($(MPEG), yes)
SUBDIRS += libs/ffmpeg
endif
endif
endif
EXTRA_DISTCLEAN = rm -f libavcodec-config.log && rm -f libs/ffmpeg/*.pc && rm -f libs/ffmpeg/config.*

ifeq ($(NOTIFYAREA),yes)
SRCS += notify_area.cpp
OBJS += notify_area.o
endif

CXXFLAGS += -I./plugins -I./plugins/feature/ -I./plugins/input \
       -I./plugins/output

SUBDIRS += po

SUBDIRS += plugins

# Used for clock support
COMMON_LIBS += -lrt

ifeq ($(MUSIC),yes)
SUBDIRS += audio-library
endif

ifeq ($(MOVIE),yes)
SUBDIRS += movie-library
endif

ifeq ($(PICTURE),yes)
SUBDIRS += pic-library
endif

EXTRALIBS = -lgettextlib $(KETLAER_DIR)/utility/libm2/libm2.a

# plugins
#SRCS += plugins/plugins.cpp plugins/output/common-output.cpp plugins/input/input.cpp plugins/feature/module.cpp
OBJS += plugins/plugins.o plugins/output/output.o plugins/input/input-base.o plugins/feature/feature.o

all: version.h $(PRG)

plugins/output/output.o:
	@cd plugins/output && make output-base && cd ../..

plugins/input/input-base.o:
	@cd plugins/input && make input-base && cd ../..

plugins/feature/feature.o:
	@cd plugins/feature && make feature-base && cd ../..

plugins/plugins.o:
	@cd plugins && make plugins.o && cd ..

MMS_DEP = $(OBJS) $(COMMON_DEPS)

$(PRG): $(MMS_DEP)
	$(DO_MAKE)
	make mms.o
	$(LIBTOOL) --mode=link $(LIBTOOL_OPTS) $(CXX) $(CXXFLAGS) -o $(PRG) \
	-rdynamic --export-dynamic $(OBJS) $(COMMON_LIBS) `$(KETLAER)/bin/imlib2-config --libs` $(EXTRALIBS)
	@rm -r .libs

core: $(MMS_DEP)
	make mms.o
	$(LIBTOOL) --mode=link $(LIBTOOL_OPTS) $(CXX) $(CXXFLAGS) -o $(PRG) \
	-rdynamic --export-dynamic $(OBJS) $(COMMON_LIBS) `$(KETLAER)/bin/imlib2-config --libs` $(EXTRALIBS)
	@rm -r .libs

# ./configure must be run if it changed
config.h: configure
	@echo "############################################################"
	@echo "####### Please run ./configure again - it's changed! #######"
	@echo "############################################################"
	@exit 1

# rebuild at every config.h/config.mak change:
version.h:
	./version.sh
	#$(MAKE) clean
	#$(MAKE) depend

# rebuild at every CVS update or config/makefile change:
version.h: # config.h config.mak Makefile

# Every mms dependency depends on version.h, to force building version.h
# first (in serial mode) before any other of the dependencies for a parallel make
# run.  This is necessary, because the make rule for version.h removes objects
# in a recursive "make distclean" and we must wait for this "make distclean" to
# finish before we can start building new object files.
# Taken from mplayer's Makefile
$(MMS_DEP): version.h

OLD_PWD = ${PWD}

LOCAL_PATCH = "echo"

ifeq ($(LIRC),yes)
	LOCAL_PATCH = make patch-lirc-local
else
ifeq ($(EVDEV),yes)
	LOCAL_PATCH = make patch-evdev-local
endif
endif

ETC_PATCH = "echo"

ifeq ($(LIRC),yes)
	ETC_PATCH = make patch-lirc
else
ifeq ($(EVDEV),yes)
	ETC_PATCH = make patch-evdev
endif
endif

patch-lirc:
	@cd $(DESTDIR)$(CONFIGDIR)/ && patch -p1 < $(OLD_PWD)/cfg/patch-lirc

patch-evdev:
	@cd $(DESTDIR)$(CONFIGDIR)/ && patch -p1 < $(OLD_PWD)/cfg/patch-evdev

patch-lirc-local:
	@cd $(HOME)/.mms && patch -p1 < $(OLD_PWD)/cfg/patch-lirc

patch-evdev-local:
	@cd $(HOME)/.mms && patch -p1 < $(OLD_PWD)/cfg/patch-evdev

install: $(PRG)
	$(MAKE) install -C po;
	$(MAKE) install -C man;
	if test ! -d $(DESTDIR)$(BINDIR) ; then mkdir -p $(DESTDIR)$(BINDIR) ; fi
	$(INSTALL) -m 755 $(INSTALLSTRIP) $(PRG) $(DESTDIR)$(BINDIR)/$(PRG)
ifeq ($(FFMPEG_THUMBNAILER),no)
	$(INSTALL) -m 755 tools/midentify $(DESTDIR)$(BINDIR)/midentify
endif
	@if test ! -d $(DESTDIR)$(DATADIR) ; then mkdir -p $(DESTDIR)$(DATADIR) ; fi
	cp -r themes $(DESTDIR)$(DATADIR)/ ;
	if test ! -d $(DESTDIR)$(DATADIR)/fonts ; then cp -r fonts $(DESTDIR)$(DATADIR)/ ; fi
	if test ! -d $(DESTDIR)$(CONFIGDIR) ; then mkdir -p $(DESTDIR)$(CONFIGDIR) ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/MovieConfig ; then $(INSTALL) -c -m 644 cfg/MovieConfig $(DESTDIR)$(CONFIGDIR)/MovieConfig ; fi
ifeq ($(VO_SDL),yes)
	if test ! -f $(DESTDIR)$(CONFIGDIR)/Config ; then $(INSTALL) -c -m 644 cfg/Config-sdl $(DESTDIR)$(CONFIGDIR)/Config && $(ETC_PATCH); fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/XineConfig ; then $(INSTALL) -c -m 644 cfg/XineConfig-sdl $(DESTDIR)$(CONFIGDIR)/XineConfig ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/MplayerConfig ; then $(INSTALL) -c -m 644 cfg/Mplayer-sdl $(DESTDIR)$(CONFIGDIR)/MplayerConfig ; fi
endif
ifeq ($(VO_OPENGL),yes)
	if test ! -f $(DESTDIR)$(CONFIGDIR)/Config ; then $(INSTALL) -c -m 644 cfg/Config-opengl $(DESTDIR)$(CONFIGDIR)/Config && $(ETC_PATCH); fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/XineConfig ; then $(INSTALL) -c -m 644 cfg/XineConfig-sdl $(DESTDIR)$(CONFIGDIR)/XineConfig ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/MplayerConfig ; then $(INSTALL) -c -m 644 cfg/Mplayer-sdl $(DESTDIR)$(CONFIGDIR)/MplayerConfig ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/OpenglConfig ; then $(INSTALL) -c -m 644 cfg/OpenglConfig $(DESTDIR)$(CONFIGDIR)/OpenglConfig ; fi
endif
ifeq ($(VO_DXR3),yes)
	if test ! -f $(DESTDIR)$(CONFIGDIR)/Config ; then $(INSTALL) -c -m 644 cfg/Config-dxr3 $(DESTDIR)$(CONFIGDIR)/Config && $(ETC_PATCH); fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/XineConfig ; then $(INSTALL) -c -m 644 cfg/XineConfig-dxr3 $(DESTDIR)$(CONFIGDIR)/XineConfig ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/MplayerConfig ; then $(INSTALL) -c -m 644 cfg/Mplayer-dxr3 $(DESTDIR)$(CONFIGDIR)/MplayerConfig ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/Dxr3Config ; then $(INSTALL) -c -m 644 cfg/Dxr3Config $(DESTDIR)$(CONFIGDIR)/Dxr3Config ; fi
endif
ifeq ($(VO_DVB),yes)
	if test ! -f $(DESTDIR)$(CONFIGDIR)/Config ; then $(INSTALL) -c -m 644 cfg/Config-dvb $(DESTDIR)$(CONFIGDIR)/Config && $(ETC_PATCH); fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/XineConfig ; then $(INSTALL) -c -m 644 cfg/XineConfig-dvb $(DESTDIR)$(CONFIGDIR)/XineConfig ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/MplayerConfig ; then $(INSTALL) -c -m 644 cfg/Mplayer-dvb $(DESTDIR)$(CONFIGDIR)/MplayerConfig ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/DvbConfig ; then $(INSTALL) -c -m 644 cfg/DvbConfig $(DESTDIR)$(CONFIGDIR)/DvbConfig ; fi
endif
ifeq ($(PYTHON),yes)
	if test ! -d $(DESTDIR)$(CONFIGDIR)/scripts ; then mkdir -p $(DESTDIR)$(CONFIGDIR)/scripts ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/PythonConfig ; then $(INSTALL) -c -m 644 cfg/PythonConfig $(DESTDIR)$(CONFIGDIR)/PythonConfig ; fi
endif
	if test ! -d $(DESTDIR)$(LIBDIR)/mms ; then mkdir -p $(DESTDIR)$(LIBDIR)/mms ; fi
ifeq ($(WEATHER),yes)
	if test ! -f $(DESTDIR)$(CONFIGDIR)/WeatherConfig ; then $(INSTALL) -c -m 644 cfg/WeatherConfig $(DESTDIR)$(CONFIGDIR)/WeatherConfig ; fi
endif
ifeq ($(CLOCK),yes)
	if test ! -f $(DESTDIR)$(CONFIGDIR)/ClockConfig ; then $(INSTALL) -c -m 644 cfg/ClockConfig $(DESTDIR)$(CONFIGDIR)/ClockConfig ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/ClockAlarms ; then $(INSTALL) -c -m 644 cfg/ClockAlarms $(DESTDIR)$(CONFIGDIR)/ClockAlarms ; fi
	if test ! -f $(DESTDIR)$(LIBDIR)/mms/alarm.sh ; then $(INSTALL) -c -m 755 tools/alarm.sh $(DESTDIR)$(LIBDIR)/mms/alarm.sh ; fi
endif
ifeq ($(VBOX),yes)
	if test ! -f $(DESTDIR)$(CONFIGDIR)/VBoxConfig ; then $(INSTALL) -c -m 644 cfg/VBoxConfig $(DESTDIR)$(CONFIGDIR)/VBoxConfig ; fi
	if test ! -f $(DESTDIR)$(LIBDIR)/mms/vboxtowav.sh ; then $(INSTALL) -c -m 755 tools/vboxtowav.sh $(DESTDIR)$(LIBDIR)/mms/vboxtowav.sh ; fi
endif
	if test ! -f $(DESTDIR)$(CONFIGDIR)/genericplayer.ops ; then $(INSTALL) -c -m 644 cfg/genericplayer.ops $(DESTDIR)$(CONFIGDIR)/genericplayer.ops ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/GenericPlayerConfig ; then $(INSTALL) -c -m 644 cfg/GenericPlayerConfig $(DESTDIR)$(CONFIGDIR)/GenericPlayerConfig ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/AudioConfig ; then $(INSTALL) -c -m 644 cfg/AudioConfig $(DESTDIR)$(CONFIGDIR)/AudioConfig ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/RadioConfig ; then $(INSTALL) -c -m 644 cfg/RadioConfig $(DESTDIR)$(CONFIGDIR)/RadioConfig ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/AlsaplayerConfig ; then $(INSTALL) -c -m 644 cfg/AlsaplayerConfig $(DESTDIR)$(CONFIGDIR)/AlsaplayerConfig ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/PictureConfig ; then $(INSTALL) -c -m 644 cfg/PictureConfig $(DESTDIR)$(CONFIGDIR)/PictureConfig ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/EPGConfig ; then $(INSTALL) -c -m 644 cfg/EPGConfig $(DESTDIR)$(CONFIGDIR)/EPGConfig ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/GameConfig ; then $(INSTALL) -c -m 644 cfg/GameConfig $(DESTDIR)$(CONFIGDIR)/GameConfig ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/LcdConfig ; then $(INSTALL) -c -m 644 cfg/LcdConfig $(DESTDIR)$(CONFIGDIR)/LcdConfig ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/TVConfig ; then $(INSTALL) -c -m 644 cfg/TVConfig $(DESTDIR)$(CONFIGDIR)/TVConfig ; fi
	if test ! -d $(DESTDIR)$(CONFIGDIR)/input ; then mkdir -p $(DESTDIR)$(CONFIGDIR)/input ; fi
	if test ! -d $(DESTDIR)$(CONFIGDIR)/input/keyboard ; then mkdir -p $(DESTDIR)$(CONFIGDIR)/input/keyboard ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/input/keyboard/audio ; then $(INSTALL) -c -m 644 cfg/input/keyboard/audio $(DESTDIR)$(CONFIGDIR)/input/keyboard/audio ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/input/keyboard/audio_extend ; then $(INSTALL) -c -m 644 cfg/input/keyboard/audio_extend $(DESTDIR)$(CONFIGDIR)/input/keyboard/audio_extend ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/input/keyboard/default ; then $(INSTALL) -c -m 644 cfg/input/keyboard/default $(DESTDIR)$(CONFIGDIR)/input/keyboard/default ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/input/keyboard/movie ; then $(INSTALL) -c -m 644 cfg/input/keyboard/movie $(DESTDIR)$(CONFIGDIR)/input/keyboard/movie ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/input/keyboard/pictures ; then $(INSTALL) -c -m 644 cfg/input/keyboard/pictures $(DESTDIR)$(CONFIGDIR)/input/keyboard/pictures ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/input/keyboard/python ; then $(INSTALL) -c -m 644 cfg/input/keyboard/python $(DESTDIR)$(CONFIGDIR)/input/keyboard/python ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/input/keyboard/playlist ; then $(INSTALL) -c -m 644 cfg/input/keyboard/playlist $(DESTDIR)$(CONFIGDIR)/input/keyboard/playlist ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/input/keyboard/search ; then $(INSTALL) -c -m 644 cfg/input/keyboard/search $(DESTDIR)$(CONFIGDIR)/input/keyboard/search ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/input/keyboard/alarm ; then $(INSTALL) -c -m 644 cfg/input/keyboard/alarm $(DESTDIR)$(CONFIGDIR)/input/keyboard/alarm ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/input/keyboard/epg ; then $(INSTALL) -c -m 644 cfg/input/keyboard/epg $(DESTDIR)$(CONFIGDIR)/input/keyboard/epg ; fi
	if test ! -f $(DESTDIR)$(CONFIGDIR)/input/keyboard/vbox ; then $(INSTALL) -c -m 644 cfg/input/keyboard/vbox $(DESTDIR)$(CONFIGDIR)/input/keyboard/vbox ; fi

ifeq ($(LIRC),yes)
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/LircConfig ; then $(INSTALL) -c -m 644 cfg/LircConfig $(DESTDIR)$(CONFIGDIR)/LircConfig ; fi
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/RemoteConfig ; then $(INSTALL) -c -m 644 cfg/RemoteConfig $(DESTDIR)$(CONFIGDIR)/RemoteConfig ; fi
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/lircrc.example ; then $(INSTALL) -c -m 644 cfg/lircrc.example $(DESTDIR)$(CONFIGDIR)/lircrc.example ; fi
	@if test ! -d $(DESTDIR)$(CONFIGDIR)/input/lirc ; then mkdir -p $(DESTDIR)$(CONFIGDIR)/input/lirc ; fi
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/input/lirc/audio ; then $(INSTALL) -c -m 644 cfg/input/lirc/audio $(DESTDIR)$(CONFIGDIR)/input/lirc/audio ; fi
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/input/lirc/audio_extend ; then $(INSTALL) -c -m 644 cfg/input/lirc/audio_extend $(DESTDIR)$(CONFIGDIR)/input/lirc/audio_extend ; fi
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/input/lirc/default ; then $(INSTALL) -c -m 644 cfg/input/lirc/default $(DESTDIR)$(CONFIGDIR)/input/lirc/default ; fi
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/input/lirc/movie ; then $(INSTALL) -c -m 644 cfg/input/lirc/movie $(DESTDIR)$(CONFIGDIR)/input/lirc/movie ; fi
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/input/lirc/pictures ; then $(INSTALL) -c -m 644 cfg/input/lirc/pictures $(DESTDIR)$(CONFIGDIR)/input/lirc/pictures ; fi
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/input/lirc/python ; then $(INSTALL) -c -m 644 cfg/input/lirc/python $(DESTDIR)$(CONFIGDIR)/input/lirc/python ; fi
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/input/lirc/playlist ; then $(INSTALL) -c -m 644 cfg/input/lirc/playlist $(DESTDIR)$(CONFIGDIR)/input/lirc/playlist ; fi
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/input/lirc/search ; then $(INSTALL) -c -m 644 cfg/input/lirc/search $(DESTDIR)$(CONFIGDIR)/input/lirc/search ; fi
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/input/lirc/epg ; then $(INSTALL) -c -m 644 cfg/input/lirc/epg $(DESTDIR)$(CONFIGDIR)/input/lirc/epg ; fi
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/input/lirc/alarm ; then $(INSTALL) -c -m 644 cfg/input/lirc/alarm $(DESTDIR)$(CONFIGDIR)/input/lirc/alarm ; fi
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/input/lirc/vbox ; then $(INSTALL) -c -m 644 cfg/input/lirc/vbox $(DESTDIR)$(CONFIGDIR)/input/lirc/vbox ; fi
endif
ifeq ($(EVDEV),yes)
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/EvdevConfig ; then $(INSTALL) -c -m 644 cfg/EvdevConfig $(DESTDIR)$(CONFIGDIR)/EvdevConfig ; fi
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/RemoteConfig ; then $(INSTALL) -c -m 644 cfg/RemoteConfig $(DESTDIR)$(CONFIGDIR)/RemoteConfig ; fi
	@if test ! -d $(DESTDIR)$(CONFIGDIR)/input/evdev ; then mkdir -p $(DESTDIR)$(CONFIGDIR)/input/evdev ; fi
	@if test ! -f $(DESTDIR)$(CONFIGDIR)/input/evdev/default ; then $(INSTALL) -c -m 644 cfg/input/evdev/default $(DESTDIR)$(CONFIGDIR)/input/evdev/default ; fi
endif
	@if test ! -f $(DESTDIR)$(LIBDIR)/mms/gen_tvlisting.sh ; then $(INSTALL) -c -m 644 tools/gen_tvlisting.sh $(DESTDIR)$(LIBDIR)/mms/gen_tvlisting.sh ; fi
	@if test ! -d $(DESTDIR)$(PLUGINDIR) ; then mkdir -p $(DESTDIR)$(PLUGINDIR) ; fi
        # clean up the plugins dir as there might be wrong versions of dynamic libraries
	@if test -f $(DESTDIR)$(PLUGINDIR)/lib_ainput_keyboard.so ; then rm $(DESTDIR)$(PLUGINDIR)/* ; fi
	$(MAKE) -C plugins install;

ifeq ($(MUSIC),yes)
	$(MAKE) -C audio-library install;
endif
ifeq ($(MOVIE),yes)
	$(MAKE) -C movie-library install;
endif
ifeq ($(PICTURE),yes)
	$(MAKE) -C pic-library install;
endif

        # create cache dir
	if test ! -d $(DESTDIR)/var/cache/mms ; then mkdir -m 777 -p $(DESTDIR)/var/cache/mms ; fi

	@echo " Installation complete!"
	@echo ""
	@echo " have fun :)"

uninstall:
	@if test ! -f $(BINDIR)/$(PRG) ; then rm -f $(BINDIR)/$(PRG) ; fi
	@rm $(DESTDIR)$(PLUGINDIR)/*
	@echo " Uninstall completed"

clean:
	@if test -f simplefile.o ; then rm *.o ; fi
	@if test -f mms ; then rm mms ; fi
	@for i in $(SUBDIRS); do $(MAKE) -C $$i clean; done

distclean:
	$(MAKE) clean
	@for i in `find ./ -name .depend`; do rm -f $$i; done
	@rm -f config.h
	@rm -f config.mak
	@touch common.mak
	@touch config.mak
	@if test -f version.h ; then rm version.h ; fi
# in case configure failed for some reason
	@if test -f test.o ; then rm test.o ; fi
	$(EXTRA_DISTCLEAN)

dep: depend

depend:
	./version.sh
	make -C plugins depend
	$(CXX) -MM $(CXXFLAGS) $(SRCS) 1> .depend

# dependencies
ifneq ($(wildcard .depend),)
include .depend
endif
