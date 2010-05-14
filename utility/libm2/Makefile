TOP=../..

include $(TOP)/Make.inc


OBJS  = s_lround.o s_lroundf.o s_round.o s_roundf.o s_trunc.o s_truncf.o
OBJS += s_llround.o s_llroundf.o

LIB=libm2.a

all : $(LIB)

$(LIB) : $(OBJS)
	rm -f $(LIB)
	$(AR) cq $(LIB) $(OBJS)

clean:
	rm -f $(LIB) $(OBJS)
	rm -f *~
