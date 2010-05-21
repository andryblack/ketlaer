import sys, playback, mainmenu
from PyQt4 import QtGui

medialist = [ "file:///tmp/DATA/tmp/test.mpg" ]

app = QtGui.QApplication(sys.argv)
sys.argv = sys.argv[1:]
#w = mainmenu.MainMenu()
w = playback.PlaybackWindow(medialist)
w.show()
sys.exit(app.exec_())
