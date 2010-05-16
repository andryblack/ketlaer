import sys, playback
from PyQt4 import QtGui

medialist = [ "file:///tmp/DATA/tmp/test.mpg", "file:///tmp/DATA/media/video/kinder/lights.wmv" ]

app = QtGui.QApplication(sys.argv)
sys.argv = sys.argv[1:]
pw = playback.PlaybackWindow(medialist)
pw.show()
sys.exit(app.exec_())
