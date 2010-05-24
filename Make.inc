TOOLCHAIN = /usr/local/toolchain_mipsel/bin

CC = $(TOOLCHAIN)/mipsel-linux-gcc
CXX = $(TOOLCHAIN)/mipsel-linux-g++
AR = $(TOOLCHAIN)/mipsel-linux-ar

LIBKETLAER=$(TOP)/libketlaer/libketlaer.a \
	   $(TOP)/utility/config/lib/.libs/libconfig.a

INCLUDES = -I $(TOP)/RebuildedInclude -I $(TOP)/libketlaer \
           -I $(TOP)/utility/config/lib

LDFLAGS = -L $(TOP)/AsusLIB -L $(TOP)/AsusRPC


CFLAGS   = -mips32 -O2 $(INCLUDES) -fPIC
CXXFLAGS = -mips32 -O2 $(INCLUDES) -fPIC

