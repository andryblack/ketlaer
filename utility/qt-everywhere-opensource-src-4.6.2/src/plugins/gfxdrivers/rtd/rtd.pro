TARGET = qgfxrtd
include(../../qpluginbase.pri)

DEFINES	+= QT_QWS_RTD

INCLUDEPATH += $(KETLAER_DIR)/RebuildedInclude $(KETLAER_DIR)/libketlaer

LIBS = -L$(KETLAER_DIR)/AsusLIB -L$(KETLAER_DIR)/AsusRPC

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
LIBS += -lCSS_344 -lHDMIControl -lTimerSubsystem
LIBS += -lsmw -lVDecoderMPEG2 -lFilterCommon -lstream
LIBS += -lAEncoderSystem-rpc -lVEncoderSystem-rpc
LIBS += -lAudioInFilter-rpc -lVideoInSystem-rpc
LIBS += $(KETLAER_DIR)/libketlaer/libketlaer.a
LIBS += $(KETLAER_DIR)/utility/config/lib/.libs/libconfig.a

LIBS += -Wl,--end-group


QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/gfxdrivers

HEADERS = qkbdrtd_qws.h qmousertd_qws.h qrtdhdr.h qscreenrtd_qws.h
SOURCES = main.cpp qkbdrtd_qws.cpp qmousertd_qws.cpp qscreenrtd_qws.cpp

target.path += $$[QT_INSTALL_PLUGINS]/gfxdrivers
INSTALLS += target
