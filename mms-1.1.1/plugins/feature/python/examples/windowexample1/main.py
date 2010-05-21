import os
import mmsv2gui

class MyItem(mmsv2gui.ListItem):
	def __init__(self, label):
		mmsv2gui.ListItem.__init__(self)
		self.setLabel(label)

class Windowexample1(mmsv2gui.Window):
	def __init__(self):
		mmsv2gui.Window.__init__(self)
		self.bg = mmsv2gui.ImageControl(0, 0, self.getWidth(), self.getHeight(), "general/bg.png")
		self.text = "Navigate between the controls by pressing keys left, right, prev and next. Activate them by pressing the action key."

		self.label = mmsv2gui.LabelControl(230, 100, self.getWidth() - 230, 30)
		self.textbox = mmsv2gui.TextBoxControl(250, 100, self.getWidth() - 300, self.getHeight() - 300)
		self.textbox.setText(self.text)

		self.button1 = mmsv2gui.ButtonControl(50, 100, 160, 40, "Button1", focus="button-focus.png", nofocus="button-nofocus.png", textoffset=15)
		self.button2 = mmsv2gui.ButtonControl(50, 150, 160, 40, "Button2", focus="button-focus.png", nofocus="button-nofocus.png", textoffset=15)
		self.button3 = mmsv2gui.ButtonControl(50, 200, 160, 40, "Button3", focus="button-focus.png", nofocus="button-nofocus.png", textoffset=15)
		self.button4 = mmsv2gui.ButtonControl(50, 250, 160, 40, "Button4", focus="button-focus.png", nofocus="button-nofocus.png", textoffset=15)
		self.button5 = mmsv2gui.ButtonControl(50, 300, 160, 40, "Button5", focus="button-focus.png", nofocus="button-nofocus.png", textoffset=15)
		self.button7 = mmsv2gui.ButtonControl(self.getWidth() - 100, self.getHeight() - 100, 50, 50, focus="sub-exit-focus.gif", nofocus="sub-exit-nofocus.png")
		self.button6 = mmsv2gui.ButtonControl(self.getWidth() - 160, self.getHeight() - 100, 50, 50, focus="sub-info-focus.gif", nofocus="sub-info-nofocus.png")

		self.listbg = mmsv2gui.ImageControl(self.getWidth() - 245, self.getHeight() - 310, 150, 210, "subpanel.png")
		self.list = mmsv2gui.ListControl(self.getWidth() - 225, self.getHeight() - 300, 100, 180, focus="list-focus.png", nofocus="list-nofocus.png", alignment="center")

		self.addControl(self.bg)
		self.addControl(self.label)
		self.addControl(self.textbox)
		self.addControl(self.button1)
		self.addControl(self.button2)
		self.addControl(self.button3)
		self.addControl(self.button4)
		self.addControl(self.button5)
		self.addControl(self.button6)
		self.addControl(self.button7)
		
		self.addControl(self.listbg)
		self.addControl(self.list)
		self.listbg.setVisible(False)
		self.list.setVisible(False)
		
		self.button1.controlRight(self.button6)
		self.button2.controlRight(self.button6)
		self.button3.controlRight(self.button6)
		self.button4.controlRight(self.button6)
		self.button5.controlRight(self.button6)
		
		self.button1.controlNext(self.button2)
		self.button1.controlPrev(self.button5)
		self.button2.controlNext(self.button3)
                self.button2.controlPrev(self.button1)
		self.button3.controlNext(self.button4)
                self.button3.controlPrev(self.button2)
		self.button4.controlNext(self.button5)
                self.button4.controlPrev(self.button3)
		self.button5.controlNext(self.button1)
                self.button5.controlPrev(self.button4)
		self.button6.controlRight(self.button7)
		self.button6.controlLeft(self.button1)
		self.button6.controlPrev(self.list)
		self.button7.controlLeft(self.button6)

		self.setFocus(self.button1)
		
		for i in range(15):
 			self.list.addItem(MyItem("item " + str(i)))

	def onClick(self, control):
		print "Control got clicked"
		print control

	def onFocus(self, control):
                print "Control got focus" 
		print control

	def onAction(self, action):
		#Importent to catch some action, otherwise user can't close window

		self.label.setLabel("")
		self.textbox.setVisible(False)
		if action == "back" and self.list.getVisible():
			self.listbg.setVisible(False)
                        self.list.setVisible(False)
			self.setFocus(self.button6)
                if action == "action":
			if self.getFocus() == self.button1:
				self.label.setLabel("You pressed button 1")
			elif self.getFocus() == self.button2:
                                self.label.setLabel("You pressed button 2")
			elif self.getFocus() == self.button3:
                                self.label.setLabel("You pressed button 3")
			elif self.getFocus() == self.button4:
                                self.label.setLabel("You pressed button 4")
			elif self.getFocus() == self.button5:
                                self.label.setLabel("You pressed button 5")
			elif self.getFocus() == self.button6:
				self.label.setLabel("Press the back key to close sub menu")
				self.listbg.setVisible(True)
                        	self.list.setVisible(True)
			elif self.getFocus() == self.button7:
				self.close()
			elif self.getFocus() == self.list:
                                self.label.setLabel("You pressed the sub menu: " + self.list.getSelectedItem().getLabel())

win = Windowexample1()
win.doModal()

del win
