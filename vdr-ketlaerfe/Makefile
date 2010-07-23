TOP = ..

include $(TOP)/Make.inc

LIBS = -L$(KETLAER)/lib -lketlaer -pthread -lz

LIBS_STATIC  = -Wl,--start-group
LIBS_STATIC += -L$(TOP)/AsusLIB -L$(TOP)/AsusRPC
LIBS_STATIC += -L$(TOP)/utility/config/lib/.libs -lconfig
LIBS_STATIC += -L$(TOP)/libketlaer -lketlaer
LIBS_STATIC += -lPlaybackAppClass -lVoutUtil
LIBS_STATIC += -lDirectGraphics -lStreamingEngine
LIBS_STATIC += -lDrawScale 
LIBS_STATIC += -lNavigationFilter -lCF
LIBS_STATIC += -lAudioOut -lDecoderSPU -lplaybackflow -lAudioDecoder
LIBS_STATIC += -lResource 
LIBS_STATIC += -lDemuxMPEGProgram -lDemuxMPEGTransport -lDemuxPassthrough
LIBS_STATIC += -lIOPluginCustom1 -lIOPluginCustom2 -lIOPluginCustom3
LIBS_STATIC += -lIOPluginFileLink -lFileIOPlugin -lHttpIOPlugin 
LIBS_STATIC += -lAudioUtil -lfileaccess 
LIBS_STATIC += -ltinyxml -lneon -lFastCopy
LIBS_STATIC += -lTimerSubsystem
LIBS_STATIC += -lHDMIControl
LIBS_STATIC += -lSecurity
LIBS_STATIC += -lsystemvideorpc -lsystemrpc
LIBS_STATIC += -lVDecoderMPEG2 -lFilterCommon -lstream 
LIBS_STATIC += -lBoard -lPlatformCommon -lGPIO -lOSAL -lMoveDataEngine
LIBS_STATIC += -lInputAIFF -lInputASF -lInputAudioES -lInputAVI -lInputBD
LIBS_STATIC += -lInputDTV -lInputDVDVideo -lInputDVDVR -lInputFLV -lInputLPCM
LIBS_STATIC += -lInputMKV -lInputMP4 -lInputMPEG -lInputNetwork -lInputOGG
LIBS_STATIC += -lInputPlaylist -lInputRM -lInputVCD -lInputVideoES -lInputWAVE
LIBS_STATIC += -lDVDCommon -lGeneralIFO -lWMDRMPD
LIBS_STATIC += -lSiDvb -lSiDvbAgent 
LIBS_STATIC += -lvrauthoring -ldvdaccess -ludfaccess -lCSS_344
LIBS_STATIC += -lebase -lvmgi -lUtil
LIBS_STATIC += -lAudioInFilter-rpc -lVideoInSystem-rpc
LIBS_STATIC += -lsmw 
LIBS_STATIC += -lAEncoderSystem-rpc -lVEncoderSystem-rpc
LIBS_STATIC += -lRPC -lrtxdr
LIBS_STATIC += -lz -lpthread -ldl
LIBS_STATIC += -Wl,--end-group

OBJS = vdr-ketlaerfe.o libfe.o

PROG          = vdr-ketlaerfe
PROG_STATIC   = vdr-ketlaerfe-static
PROG_ROOTAPP2 = rootapp2

all : $(PROG) $(PROG_STATIC) $(PROG_ROOTAPP2)

$(PROG) : $(OBJS)
	$(CXX) -g -o $(PROG) $(OBJS) $(LIBS)

$(PROG_STATIC) : $(OBJS)
	$(CXX) -static -o $(PROG_STATIC) $(OBJS) $(LIBS_STATIC)
	mipsel-linux-strip $(PROG_STATIC)

$(PROG_ROOTAPP2) : rootapp2.cpp
	$(CXX) -static -o $(PROG_ROOTAPP2) rootapp2.cpp
	mipsel-linux-strip $(PROG_ROOTAPP2)

install : all
	cp $(PROG)          $(KETLAER)/bin
	cp $(PROG_STATIC)   $(KETLAER)/bin
	cp $(PROG_ROOTAPP2) $(KETLAER)/bin

clean:
	rm -f *.o *~ 
	rm -f $(PROG) 
	rm -f $(PROG_STATIC)
	rm -f $(PROG_ROOTAPP2)
