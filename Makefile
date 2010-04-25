TOOLCHAIN_DIR=/usr/local/toolchain_mipsel/bin/
GPP=$(TOOLCHAIN_DIR)mipsel-linux-g++
STRIP=$(TOOLCHAIN_DIR)mipsel-linux-strip

INCS  = -I RebuildedInclude 

LIBS  = -L AsusLIB -L AsusRPC
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
LIBS += -lCSS_344 -lz -lHDMIControl
LIBS += -lsmw -lVDecoderMPEG2 -lFilterCommon -lstream
LIBS += -lAEncoderSystem-rpc -lVEncoderSystem-rpc
LIBS += -lAudioInFilter-rpc -lVideoInSystem-rpc
LIBS += -lpthread -ldl

LIBS += -Wl,--end-group

OBJS = hello.o firmware_debug.o SetupClass.o 

hello : $(OBJS)
	$(GPP) -g -static $(OBJS) -o hello $(LIBS)


hello.o : hello.cpp
	$(GPP) -g $(INCS) -c hello.cpp

SetupClass.o : SetupClass.cpp
	$(GPP) -g $(INCS) -c SetupClass.cpp

clean : 
		rm -f SetupClass.o hello.o hello