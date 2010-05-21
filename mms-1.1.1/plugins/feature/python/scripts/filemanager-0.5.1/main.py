'''
Author : Yann Cezard <eesprit@free.fr>
Description : A Simple File Manager for MMSv2
License : GPLv2
'''
import os
import os.path
import mmsv2gui
import commands
import shutil
import sys

### EDIT HERE :
# syntax is : [ DISPLAYED_NAME, DEVICE, MOUNTPOINT ]
mountpoints = [ ["DVD Rom", '/dev/dvd', '/media/dvd'],
               ["Archos Jukebox", '/dev/sda1', '/mnt/jukebox'], 
             ]

# To only allow a subset of the file system to be viewable :
# Note that there is no real fakeroot, just masquerading, if
# you have symbolic links, it will be possible to escape from the jail !!!
# No fakeroot : fakeroot =['/',]
fakeroot = ['/media', '/tmp', '/data/data/Videos']

# The commands to mount/umount (adjust to suit your needs, if using sudo, update the sudoers table so that
# the user MMS is launched under has the right to use mount and unmount wthout password (NOPASSWD)
mount_command  = "/usr/bin/sudo /bin/mount"
umount_command = "/usr/bin/sudo /bin/umount"

# This is for masking dot_files or not :
HIDE_DOT_FILES = False
### END USER EDITION

NAME_IDX, DEV_IDX, MP_IDX = 0, 1, 2

class MountPoint(mmsv2gui.ListItem):
    def __init__(self, label):
        mmsv2gui.ListItem.__init__(self, label)
        self.setLabel(label)
        
    def setMountPoint(self, mountpoint):
        self.mountpoint = mountpoint

    def getMountPoint(self):
        return self.mountpoint

    def check(self):
        if self.getStatus():
            self.setThumbImage('active.png')
        else:
            self.setThumbImage('inactive.png')
            
    def getStatus(self):
        command = "/bin/grep %s /etc/mtab > /dev/null"%(self.mountpoint[DEV_IDX])
        return commands.getstatusoutput(command)[0] == 0

    def mount(self):
        command = "%s %s %s"%(mount_command, self.mountpoint[DEV_IDX], self.mountpoint[MP_IDX])
        return commands.getstatusoutput(command)

    def umount(self):
        command = "%s %s"%(umount_command, self.mountpoint[DEV_IDX])
        return commands.getstatusoutput(command)



class MountPoints(mmsv2gui.Window):
    def refresh(self):
        self.mountpoints.clear()
        for mountpoint in mountpoints :
            mp = MountPoint(mountpoint[NAME_IDX])
            mp.setMountPoint(mountpoint)
            mp.check()
            self.mountpoints.addItem(mp)
            
            
    def __init__(self):
        mmsv2gui.Window.__init__(self)

        self.bg = mmsv2gui.ImageControl(0, 0, self.getWidth(), self.getHeight(), "general/bg.png")
        
        left_offset = 50
        up_offset = 150
        right_offset = 50
        down_offset = 50
        middle_offset = 50
        
        self.label = mmsv2gui.LabelControl(left_offset, 25, self.getWidth() - left_offset - right_offset, 75)
        self.label.setLabel("Mount Point Manager")
        
        width = (self.getWidth() - left_offset - right_offset - middle_offset)/2
        height = self.getHeight() - up_offset - down_offset
        
        self.mountpoints = mmsv2gui.ListControl(left_offset, up_offset, width, height,focus="list-focus.png", nofocus="list-nofocus.png", alignment="center" )
        
        self.textbox = mmsv2gui.TextBoxControl(left_offset + width + middle_offset, up_offset, width, height)
        self.textbox.setText("Messages")
        
        
        self.addControl(self.bg)
        self.addControl(self.label)
        self.addControl(self.mountpoints)
        self.addControl(self.textbox)
        
        self.refresh()
        self.setFocus(self.mountpoints)


    def onAction(self, action):
        #Importent to catch some action, otherwise user can't close window

        #self.label.setLabel("")
        #self.textbox.setVisible(False)
        if action == "back":
            self.close()
        if action == "action":
            if self.getFocus() == self.mountpoints:
                mountpoint = self.mountpoints.getSelectedItem()
                if mountpoint.getStatus():
                    self.textbox.setText(mountpoint.umount()[1])
                else:
                    self.textbox.setText(mountpoint.mount()[1])
                # Wait 2 seconds before checking status...
                #time.sleep(2)
                self.refresh()


