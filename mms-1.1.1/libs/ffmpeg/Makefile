### The directory environment:

MMSDIR = ../../

### Includes and Defines

include $(MMSDIR)/config.mak
include $(MMSDIR)/common.mak

ifneq ($(FFMPEG_EXTERNAL), yes)
SUBDIRS += libavcodec libavutil libavformat libswscale
EXTRA_DISTCLEAN = rm -f *.pc && rm -f config.*
endif

all:
	$(DO_MAKE)

clean:
	$(DO_MAKECLEAN)

distclean:
	$(MAKE) clean
	$(EXTRA_DISTCLEAN)
