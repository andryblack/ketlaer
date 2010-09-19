# Tell the original .pro file about additional directories.
INCLUDEPATH = /usr/local/etc/ketlaer/include/python2.6 /root/src/ketlaer/utility/pyqt-4.7.3/qpy/QtGui
CONFIG += release
VPATH = /root/src/ketlaer/utility/pyqt-4.7.3/qpy/QtGui
include(/root/src/ketlaer/utility/pyqt-4.7.3/qpy/QtGui/qpygui.pro)
