TOP=../

include $(TOP)/Make.inc

OBJSONLY = HDMI_thread.o firmware_debug.o

OBJS = ConfigFile.o hdmihelper.o RemoteMap.o SetupClass.o

LIB=libketlaer.a

all : $(LIB)

$(LIB) : $(OBJS) $(OBJSONLY)
	rm -f $(LIB)
	$(AR) cq $(LIB) $(OBJS) $(OBJSONLY)

clean:
	rm -f $(LIB) $(OBJS)
	rm -f *~
