# Tell the original .pro file about additional directories.
INCLUDEPATH = /usr/local/etc/ketlaer/include/python2.6 ../../QtCore /root/src/ketlaer/utility/pyqt-4.7.3/qpy/QtCore
CONFIG += release
VPATH = /root/src/ketlaer/utility/pyqt-4.7.3/qpy/QtCore
include(/root/src/ketlaer/utility/pyqt-4.7.3/qpy/QtCore/qpycore.pro)