class MessageDialog(mmsv2gui.Window):
    def __init__(self):
        mmsv2gui.Window.__init__(self)
        button_width = 150
        button_height = 40
        border = 25
        msg_border = 5
        self.bg = mmsv2gui.RectangleControl(self.getWidth()/4, self.getHeight()/4, self.getWidth()/2, self.getHeight()/2, 200, '0xff0000')
        self.ok = mmsv2gui.ButtonControl(self.getWidth()/2 + border - button_width/2, 3*self.getHeight()/4 - border - button_height, button_width, button_height, "Ok", focus="button-focus.png", nofocus="button-nofocus.png", textoffset=15)
        
        self.message = mmsv2gui.TextBoxControl(self.getWidth()/4 + msg_border, self.getHeight()/4 + msg_border, self.getWidth()/2 - 2*msg_border, self.getHeight()/2 - 2*msg_border)
        #self.message = mmsv2gui.TextBoxControl(self.getWidth()/4 + msg_border, self.getHeight()/4 + msg_border, 1000, 100)
        
        self.addControl(self.bg)
        self.addControl(self.message)
        self.addControl(self.ok)
                
        self.setFocus(self.ok)

    def setMessage(self, text):
        self.message.setText(text) 
    
    def display(self):
        self.doModal()
    
    def onAction(self, action):
        self.close()

class Dialog(mmsv2gui.Window):
    def __init__(self):
        mmsv2gui.Window.__init__(self)
        button_width = 150
        button_height = 40
        border = 25
        msg_border = 5
        self.bg = mmsv2gui.RectangleControl(self.getWidth()/4, self.getHeight()/4, self.getWidth()/2, self.getHeight()/2, 200, '0x000000')
        self.ok = mmsv2gui.ButtonControl(self.getWidth()/4 + border, 3*self.getHeight()/4 - border - button_height, button_width, button_height, "Ok", focus="button-focus.png", nofocus="button-nofocus.png", textoffset=15)
        self.cancel = mmsv2gui.ButtonControl(3*self.getWidth()/4 - border - button_width, 3*self.getHeight()/4 - border - button_height, button_width, button_height, "Cancel", focus="button-focus.png", nofocus="button-nofocus.png", textoffset=15)
        self.choice = False
        
        self.message = mmsv2gui.TextBoxControl(self.getWidth()/4 + msg_border, self.getHeight()/4 + msg_border, self.getWidth()/2 - 2*msg_border, self.getHeight()/2 - 2*msg_border)
        #self.message = mmsv2gui.TextBoxControl(self.getWidth()/4 + msg_border, self.getHeight()/4 + msg_border, 1000, 100)
        
        self.addControl(self.bg)
        self.addControl(self.message)
        self.addControl(self.ok)
        self.addControl(self.cancel)
        
        self.ok.controlRight(self.cancel)
        self.cancel.controlRight(self.ok)
        self.ok.controlLeft(self.cancel)
        self.cancel.controlLeft(self.ok)
        
        self.setFocus(self.cancel)

    def setMessage(self, text):
        self.message.setText(text)

    def getChoice(self):
        self.doModal()
        print >> sys.stderr, "Returning : %s" % str(self.choice)
        return self.choice
    
    def onAction(self, action):
        if action == "action":
            if self.getFocus() == self.ok:
                self.choice = True
                self.close()
            elif self.getFocus() == self.cancel:
                self.choice = False
                self.close()


