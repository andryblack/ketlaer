import mmsv2gui, mmsv2
import os, sys

sys.path.append("./lib")
import appletrailer
import applelib

# Change this
# BASERDIR and DOWNLOADDIR must exist 
# and mms user must have write access
#
# BASEDIR is the dir that contains tralier
# images and database
#
# DOWNLOADDIR is the dir that will store
# downloaded trailers
########################################
BASEDIR = '/tmp'
DOWNLOADDIR = '/tmp'
########################################

DBPATH = os.path.join(BASEDIR, 'appletrailers.db')
IMAGEDIR = os.path.join(BASEDIR, 'images')

if not os.path.exists(IMAGEDIR):
  os.mkdir(IMAGEDIR)

JUSTADDED = 30
EXCLUSIVE = 31
HD = 32
FEATURED = 33
MOSTPOPULAR = 34
GENRES = 18
PARENTLIST = 12
CHILDLIST = 19
TRAILERIMAGE = 13
MESSAGELABEL = 14
SWITCH = 50
HEADLABEL = 3
EXIT = 2

class ProgressAction(mmsv2gui.Action):
	def __init__(self, label, text):
		mmsv2gui.Action.__init__(self)
    		self.label = label
    		self.text = text

  	def run(self):
    		self.label.setLabel(self.text)

class  MovieItem(mmsv2gui.ListItem):
	def __init__(self, id, name, image, date, new):
		mmsv2gui.ListItem.__init__(self)
		self.id = id
		self.name = name
		self.image = image
		self.date = date
		self.new = new

		self.setLabel(self.name)
		if self.new == 1:
			self.setLabel2("(n)")

class TrailerItem(mmsv2gui.ListItem):
	def __init__(self, url, size):
		mmsv2gui.ListItem.__init__(self)
		self.url = url
		self.size = size

		self.setLabel(self.size)

		if self.url:
			if os.path.exists(self.getFilename(DOWNLOADDIR, self.url)):
				self.setLabel2("(d)")

	def getFilename(self, path, url):
                idx = url.rfind('/')
                if idx == -1:
                        return None

                filename = url[(idx+1):]
                path = os.path.join(path, filename)

                return path

class GenreItem(mmsv2gui.ListItem):
        def __init__(self, id, name, count):
		mmsv2gui.ListItem.__init__(self)
                self.id = id
                self.name = name
		self.count = count

                self.setLabel(self.name)
		self.setLabel2('(' + str(self.count) + ')')

