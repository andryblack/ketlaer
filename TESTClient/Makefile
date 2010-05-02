TOP=..

include $(TOP)/Make.inc

LIBS  = -Wl,--start-group

LIBS += -lVoutUtil
LIBS += -lDirectGraphics -lMoveDataEngine -lStreamingEngine
LIBS += -lDrawScale -lBoard -lGPIO -lHDMIControl -lPlatformCommon
LIBS += -lOSAL -lsystemrpc -lsystemvideorpc -lRPC -lrtxdr
LIBS += -lPlaybackAppClass -lSiDvb -lSiDvbAgent -lNavigationFilter -lCF
LIBS += -lUtil -lAudioOut -lDecoderSPU -lplaybackflow -lAudioDecoder
LIBS += -lResource -lSecurity
LIBS += -lInputAIFF -lInputASF -lInputAudioES -lInputAVI -lInputBD
LIBS += -lInputDTV -lInputDVDVideo -lInputDVDVR -lInputFLV -lInputLPCM
LIBS += -lInputMKV -lInputMP4 -lInputMPEG -lInputNetwork -lInputOGG
LIBS += -lInputPlaylist -lInputRM -lInputVCD -lInputVideoES -lInputWAVE
LIBS += -lDemuxMPEGProgram -lDemuxMPEGTransport -lDemuxPassthrough
LIBS += -lIOPluginCustom1 -lIOPluginCustom2 -lIOPluginCustom3
LIBS += -lIOPluginFileLink -lFileIOPlugin -lHttpIOPlugin -lWMDRMPD
LIBS += -lAudioUtil -lGeneralIFO -lfileaccess -ldvdaccess -lDVDCommon
LIBS += -lvrauthoring -lebase -ltinyxml -ludfaccess -lneon -lFastCopy
LIBS += -lCSS_344 -lz -lHDMIControl -lTimerSubsystem
LIBS += -lsmw -lVDecoderMPEG2 -lFilterCommon -lstream
LIBS += -lAEncoderSystem-rpc -lVEncoderSystem-rpc
LIBS += -lAudioInFilter-rpc -lVideoInSystem-rpc
LIBS += -lpthread -ldl
LIBS += $(LIBKETLAER)

LIBS += -Wl,--end-group

PROG = hello
OBJS = hello.o

$(PROG) : $(OBJS)
	$(CXX) $(LDFLAGS) -g -static $(OBJS) -o $(PROG) $(LIBS)


stripped: $(PROG)
	$(STRIP) $(PROG)

clean : 
	rm -f $(OBJS) $(PROG)
	rm -f *~