class File(mmsv2gui.ListItem):
    def __init__(self, label):
        mmsv2gui.ListItem.__init__(self, label)
        self.setLabel(label)
        self.setThumbImage('file.png')


class Folder(mmsv2gui.ListItem):
    def __init__(self, label):
        mmsv2gui.ListItem.__init__(self, label)
        self.setLabel(label)
        self.setThumbImage('folder.png')


class FileLink(mmsv2gui.ListItem):
    def __init__(self, label):
        mmsv2gui.ListItem.__init__(self, label)
        self.setLabel(label)
        self.setThumbImage('file-link.png')


class FolderLink(mmsv2gui.ListItem):
    def __init__(self, label):
        mmsv2gui.ListItem.__init__(self, label)
        self.setLabel(label)
        self.setThumbImage('folder-link.png')

def fakeRoot(path):
    depth = path.count('/')
    if path[-1] == '/':
        depth -= 1
    for folder in fakeroot:
        f_depth = folder.count('/')
        if folder[-1] == '/':
            f_depth -= 1
        test = None
        if f_depth <= depth:
            test = folder
        else:
            test = folder.rsplit('/', f_depth - depth)[0]
        if path.startswith(test):
            return True
    return False
        
class Browser(mmsv2gui.ListControl):
    def sortList(self, liste):
        folders = []
        files = []
        for item in liste:
            if HIDE_DOT_FILES and item.startswith('.'):
                continue
            item_path = os.path.join(self.path, item)
            if fakeRoot(item_path):
                if os.path.islink(item_path):
                    if os.path.isfile(os.path.realpath(item_path)):
                        files.append(item)
                    elif os.path.isdir(os.path.realpath(item_path)):
                        folders.append(item)
                elif os.path.isfile(item_path):
                    files.append(item)
                elif os.path.isdir(item_path):
                    folders.append(item)
            
        folders.sort()
        files.sort()
        folders.extend(files)
        return folders
             
    def setPath(self, path):
        self.path = path
        self.clear()
        content = self.sortList(os.listdir(self.path))
        if self.path != '/':
            self.addItem(Folder('..'))
        for item in content:
            item_path = os.path.join(self.path, item)
            if os.path.islink(item_path):
                if os.path.isfile(os.path.realpath(item_path)):
                    self.addItem(FileLink(item))
                elif os.path.isdir(os.path.realpath(item_path)):
                    self.addItem(FolderLink(item))
            elif os.path.isfile(item_path):
                self.addItem(File(item))
            elif os.path.isdir(item_path):
                self.addItem(Folder(item))
        
    def getPath(self):
        return self.path
    
    def refresh(self):
        self.setPath(self.path)
    
    def getSelectedPath(self):
        return os.path.join(self.path, self.getSelectedItem().getLabel())
    
    def __init__(self, x, y, w, h, focus, nofocus, alignment):
        mmsv2gui.ListControl.__init__(self, x, y, w, h, focus=focus, nofocus=nofocus, alignment=alignment)
        self.path = '/'

    def changePath(self):
        selected = self.getSelectedItem().getLabel()
        if selected == '..':
            self.setPath(os.path.split(self.getPath())[0])
        else:
            self.setPath(os.path.join(self.getPath(), selected))


