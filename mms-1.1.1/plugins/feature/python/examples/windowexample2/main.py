import os
import mmsv2gui

class Windowexample2(mmsv2gui.Window):
	def __init__(self, xmlfile):
		mmsv2gui.Window.__init__(self, xmlfile)

	def onClick(self, control):
		print "Control got clicked"
		print control

	def onFocus(self, control):
                print "Control got focus" 
		print control

win = Windowexample2("window.xml")
win.doModal()

del win
