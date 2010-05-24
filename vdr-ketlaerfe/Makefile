TOP = ..

include $(TOP)/Make.inc

LIBS = -L$(KETLAER)/lib -lketlaer -pthread -lz

OBJS = vdr-ketlaerfe.o libfe.o

PROG = vdr-ketlaerfe

all : $(PROG)

$(PROG) : $(OBJS)
	$(CXX) -g -o $(PROG) $(OBJS) $(LIBS)

install : all
	cp $(PROG) $(KETLAER)/bin

clean:
	rm -f *.o *~ $(PROG)
