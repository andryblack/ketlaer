import sys, re
from PyQt4 import QtGui, QtCore, QtNetwork

class StatusMonitor(QtCore.QThread):
    def __init__(self, pbc, window):
        super(StatusMonitor, self).__init__()
        
        self.pbc = pbc
        self.window = window
        self.running = True

    def run(self):
        while self.running:
            res = self.pbc.GetEvent(100)
            event = re.split(":", res)
            if event[1] == "0":
                self.window.playbackEvent(event)
                self.pbc.FreeEvent(event[2])

    def quit(self):
        self.running = False

    def exit(self, code):
        self.running = False


class PlaybackController(QtCore.QObject):
    def __init__(self, window):
        super(PlaybackController, self).__init__()
        
        self.ExecCmd("Version 1")
        self.monitor = StatusMonitor(self, window)
        self.window = window
        self.playing = False

    def ExecCmd(self, cmd):
        sock = QtNetwork.QLocalSocket()
        sock.connectToServer(".playback")
        sock.waitForConnected()
        sock.writeData(cmd+"\n")
        sock.waitForBytesWritten()
        sock.waitForReadyRead()
        return sock.readLine()

    def GetReservedColorKey(self):
        res = self.ExecCmd("GetReservedColorKey")
        return int(re.split(":", res)[1])

    def LoadMedia(self, str):
        self.monitor.start()
        res = self.ExecCmd("LoadMedia "+str)
        return int(re.split(":", res)[1])
       
    def Play(self):
        self.playing = True
        res = self.ExecCmd("Play")
        return int(re.split(":", res)[1])
    
    def Pause(self):
        self.playing = False
        res = self.ExecCmd("Pause")
        return int(re.split(":", res)[1])
    
    def TogglePlayPause(self):
        if self.playing:
            return self.Pause()
        else:
            return self.Play()
        
    def Stop(self):
        self.playing = False
        res = self.ExecCmd("Stop")
        return int(re.split(":", res)[1])

    def GetEvent(self, timeoutms):
        return self.ExecCmd("GetEvent "+str(timeoutms))
            
    def FreeEvent(self, evid):
        res = self.ExecCmd("FreeEvent "+evid)
        return int(re.split(":", res)[1])


class PlaybackWindow(QtGui.QWidget):
    def __init__(self, medialist, parent=None):
        super(PlaybackWindow, self).__init__(parent, QtCore.Qt.FramelessWindowHint)

        screen = QtGui.QDesktopWidget().screenGeometry();
	self.move(0, 0)
        self.resize(screen.width(), screen.height());
        self.setWindowTitle('PlaybackWindow')
        self.pbc = PlaybackController(self)
        self.colorkey = self.pbc.GetReservedColorKey()
        self.setBackgroundRole(QtGui.QPalette.Window)
        palette = QtGui.QPalette();
        palette.setColor(QtGui.QPalette.Window, QtGui.QColor(self.colorkey))
        self.setPalette(palette)
        self.medialist = medialist
        self.mediaidx = -1
        self.playNextMedia()

    def playNextMedia(self):
        self.mediaidx += 1
        if self.mediaidx >= len(self.medialist):
            self.pbc.Stop()
            self.close()
        else:
            self.pbc.LoadMedia(self.medialist[self.mediaidx])
            self.pbc.Play()

    def playPreviousMedia(self):
        if self.mediaidx > 0:
            self.mediaidx -= 1
            self.pbc.LoadMedia(self.medialist[self.mediaidx])
            self.pbc.Play()

    def playbackEvent(self, event):
        code = event[4]
        print "playback:", code
        if code == "FE_Playback_Completed":
            self.playNextMedia()
        elif code == "FE_Playback_FatalError":
            self.playNextMedia()

    def keyPressEvent(self, event):
        key = event.key()
        print "key:", key
        if key == QtCore.Qt.Key_MediaPlay or key == QtCore.Qt.Key_Space:
            self.pbc.TogglePlayPause()
        elif key == QtCore.Qt.Key_MediaStop or key == QtCore.Qt.Key_S:
            self.pbc.Stop()
        elif key == QtCore.Qt.Key_NextMedia or key == QtCore.Qt.Key_N:
            self.playNextMedia()
        elif key == QtCore.Qt.Key_PrevMedia or key == QtCore.Qt.Key_P:
            self.playPreviousMedia()
            

