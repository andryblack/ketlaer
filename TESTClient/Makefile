TOOLCHAIN_DIR=/usr/local/toolchain_mipsel/bin/
GPP=$(TOOLCHAIN_DIR)mipsel-linux-g++
STRIP=$(TOOLCHAIN_DIR)mipsel-linux-strip
PROGNAME=tclient

TOP=..

INCS = -I $(TOP)/RebuildedInclude -I $(TOP)/include -I $(TOP)/Utility/config/lib

LIBS  = -L $(TOP)/AsusLIB -L $(TOP)/AsusRPC -L $(TOP)/Utility/config/lib/.libs
LIBS += -Wl,--start-group

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
LIBS += -lconfig++
LIBS += -Wl,--end-group


OBJS = hello.o hdmihelper.o SetupClass.o ConfigFile.o RemoteMap.o

$(PROGNAME) : $(OBJS)
	$(GPP) -g -static $(OBJS) firmware_debug.o HDMI_thread.o -o $(PROGNAME) $(LIBS)


hello.o : hello.cpp
	$(GPP) -g $(INCS) -c hello.cpp

hdmihelper.o : hdmihelper.cpp
	$(GPP) -g $(INCS) -c hdmihelper.cpp

SetupClass.o : SetupClass.cpp
	$(GPP) -g $(INCS) -c SetupClass.cpp

ConfigFile.o : ConfigFile.cpp 
	$(GPP) -g $(INCS) -c ConfigFile.cpp

RemoteMap.o : RemoteMap.cpp
	$(GPP) -g $(INCS) -c RemoteMap.cpp

striped: $(PROGNAME)
	$(STRIP) $(PROGNAME)

clean : 
	rm -f $(OBJS) $(PROGNAME) 
	rm -f *~