class AppleTrailerWindow(mmsv2gui.Window):
	def __init__(self):
		mmsv2gui.Window.__init__(self, "gui/window.xml")
	  	self.apple = appletrailer.AppleTrailer(DBPATH, IMAGEDIR)
		self.parentlist = self.getControl(PARENTLIST)
		self.childlist = self.getControl(CHILDLIST)
		self.image = self.getControl(TRAILERIMAGE)
		self.message = self.getControl(MESSAGELABEL)
		self.head = self.getControl(HEADLABEL)
		self.switch = self.getControl(SWITCH)
		self.stream = True
		self.isMovie = False
		self.isGenre = False
		self.parentlist.setVisible(False)
		self.childlist.setVisible(False)
		self.player = mmsv2.Player(True)

        def cleanup(self):
		self.apple.cleanup()
		del self.apple
		del self.player

	def progressUpdate(self, current, total, cat, text):
		if not text:
                	action = ProgressAction(self.head, "Updating - " + cat + " (" + str(current) + " of " +  str(total) + ")")
		else:
			action = ProgressAction(self.head, text)

                mmsv2gui.invokeLater(action)

        def progressDownload(self, current, text):
		if not text:
			action = ProgressAction(self.head, "Downloading (" + str(current) + "%)")
		else:
			action = ProgressAction(self.head, text)

		mmsv2gui.invokeLater(action)

	def onClick(self, control):
		if control == EXIT:
			if self.apple.isBusy():
                                self.head.setLabel(self.apple.getMessage())
                        else:
                                self.close()

		elif control > 29 and control < 35:
			movies = self.apple.getMovieByCategory(self.progressUpdate, applelib._FEEDS[control - 30])
			self.parentlist.clear()
			for movie in movies:
				self.parentlist.addItem(MovieItem(movie['id'], movie['name'], movie['image'], movie['date'], movie['new']))

			if self.parentlist.size():
				self.parentlist.setVisible(True)
				self.childlist.setVisible(False)

				self.isMovie = True
				self.isGenre = False
			else:
				self.parentlist.setVisible(False)
				self.childlist.setVisible(False)

                                self.isMovie = True
                                self.isGenre = False

		elif control == GENRES:
                        genres = self.apple.getGenres(self.progressUpdate)
                        self.parentlist.clear()
                        for genre in genres:
                                self.parentlist.addItem(GenreItem(genre['id'], genre['name'], self.apple.getMovieCount(genre['name'])))

			if self.parentlist.size():
				self.parentlist.setVisible(True)
                        	self.childlist.setVisible(False)

				self.isMovie = False
                        	self.isGenre = True
			else:
				self.parentlist.setVisible(False)
                                self.childlist.setVisible(False)

                                self.isMovie = False
                                self.isGenre = True

		elif control == SWITCH:
			if self.stream:
				self.switch.setLabel("Download")
				self.stream = False
			else:
				self.switch.setLabel("Play")
				self.stream = True
			

		elif control == PARENTLIST:
			if self.parentlist.size():
				if self.isMovie:
					if self.parentlist.getSelectedItem().id:
						trailers = self.apple.getMovieStream(self.parentlist.getSelectedItem().id)
						self.childlist.clear()
						self.childlist.addItem(TrailerItem(None, '..'))
						for trailer in trailers:
							if trailer['size'] and trailer['url']:
								self.childlist.addItem(TrailerItem(trailer['url'], trailer['size']))

						self.parentlist.setVisible(False)
						self.childlist.setVisible(True)

						self.isMovie = False
						self.isGenre = False

						self.setFocus(self.childlist)

					else:
						genres = self.apple.getGenres(self.progressUpdate)
                        			self.parentlist.clear()
                        			for genre in genres:
                                			self.parentlist.addItem(GenreItem(genre['id'], genre['name'], self.apple.getMovieCount(genre['name'])))

						self.parentlist.setVisible(True)
                                                self.childlist.setVisible(False)

                        			self.isMovie = False
                        			self.isGenre = True	
					
				elif self.isGenre:
					movies = self.apple.getMovieByGenre(self.parentlist.getSelectedItem().name)
                        		self.parentlist.clear()
					self.parentlist.addItem(MovieItem(None, '..', None, None, 0))
                        		for movie in movies:
                                		self.parentlist.addItem(MovieItem(movie['id'], movie['name'], movie['image'], movie['date'], movie['new']))

					self.parentlist.setVisible(True)
                                        self.childlist.setVisible(False)

                        		self.isMovie = True
					self.isGenre = False

		elif control == CHILDLIST:
			if self.childlist.size():
				if self.childlist.getSelectedItem().url:
					if self.stream:
						filename = self.getFilename(DOWNLOADDIR, self.childlist.getSelectedItem().url)

						if os.path.exists(filename):
							self.player.play(filename)
						else:
							self.player.play(self.childlist.getSelectedItem().url)
					else:
						if self.apple.isBusy():
                                			self.head.setLabel(self.apple.getMessage())
						else:
							self.apple.downloadTrailer(self.progressDownload, DOWNLOADDIR, self.childlist.getSelectedItem().url)
				else:
					self.parentlist.setVisible(True)
                                        self.childlist.setVisible(False)

					self.isMovie = True
                                        self.isGenre = False

					self.setFocus(self.parentlist)

	def onFocus(self, control):
		if control == PARENTLIST:
			if self.parentlist.size():
				if self.isMovie:
					movie = self.parentlist.getSelectedItem()
					if movie.image:
						image = self.apple.fetchImage(movie.image)
						if image:
							self.image.setPath(image)
							self.image.setVisible(True)
						else:
							self.image.setVisible(False)

						self.message.setLabel(str(movie.date))
				elif self.isGenre:
                                        self.image.setVisible(False)
                        		self.message.setLabel('')
			else:
				self.image.setVisible(False)
                                self.message.setLabel('')

	def onAction(self, action):
		if action == "second_action":
			self.apple.cancel()

		if action == "back":
			if self.apple.isBusy():
				self.head.setLabel(self.apple.getMessage())
			else:
				self.close()

		if action == "prev":
			if self.getFocus() == self.parentlist:
				if self.parentlist.size():
					if self.isMovie:
						movie = self.parentlist.getSelectedItem()
						if movie.image:
							image = self.apple.fetchImage(movie.image)
							if image:
                        					self.image.setPath(image)
								self.image.setVisible(True)
							else:
								self.image.setVisible(False)

                        				self.message.setLabel(str(movie.date))
						else:
							self.image.setVisible(False)
                                			self.message.setLabel('')
					elif self.isGenre:
                                        	self.image.setVisible(False)
                                        	self.message.setLabel('')

		if action == "next":
                        if self.getFocus() == self.parentlist:
				if self.parentlist.size():
					if self.isMovie:
                                		movie = self.parentlist.getSelectedItem()
						if movie.image:
							image = self.apple.fetchImage(movie.image)
							if image:
                                				self.image.setPath(image)
								self.image.setVisible(True)
							else:
								self.image.setVisible(False)

                                			self.message.setLabel(str(movie.date))
						else:
                                                        self.image.setVisible(False)
                                                        self.message.setLabel('')
					elif self.isGenre:
                                                self.image.setVisible(False)
                                                self.message.setLabel('')

			elif ((self.getFocusId() > 29) and (self.getFocusId() < 35)) or (self.getFocusId() == 18) or (self.getFocusId() == 50):
				self.setFocus(self.childlist)

	def getFilename(self, path, url):
    		idx = url.rfind('/')
    		if idx == -1:
      			return None

    		filename = url[(idx+1):]
    		path = os.path.join(path, filename)

    		return path

win = AppleTrailerWindow()
win.doModal()
win.cleanup()

del win
