from PyQt4 import QtCore, QtGui

class MainMenu(QtGui.QWidget):
    def __init__(self, parent=None):
        super(MainMenu, self).__init__(parent, QtCore.Qt.FramelessWindowHint)

        screen = QtGui.QDesktopWidget().screenGeometry();
	self.move(0, 0)
        self.resize(screen.width(), screen.height());
        self.setWindowTitle('MainMenu')
        #self.setBackgroundRole(QtGui.QPalette.Window)
        #palette = QtGui.QPalette();
        #palette.setColor(QtGui.QPalette.Window, QtGui.QColor(self.colorkey))
        #self.setPalette(palette)

    def keyPressEvent(self, event):
        key = event.key()
        print "key:", key
        if key == QtCore.Qt.Key_Escape:
            self.close()

    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        painter.drawText(50, 50, "Hello, World!")


