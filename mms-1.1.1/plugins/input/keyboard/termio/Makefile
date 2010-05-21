CC = mipsel-linux-gcc

CFLAGS = -Wall

.SUFFIXES: .c .o

.c.o:
	$(CC) -c $(CFLAGS) -fPIC -o $@ $<

all: getch2.o

clean: 
	@if test -f getch2.o ; then rm getch2.o ; fi
