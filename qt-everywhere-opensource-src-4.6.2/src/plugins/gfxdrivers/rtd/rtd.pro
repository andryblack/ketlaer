TARGET = qgfxrtd
include(../../qpluginbase.pri)

DEFINES	+= QT_QWS_RTD

INCLUDEPATH += $(KETLAER_DIR)/RebuildedInclude $(KETLAER_DIR)/libketlaer

LIBS += \ 
	-L$(KETLAER_DIR)/AsusLIB \
        -L$(KETLAER_DIR)/AsusRPC \
        -L$(KETLAER_DIR)/libketlaer \
        -L$(KETLAER_DIR)/Utility/config/lib/.libs \
        -L$(KETLAER_DIR)/libm2 \
        -Wl,--start-group \
        -lVoutUtil \
        -lDirectGraphics -lMoveDataEngine -lStreamingEngine \
        -lDrawScale -lBoard -lGPIO -lHDMIControl -lPlatformCommon \
        -lOSAL -lsystemrpc -lsystemvideorpc -lRPC -lrtxdr \
        -lPlaybackAppClass -lSiDvb -lSiDvbAgent -lNavigationFilter -lCF \
        -lUtil -lAudioOut -lDecoderSPU -lplaybackflow -lAudioDecoder \
        -lResource -lSecurity \
        -lInputAIFF -lInputASF -lInputAudioES -lInputAVI -lInputBD \
        -lInputDTV -lInputDVDVideo -lInputDVDVR -lInputFLV -lInputLPCM \
        -lInputMKV -lInputMP4 -lInputMPEG -lInputNetwork -lInputOGG \
        -lInputPlaylist -lInputRM -lInputVCD -lInputVideoES -lInputWAVE \
        -lDemuxMPEGProgram -lDemuxMPEGTransport -lDemuxPassthrough \
        -lIOPluginCustom1 -lIOPluginCustom2 -lIOPluginCustom3 \
        -lIOPluginFileLink -lFileIOPlugin -lHttpIOPlugin -lWMDRMPD \
        -lAudioUtil -lGeneralIFO -lfileaccess -ldvdaccess -lDVDCommon \
        -lvrauthoring -lebase -ltinyxml -ludfaccess -lneon -lFastCopy \
        -lCSS_344 -lz -lHDMIControl -lTimerSubsystem \
        -lsmw -lVDecoderMPEG2 -lFilterCommon -lstream \
        -lAEncoderSystem-rpc -lVEncoderSystem-rpc \
        -lAudioInFilter-rpc -lVideoInSystem-rpc \
        -lketlaer -lconfig -lm2 -ldl \
        -Wl,--end-group

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/gfxdrivers

HEADERS = qkbdrtd_qws.h qmousertd_qws.h qrtdhdr.h qscreenrtd_qws.h
SOURCES = main.cpp qkbdrtd_qws.cpp qmousertd_qws.cpp qscreenrtd_qws.cpp

target.path += $$[QT_INSTALL_PLUGINS]/gfxdrivers
INSTALLS += target
