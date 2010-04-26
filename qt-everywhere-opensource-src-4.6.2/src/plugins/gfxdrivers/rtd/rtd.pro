TARGET = qscreenrtd
include(../../qpluginbase.pri)

DEFINES	+= QT_QWS_RTD QT_QWS_MOUSE_RTD QT_QWS_KBD_RTD

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/gfxdrivers

HEADERS = \
	qscreenrtd_qws.h \
	qkbdrtd_qws.h \
	qmousertd_qws.h

SOURCES	= main.cpp \
	qscreenrtd_qws.cpp \
	qkbdrtd_qws.cpp \
	qmousertd_qws.cpp

target.path += $$[QT_INSTALL_PLUGINS]/gfxdrivers
INSTALLS += target



KETLAERDIR=$$QT_BUILD_TREE/..
INCLUDEPATH=$$KETLAERDIR/RebuildedInclude

LIBS  = -L$$KETLAERDIR/AsusLIB -L$$KETLAERDIR/AsusRPC

LIBS += -Wl,--start-group

LIBS += firmware_debug.o
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