class FileManager(mmsv2gui.Window):
    def __init__(self):
        mmsv2gui.Window.__init__(self)
        up_offset = 50
        left_offset = 50
        right_offset = 50
        down_offset = 125 
        action_offset = 150
        bg_border = 5

        self.bg = mmsv2gui.ImageControl(0, 0, self.getWidth(), self.getHeight(), "general/bg.png")

        browser_width = (self.getWidth() - left_offset - action_offset - right_offset)/2
        browser_height = self.getHeight() - up_offset - down_offset
        
        self.l_browser_path = mmsv2gui.LabelControl(left_offset, up_offset - 30 - bg_border, browser_width, 30)
        self.r_browser_path = mmsv2gui.LabelControl(browser_width + action_offset + left_offset, up_offset - 30 - bg_border, browser_width, 30)
        
        self.l_browser_bg = mmsv2gui.RectangleControl(left_offset - bg_border, up_offset - bg_border, browser_width+2*bg_border, browser_height +2*bg_border, 20, '0xffffff')
        self.l_browser = Browser(left_offset, up_offset, browser_width, browser_height, "list-focus.png", "list-nofocus.png", "center")

        nb_buttons = 4
        button_height = 30
        buttons_offset = (browser_height - nb_buttons*button_height)/(nb_buttons+1)
        self.cp_button = mmsv2gui.ButtonControl(left_offset + browser_width + bg_border, up_offset + buttons_offset, action_offset - 2* bg_border, button_height, "Copy", focus="button-focus.png", nofocus="button-nofocus.png", textoffset=15)
        self.mv_button = mmsv2gui.ButtonControl(left_offset + browser_width + bg_border, up_offset + 2*buttons_offset + button_height, action_offset - 2* bg_border, button_height, "Move", focus="button-focus.png", nofocus="button-nofocus.png", textoffset=15)
        self.ln_button = mmsv2gui.ButtonControl(left_offset + browser_width + bg_border, up_offset + 3*buttons_offset + 2*button_height, action_offset - 2* bg_border, button_height, "Link", focus="button-focus.png", nofocus="button-nofocus.png", textoffset=15)
        self.mount_button = mmsv2gui.ButtonControl(left_offset + browser_width + bg_border, up_offset + 4*buttons_offset + 3*button_height, action_offset - 2* bg_border, button_height, "Mount...", focus="button-focus.png", nofocus="button-nofocus.png", textoffset=15)

        self.r_browser_bg = mmsv2gui.RectangleControl(self.getWidth() - right_offset - browser_width - bg_border, up_offset - bg_border, browser_width+2*bg_border, browser_height+2*bg_border, 20, '0xffffff')
        self.r_browser = Browser(self.getWidth() - right_offset - browser_width, up_offset, browser_width, browser_height, "list-focus.png", "list-nofocus.png", "center" ) 

        self.messages_bg = mmsv2gui.RectangleControl(left_offset- bg_border, self.getHeight() - down_offset + 2*bg_border, self.getWidth() - 2*bg_border, down_offset-4*bg_border, 10, '0xffffff')
        message_line_height = (down_offset-4*bg_border)/3
        
        self.message1 = mmsv2gui.TextBoxControl(left_offset, self.getHeight() - down_offset + 4*bg_border, 2*browser_width, message_line_height)
        self.message2 = mmsv2gui.TextBoxControl(left_offset, self.getHeight() - down_offset + 4*bg_border + message_line_height, 2*browser_width, message_line_height)
        self.message3 = mmsv2gui.TextBoxControl(left_offset, self.getHeight() - down_offset + 4*bg_border + 2*message_line_height, 2*browser_width, message_line_height)
        self.messages = ['', '', '']
        self.msgidx = 0
        
        self.l_browser.setPath('/')
        self.l_browser_path.setLabel(self.l_browser.getPath())
        self.r_browser.setPath('/')
        self.r_browser_path.setLabel(self.r_browser.getPath())
        
        self.addControl(self.bg)
        
        self.addControl(self.l_browser_path)
        self.addControl(self.l_browser_bg)
        self.addControl(self.l_browser)
        
        self.addControl(self.cp_button)
        self.addControl(self.mv_button)
        self.addControl(self.ln_button)
        self.addControl(self.mount_button)
        
        self.addControl(self.r_browser_path)
        self.addControl(self.r_browser_bg)
        self.addControl(self.r_browser)
        
        self.addControl(self.messages_bg)
        self.addControl(self.message1)
        self.addControl(self.message2)
        self.addControl(self.message3)

        self.l_browser.controlRight(self.cp_button)
        
        self.cp_button.controlRight(self.r_browser)
        self.mv_button.controlRight(self.r_browser)
        self.ln_button.controlRight(self.r_browser)
        self.mount_button.controlRight(self.r_browser)
        
        self.cp_button.controlLeft(self.l_browser)
        self.mv_button.controlLeft(self.l_browser)
        self.ln_button.controlLeft(self.l_browser)
        self.mount_button.controlLeft(self.l_browser)
        
        self.cp_button.controlNext(self.mv_button)
        self.cp_button.controlPrev(self.mount_button)
        self.mv_button.controlNext(self.ln_button)
        self.mv_button.controlPrev(self.cp_button)
        self.ln_button.controlNext(self.mount_button)
        self.ln_button.controlPrev(self.mv_button)
        self.mount_button.controlNext(self.cp_button)
        self.mount_button.controlPrev(self.ln_button)
        
        self.l_browser.controlLeft(self.r_browser)
        self.r_browser.controlRight(self.l_browser)
        self.r_browser.controlLeft(self.cp_button)
        
        self.setFocus(self.l_browser)

    def appendMessage(self, message):
        if self.msgidx >= 3:
            self.messages[0] = self.messages[1]
            self.messages[1] = self.messages[2]
            self.messages[2] = message
        else:
            self.messages[self.msgidx] = message
            self.msgidx += 1
        self.message1.setText(self.messages[0])
        self.message2.setText(self.messages[1])
        self.message3.setText(self.messages[2])

    #def onClick(self, control):
        #self.appendMessage("Left Selection : " + self.l_browser.getSelectedPath() + " - Right Selection : " + self.r_browser.getSelectedPath())

    def onFocus(self, control):
        if self.getFocus() == self.cp_button:
            self.appendMessage("Copying " + self.l_browser.getSelectedPath() + " to " + self.r_browser.getPath() + " ??")
        elif self.getFocus() == self.mv_button:
            self.appendMessage("Moving " + self.l_browser.getSelectedPath() + " to " + self.r_browser.getPath() + " ??")
        elif self.getFocus() == self.ln_button:
            self.appendMessage("Linking " + self.l_browser.getSelectedPath() + " to " + self.r_browser.getPath() + " ??")

    def copy(self):
        self.dialog = Dialog()
        self.dialog.setMessage("Are you sure you want to copy " + self.l_browser.getSelectedPath() + " to " + self.r_browser.getPath() + "?")
        if self.dialog.getChoice():
            del self.dialog
            self.appendMessage("Copying " + self.l_browser.getSelectedPath() + " to " + self.r_browser.getPath() + " . . .")
            print >> sys.stderr, 'shutil.copytree("%s", "%s")' % (self.l_browser.getSelectedPath(), self.r_browser.getPath())
            try:
                if os.path.isdir(self.l_browser.getSelectedPath()):
                    shutil.copytree(self.l_browser.getSelectedPath(), self.r_browser.getPath())
                elif os.path.isfile(self.l_browser.getSelectedPath()):
                    shutil.copy(self.l_browser.getSelectedPath(), self.r_browser.getPath())
                self.appendMessage("Copy OK")
	    except IOError, exc:
                self.appendMessage("Copy Error")
                self.dialog = MessageDialog()
                self.dialog.setMessage("Error: " + str(exc))
                self.dialog.display() 
                del self.dialog
	    except OSError, exc:
                self.appendMessage("Copy Error")
                self.dialog = MessageDialog()
                self.dialog.setMessage("Error: " + str(exc))
                self.dialog.display() 
                del self.dialog
        else:
            del self.dialog
            self.appendMessage("Copy Canceled")
        
    def move(self):
        dialog = Dialog()
        dialog.setMessage("Are you sure you want to move " + self.l_browser.getSelectedPath() + " to " + self.r_browser.getPath() + "?")
        if dialog.getChoice():
            del dialog
            self.appendMessage("Moving " + self.l_browser.getSelectedPath() + " to " + self.r_browser.getPath() + " . . .")
            try:
                if os.path.isfile(self.l_browser.getSelectedPath()):
                    print >> sys.stderr, 'shutil.move("%s", "%s")' % (self.l_browser.getSelectedPath(), self.r_browser.getPath())
                    shutil.move(self.l_browser.getSelectedPath(), self.r_browser.getPath())
                    self.appendMessage("Move OK")
                elif os.path.isdir(self.l_browser.getSelectedPath()):
                    print >> sys.stderr, 'shutil.move("%s", "%s")' % (self.l_browser.getSelectedPath(), os.path.join(self.r_browser.getPath(), os.path.basename(self.l_browser.getSelectedPath())))
                    shutil.move(self.l_browser.getSelectedPath(), os.path.join(self.r_browser.getPath(), os.path.basename(self.l_browser.getSelectedPath())))
                    self.appendMessage("Move OK")
	    except IOError, exc:
                self.appendMessage("Move Error")
                self.dialog = MessageDialog()
                self.dialog.setMessage("Error: " + str(exc))
                self.dialog.display() 
                del self.dialog
	    except OSError, exc:
                self.appendMessage("Move Error")
                self.dialog = MessageDialog()
                self.dialog.setMessage("Error: " + str(exc))
                self.dialog.display() 
                del self.dialog
        else:
            del dialog
            self.appendMessage("Move Canceled")

    def link(self):
        dialog = Dialog()
        dialog.setMessage("Are you sure you want to link " + self.l_browser.getSelectedPath() + " to " + self.r_browser.getPath() + "?")
        if dialog.getChoice():
            del dialog
            self.appendMessage("Linking " + self.l_browser.getSelectedPath() + " to " + self.r_browser.getPath() + " . . .")
            try:
                if os.path.isfile(self.l_browser.getSelectedPath()):
	            print >> sys.stderr, 'os.link("%s", "%s")' % (self.l_browser.getSelectedPath(), os.path.join(self.r_browser.getPath(), os.path.basename(self.l_browser.getSelectedPath()))) 
                    os.link(self.l_browser.getSelectedPath(), os.path.join(self.r_browser.getPath(), os.path.basename(self.l_browser.getSelectedPath()))) 
                    self.appendMessage("Link OK")
                else:
                    self.dialog = MessageDialog()
                    self.dialog.setMessage("Error: can't hard link to a dir")
                    self.dialog.display() 
	    except IOError, exc:
                self.appendMessage("Link Error")
                self.dialog = MessageDialog()
                self.dialog.setMessage("Error: " + str(exc))
                self.dialog.display()
                del self.dialog
	    except OSError, exc:
                self.appendMessage("Link Error")
                self.dialog = MessageDialog()
                self.dialog.setMessage("Error: " + str(exc))
                self.dialog.display() 
                del self.dialog
        else:
            del dialog
            self.appendMessage("Link Canceled")

    def onAction(self, action):
        #Importent to catch some action, otherwise user can't close window
        if action == "back":
            self.close()
        if action == "action":
            if self.getFocus() == self.l_browser:
                self.l_browser.changePath()
                self.l_browser_path.setLabel(self.l_browser.getPath())
                self.appendMessage("Left Selection : " + self.l_browser.getSelectedPath() + " - Right Selection : " + self.r_browser.getSelectedPath())
            elif self.getFocus() == self.r_browser:
                self.r_browser.changePath()
                self.r_browser_path.setLabel(self.r_browser.getPath())
                self.appendMessage("Left Selection : " + self.l_browser.getSelectedPath() + "- Right Selection : " + self.r_browser.getSelectedPath())
            elif self.getFocus() == self.cp_button:
                self.copy()
                self.r_browser.refresh()
            elif self.getFocus() == self.mv_button:
                self.move()
		self.l_browser.refresh()
                self.r_browser.refresh()
            elif self.getFocus() == self.ln_button:
                self.link()
                self.r_browser.refresh()
            elif self.getFocus() == self.mount_button:
                mountpoints = MountPoints()
                mountpoints.doModal()
                del mountpoints

win = FileManager()
win.doModal()
del win
