#!/usr/bin/env python

"""PyQt4 port of the richtext/textobject example from Qt v4.x"""

# This is only needed for Python v2 but is harmless for Python v3.
import sip
sip.setapi('QVariant', 2)

from PyQt4 import QtCore, QtGui, QtSvg


class SvgTextObject(QtGui.QPyTextObject):
    def intrinsicSize(self, doc, posInDocument, format):
        renderer = QtSvg.QSvgRenderer(format.property(Window.SvgData))
        size = renderer.defaultSize()

        if size.height() > 25:
            size *= 25.0 / size.height()

        return QtCore.QSizeF(size)

    def drawObject(self, painter, rect, doc, posInDocument, format):
        renderer = QtSvg.QSvgRenderer(format.property(Window.SvgData))
        renderer.render(painter, rect)


class Window(QtGui.QWidget):

    SvgTextFormat = QtGui.QTextFormat.UserObject + 1

    SvgData = 1

    def __init__(self):
        super(Window, self).__init__()

        self.setupGui()
        self.setupTextObject()

        self.setWindowTitle("Text Object Example")

    def insertTextObject(self):
        fileName = self.fileNameLineEdit.text()
        file = QtCore.QFile(fileName)

        if not file.open(QtCore.QIODevice.ReadOnly):
            QtGui.QMessageBox.warning(self, "Error Opening File",
                    "Could not open '%s'" % fileName)

        svgData = file.readAll()

        svgCharFormat = QtGui.QTextCharFormat()
        svgCharFormat.setObjectType(Window.SvgTextFormat)
        svgCharFormat.setProperty(Window.SvgData, svgData)

        try:
            # Python v2.
            orc = unichr(0xfffc)
        except NameError:
            # Python v3.
            orc = chr(0xfffc)

        cursor = self.textEdit.textCursor()
        cursor.insertText(orc, svgCharFormat)
        self.textEdit.setTextCursor(cursor)

    def setupTextObject(self):
        svgInterface = SvgTextObject(self)
        self.textEdit.document().documentLayout().registerHandler(Window.SvgTextFormat, svgInterface)

    def setupGui(self):
        fileNameLabel = QtGui.QLabel("Svg File Name:")
        self.fileNameLineEdit = QtGui.QLineEdit()
        insertTextObjectButton = QtGui.QPushButton("Insert Image")

        self.fileNameLineEdit.setText('./files/heart.svg')
        insertTextObjectButton.clicked.connect(self.insertTextObject)

        bottomLayout = QtGui.QHBoxLayout()
        bottomLayout.addWidget(fileNameLabel)
        bottomLayout.addWidget(self.fileNameLineEdit)
        bottomLayout.addWidget(insertTextObjectButton)

        self.textEdit = QtGui.QTextEdit()

        mainLayout = QtGui.QVBoxLayout()
        mainLayout.addWidget(self.textEdit)
        mainLayout.addLayout(bottomLayout)

        self.setLayout(mainLayout)


if __name__ == '__main__':

    import sys

    app = QtGui.QApplication(sys.argv)
    window = Window()
    window.show()
    sys.exit(app.exec_())
