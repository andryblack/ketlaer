
LIBTOOL=$(MMSDIR)/libtool-ketlaer
LIBTOOL_OPTS = --tag=CXX 

EXTRA_FLAGS += -ffast-math -funroll-loops -fomit-frame-pointer

ifeq ($(GCC_VER_MAJOR),3)
ifeq ($(GCC_VER_MINOR),4)
EXTRA_FLAGS += -ftracer -funit-at-a-time
endif
ifeq ($(GCC_VER_MINOR),3)
EXTRA_FLAGS += -ftracer
endif
endif

ifeq ($(DEBUG),yes)
# use -Wno-long-long to make taglib happy
EXTRA_FLAGS = -g -Wall -W -Wno-sign-compare -Wold-style-cast -Wundef -Wno-long-long
OPTIMIZATION = -mips32 -O2
else
ifeq ($(SIZE_OPTIMIZE),yes)
OPTIMIZATION = -mips32 -Os
else
OPTIMIZATION = -mips32 -O3
endif
endif

# Do not strip the binaries at installation
ifeq ($(DEBUG),yes)
INSTALLSTRIP =
else
INSTALLSTRIP = 
endif

KETLAER_INCS = -I$(KETLAER)/include -I$(KETLAER_DIR)/libketlaer
KETLAER_LIBS = -L$(KETLAER)/lib -lketlaer -lz -lpthread -ldl -lm

STDFLAGS = -I$(KETLAER)/include -I. \
	 -pthread -pipe $(OPTIMIZATION) $(EXTRA_FLAGS) $(OPTFLAGS)

COMMON_LIBS += $(KETLAER_LIBS)
COMMON_LIBS += $(COMMONCPP2_LIBS)
STDFLAGS += $(COMMONCPP2_CFLAGS)

CXXFLAGS = $(STDFLAGS)

CXXFLAGS += $(BOOST_INCLUDE)

CXX=mipsel-linux-g++

C=mipsel-linux-gcc

.SUFFIXES: .cpp .c .o
.cpp.o: ; $(CXX) $(CXXFLAGS) -c $<
.c.o: ; $(C) $(STDFLAGS) $(CFLAGS) -c $<

DO_MAKE = @for i in $(SUBDIRS); do $(MAKE) -C $$i || break; done
DO_MAKECLEAN = @find ./ -mindepth 2  -maxdepth 2 -name 'Makefile'  -type f | sed 's/[mM]akefile$$//g' | xargs -I '{}' make -C  '{}' clean


