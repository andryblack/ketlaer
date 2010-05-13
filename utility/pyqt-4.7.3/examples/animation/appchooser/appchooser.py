#!/usr/bin/env python


from PyQt4 import QtCore, QtGui

import appchooser_rc


class Pixmap(QtGui.QGraphicsWidget):
    clicked = QtCore.pyqtSignal()

    def __init__(self, pix, parent=None):
        super(Pixmap, self).__init__(parent)

        self.orig = QtGui.QPixmap(pix)
        self.p = QtGui.QPixmap(pix)

    def paint(self, painter, option, widget):
        painter.drawPixmap(QtCore.QPointF(), self.p)

    def mousePressEvent(self, ev):
        self.clicked.emit()

    def setGeometry(self, rect):
        super(Pixmap, self).setGeometry(rect)

        if rect.size().width() > self.orig.size().width():
            self.p = self.orig.scaled(rect.size().toSize())
        else:
            self.p = QtGui.QPixmap(self.orig)


def createStates(objects, selectedRect, parent):
    for obj in objects:
        state = QtCore.QState(parent)
        state.assignProperty(obj, 'geometry', selectedRect)
        parent.addTransition(obj.clicked, state)


def createAnimations(objects, machine):
    for obj in objects:
        animation = QtCore.QPropertyAnimation(obj, 'geometry', obj)
        machine.addDefaultAnimation(animation)


if __name__ == '__main__':

    import sys

    app = QtGui.QApplication(sys.argv)

    p1 = Pixmap(QtGui.QPixmap(':/digikam.png'))
    p2 = Pixmap(QtGui.QPixmap(':/akregator.png'))
    p3 = Pixmap(QtGui.QPixmap(':/accessories-dictionary.png'))
    p4 = Pixmap(QtGui.QPixmap(':/k3b.png'))

    p1.setGeometry(QtCore.QRectF(0.0, 0.0, 64.0, 64.0))
    p2.setGeometry(QtCore.QRectF(236.0, 0.0, 64.0, 64.0))
    p3.setGeometry(QtCore.QRectF(236.0, 236.0, 64.0, 64.0))
    p4.setGeometry(QtCore.QRectF(0.0, 236.0, 64.0, 64.0))

    scene = QtGui.QGraphicsScene(0, 0, 300, 300)
    scene.setBackgroundBrush(QtCore.Qt.white)
    scene.addItem(p1)
    scene.addItem(p2)
    scene.addItem(p3)
    scene.addItem(p4)

    window = QtGui.QGraphicsView(scene)
    window.setFrameStyle(0)
    window.setAlignment(QtCore.Qt.AlignLeft | QtCore.Qt.AlignTop)
    window.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
    window.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)

    machine = QtCore.QStateMachine()
    machine.setGlobalRestorePolicy(QtCore.QStateMachine.RestoreProperties)

    group = QtCore.QState(machine)
    selectedRect = QtCore.QRect(86, 86, 128, 128)

    idleState = QtCore.QState(group)
    group.setInitialState(idleState)

    objects = [p1, p2, p3, p4]
    createStates(objects, selectedRect, group)
    createAnimations(objects, machine)

    machine.setInitialState(group)
    machine.start()

    window.resize(300, 300)
    window.show()

    sys.exit(app.exec_())
