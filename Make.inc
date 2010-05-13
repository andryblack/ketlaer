TOOLCHAIN = /usr/local/toolchain_mipsel/bin

CC = $(TOOLCHAIN)/mipsel-linux-gcc
CXX = $(TOOLCHAIN)/mipsel-linux-g++
AR = $(TOOLCHAIN)/mipsel-linux-ar

LIBKETLAER=$(TOP)/libketlaer/libketlaer.a \
	   $(TOP)/Utility/config/lib/.libs/libconfig.a

INCLUDES = -I $(TOP)/RebuildedInclude -I $(TOP)/libketlaer \
           -I $(TOP)/Utility/config/lib

LDFLAGS = -L $(TOP)/AsusLIB -L $(TOP)/AsusRPC


CFLAGS = $(INCLUDES) 
CXXFLAGS = $(INCLUDES) --no-rtti --no-exceptions

