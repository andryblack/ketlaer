TOP=../

include $(TOP)/Make.inc

OBJSONLY = HDMI_thread.o firmware_debug.o

OBJS = init.o ConfigFile.o hdmihelper.o RemoteMap.o SetupClass.o

LIB=libketlaer.a

LIBSO=libketlaer.so

all : $(LIB) $(LIBSO) 

$(LIB) : $(OBJS) $(OBJSONLY)
	rm -f $(LIB)
	$(AR) cq $(LIB) $(OBJS) $(OBJSONLY)

LIBS  = -L$(TOP)/AsusLIB -L$(TOP)/AsusRPC
LIBS += -L$(TOP)/utility/config/lib/.libs -lconfig
LIBS += -lPlaybackAppClass -lVoutUtil
LIBS += -lDirectGraphics -lStreamingEngine
LIBS += -lDrawScale 
LIBS += -lNavigationFilter -lCF
LIBS += -lAudioOut -lDecoderSPU -lplaybackflow -lAudioDecoder
LIBS += -lResource 
LIBS += -lDemuxMPEGProgram -lDemuxMPEGTransport -lDemuxPassthrough
LIBS += -lIOPluginCustom1 -lIOPluginCustom2 -lIOPluginCustom3
LIBS += -lIOPluginFileLink -lFileIOPlugin -lHttpIOPlugin 
LIBS += -lAudioUtil -lfileaccess 
LIBS += -ltinyxml -lneon -lFastCopy
LIBS += -lTimerSubsystem
LIBS += -lHDMIControl
LIBS += -lSecurity
LIBS += -lsystemvideorpc -lsystemrpc
LIBS += -lVDecoderMPEG2 -lFilterCommon -lstream 
LIBS += -lBoard -lPlatformCommon -lGPIO -lOSAL -lMoveDataEngine
LIBS += -lInputAIFF -lInputASF -lInputAudioES -lInputAVI -lInputBD
LIBS += -lInputDTV -lInputDVDVideo -lInputDVDVR -lInputFLV -lInputLPCM
LIBS += -lInputMKV -lInputMP4 -lInputMPEG -lInputNetwork -lInputOGG
LIBS += -lInputPlaylist -lInputRM -lInputVCD -lInputVideoES -lInputWAVE
LIBS += -lDVDCommon -lGeneralIFO -lWMDRMPD
LIBS += -lSiDvb -lSiDvbAgent 
LIBS += -lvrauthoring -ldvdaccess -ludfaccess -lCSS_344
LIBS += -lebase -lvmgi -lUtil
LIBS += -lAudioInFilter-rpc -lVideoInSystem-rpc
LIBS += -lsmw 
LIBS += -lAEncoderSystem-rpc -lVEncoderSystem-rpc
LIBS += -lRPC -lrtxdr
LIBS += -lz -lpthread -ldl

$(LIBSO) : $(OBJS) $(OBJSONLY)
	$(CXX) -shared -o $(LIBSO) $(OBJS) $(OBJSONLY) $(LIBS)

install: all
	cp $(LIBSO) $(KETLAER)/lib

clean:
	rm -f $(LIB) $(LIBSO) $(OBJS)
	rm -f *~
