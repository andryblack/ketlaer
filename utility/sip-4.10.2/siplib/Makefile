TARGET = sip.so
OFILES = siplib.o apiversions.o descriptors.o qtlib.o threads.o objmap.o voidptr.o bool.o
HFILES = sip.h sipint.h 

CC = gcc
CXX = g++
LINK = g++
CPPFLAGS = -DNDEBUG -I. -I/usr/local/include/python2.6
CFLAGS = -pipe -fPIC -O2 -w
CXXFLAGS = -pipe -fPIC -O2 -w
LFLAGS = -shared -Wl,--version-script=sip.exp
LIBS = 
.SUFFIXES: .c .o .cpp .cc .cxx .C


.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

all: $(TARGET)

$(OFILES): $(HFILES)

$(TARGET): $(OFILES)
	@echo '{ global: initsip; local: *; };' > sip.exp
	$(LINK) $(LFLAGS) -o $(TARGET) $(OFILES) $(LIBS)

install: $(TARGET)
	@test -d $(DESTDIR)/usr/local/lib/python2.6/site-packages || mkdir -p $(DESTDIR)/usr/local/lib/python2.6/site-packages
	cp -f $(TARGET) $(DESTDIR)/usr/local/lib/python2.6/site-packages/$(TARGET)
	strip $(DESTDIR)/usr/local/lib/python2.6/site-packages/$(TARGET)
	@test -d $(DESTDIR)/usr/local/include/python2.6 || mkdir -p $(DESTDIR)/usr/local/include/python2.6
	cp -f /root/src/ketlaer/utility/sip-4.10.2/siplib/sip.h $(DESTDIR)/usr/local/include/python2.6/sip.h

clean:
	-rm -f $(TARGET)
	-rm -f siplib.o
	-rm -f apiversions.o
	-rm -f descriptors.o
	-rm -f qtlib.o
	-rm -f threads.o
	-rm -f objmap.o
	-rm -f voidptr.o
	-rm -f bool.o
	-rm -f sip.exp
