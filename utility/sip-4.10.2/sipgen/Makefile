TARGET = sip
OFILES = main.o transform.o gencode.o export.o heap.o parser.o lexer.o
HFILES = sip.h parser.h

CC = gcc
CXX = g++
LINK = g++
CPPFLAGS = -DNDEBUG -I.
CFLAGS = -pipe -O2 -w
CXXFLAGS = -pipe -O2 -w
LFLAGS = 
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
	$(LINK) $(LFLAGS) -o $(TARGET) $(OFILES) $(LIBS)

install: $(TARGET)
	@test -d $(DESTDIR)/usr/local/bin || mkdir -p $(DESTDIR)/usr/local/bin
	cp -f $(TARGET) $(DESTDIR)/usr/local/bin/$(TARGET)

clean:
	-rm -f $(TARGET)
	-rm -f main.o
	-rm -f transform.o
	-rm -f gencode.o
	-rm -f export.o
	-rm -f heap.o
	-rm -f parser.o
	-rm -f lexer.o
