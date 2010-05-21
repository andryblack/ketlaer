TARGET = qgfxrtd
include(../../qpluginbase.pri)

DEFINES	+= QT_QWS_RTD

INCLUDEPATH += $(KETLAER_DIR)/libketlaer

LIBS = -L$(KETLAER)/lib -lketlaer

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/gfxdrivers

HEADERS = qkbdrtd_qws.h qmousertd_qws.h qrtdhdr.h qscreenrtd_qws.h 
SOURCES = main.cpp qkbdrtd_qws.cpp qmousertd_qws.cpp qscreenrtd_qws.cpp

target.path += $$[QT_INSTALL_PLUGINS]/gfxdrivers
INSTALLS += target
