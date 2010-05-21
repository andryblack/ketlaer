import mmsv2gui, mmsv2
import os, sys
import urllib

sys.path.append("./lib")
import youtubelib
import youtubedb

# Change this
# BASERDIR and DOWNLOADDIR must exist 
# and mms user must have write access
#
########################################
BASEDIR = '/tmp'
DOWNLOADDIR = '/tmp'
########################################

DBPATH = os.path.join(BASEDIR, 'youtube.db')

MOVIELIST = 13
SEARCHFIELD = 14
MESSAGELABEL = 3
EXIT = 2
RECENTLYADDED = 16
RECENTLYFEATURED = 17
TOPFAVORITES = 18
TOPRATED = 19
SEARCH = 20
SEARCHBG = 21
FAVORITES = 22

class FavoritesWindow1(mmsv2gui.Window):
	def __init__(self, xmlpath, id, name, url):
		mmsv2gui.Window.__init__(self, xmlpath)
		self.id = id
		self.name = name
		self.url = url
		self.count = 0
		self.message = self.getControl(2)

	def callback(self, bs, bc, tot):
                current = int(float(bs*bc)/float(tot) * float(100))
                if current > self.count:
                        self.count = current
                        if self.count <= 100:
                                self.progress(self.count, "Downloading")

	def progress(self, current, text):
                if current != None:
                        self.message.setLabel(text +  " (" + str(current) + "%)")
                else:
                        self.message.setLabel(text)

                mmsv2gui.redraw()

	def download(self):
                self.count = 0

                self.progress(self.count, "Downloading")
                urllib.urlretrieve(self.url, filename=os.path.join(DOWNLOADDIR, self.name + ".flv"), reporthook=self.callback)
                self.progress(None, " ")

	def onClick(self, control):
		if control == 10:
			db = youtubedb.YouTubeDB(DBPATH)
			db.insertMovie(self.id, self.name)
			del db

			self.close()

		elif control == 11:
			self.download()

			self.close()

	def onFocus(self, control):
		if control == 10:
			self.progress(None, "Add to favorites")
                elif control == 11:
			self.progress(None, "Download movie")

class FavoritesWindow2(mmsv2gui.Window):
        def __init__(self, xmlpath, id, name, url):
		mmsv2gui.Window.__init__(self, xmlpath)
                self.id = id
                self.name = name
                self.url = url
                self.count = 0
                self.message = self.getControl(2)

        def callback(self, bs, bc, tot):
                current = int(float(bs*bc)/float(tot) * float(100))
                if current > self.count:
                        self.count = current
                        if self.count <= 100:
                                self.progress(self.count, "Downloading")

        def progress(self, current, text):
                if current != None:
                        self.message.setLabel(text +  " (" + str(current) + "%)")
                else:
                        self.message.setLabel(text)

                mmsv2gui.redraw()

        def download(self):
                self.count = 0

                self.progress(self.count, "Downloading")
                urllib.urlretrieve(self.url, filename=os.path.join(DOWNLOADDIR, self.name + ".flv"), reporthook=self.callback)
                self.progress(None, " ")

	def onClick(self, control):
                if control == 10:
                        db = youtubedb.YouTubeDB(DBPATH)
                        db.deleteMovie(self.id)
                        del db

                        self.close()

                elif control == 11:
                        self.download()

                        self.close()

        def onFocus(self, control):
                if control == 10:
                        self.progress(None, "Delete from favorites")
                elif control == 11:
                        self.progress(None, "Download movie")	

class  MovieItem(mmsv2gui.ListItem):
	def __init__(self, id, name, favorite):
		self.id = id
		self.name = name
		self.favorite = favorite

		self.setLabel(self.name)

class YouTubeWindow(mmsv2gui.Window):
	def __init__(self, xmlpath):
		mmsv2gui.Window.__init__(self, xmlpath)
	  	self.youtube = youtubelib.YouTube(BASEDIR, self.progress)
		self.movielist = self.getControl(MOVIELIST)
		self.message = self.getControl(MESSAGELABEL)
		self.searchfield = self.getControl(SEARCHFIELD)
		self.searchbg = self.getControl(SEARCHBG)
		self.movielist.setVisible(False)
		self.searchfield.setEditable(True)
		self.searchfield.setVisible(False)
                self.searchbg.setVisible(False)
		self.player = mmsv2.Player(True)

        def cleanup(self):
		del self.youtube
		del self.player

        def progress(self, current, text):
		if current != None:
			self.message.setLabel(text +  " (" + str(current) + "%)")
		else:
			self.message.setLabel(text)

		mmsv2gui.redraw()

	def onClick(self, control):
		if control == EXIT:
                	self.close()

		elif control == SEARCHFIELD:
			searchtext = self.searchfield.getText()
			if searchtext:
				movies = self.youtube.search(searchtext)
				self.movielist.clear()
				for movie in movies:
					self.movielist.addItem(MovieItem(movie[1], movie[0], False))

				if self.movielist.size():
					self.movielist.setVisible(True)
				else:
					self.movielist.setVisible(False)

			self.searchfield.setVisible(False)
                        self.searchbg.setVisible(False)

			self.setFocusId(SEARCH)

		elif control == SEARCH:
			self.searchfield.setVisible(True)
			self.searchbg.setVisible(True)

			self.setFocusId(SEARCHFIELD)

		elif control == RECENTLYADDED:
                	movies = self.youtube.get_feed('recently_added')
			self.movielist.clear()
                        for movie in movies:
                        	self.movielist.addItem(MovieItem(movie[1], movie[0], False))

                        if self.movielist.size():
                        	self.movielist.setVisible(True)
                        else:
                        	self.movielist.setVisible(False)

		elif control == RECENTLYFEATURED:                                                      
                        movies = self.youtube.get_feed('recently_featured')
                        self.movielist.clear()
                        for movie in movies:
                        	self.movielist.addItem(MovieItem(movie[1], movie[0], False))

                        if self.movielist.size():
                        	self.movielist.setVisible(True)
                        else:
                        	self.movielist.setVisible(False)

		elif control == TOPFAVORITES:                                                      
                        movies = self.youtube.get_feed('top_favorites')
                        self.movielist.clear()
                        for movie in movies:
                        	self.movielist.addItem(MovieItem(movie[1], movie[0], False))

                       	if self.movielist.size():
                        	self.movielist.setVisible(True)
                        else:
                        	self.movielist.setVisible(False)

		elif control == TOPRATED:                                                    
                        movies = self.youtube.get_feed('top_rated')
                        self.movielist.clear()
                        for movie in movies:
                        	self.movielist.addItem(MovieItem(movie[1], movie[0], False))

                        if self.movielist.size():
                        	self.movielist.setVisible(True)
                        else:
                         	self.movielist.setVisible(False)

		elif control == MOVIELIST:
			if self.movielist.size():
				movieid = self.movielist.getSelectedItem().id

				try:
					filename = self.movielist.getSelectedItem().name + ".flv"
					filename = os.path.join(DOWNLOADDIR, filename)
					if os.path.exists(filename):
						self.player.play(filename)
					else:
						movieurl = self.youtube.get_video_url(movieid)
						self.player.play(movieurl)
				except Exception, e:
					pass
		
		elif control == FAVORITES:
			db = youtubedb.YouTubeDB(DBPATH)
                        movies = db.getMovies()
			del db
                        self.movielist.clear()
                        for movie in movies:
                                self.movielist.addItem(MovieItem(movie['id'], movie['name'], True))

                        if self.movielist.size():
                                self.movielist.setVisible(True)
                        else:
                                self.movielist.setVisible(False)

	def onAction(self, action):
		if action == "back":
			self.close()
		if (action == "second_action") and (self.getFocusId() == MOVIELIST):
			if self.movielist.getSelectedItem().favorite:
				w = None
				try:
					w = FavoritesWindow2("gui/favoriteswindow2.xml", self.movielist.getSelectedItem().id, self.movielist.getSelectedItem().name, self.youtube.get_video_url(self.movielist.getSelectedItem().id))
                			w.doModal()
				except Exception, e:
                                        pass

                		del w

				db = youtubedb.YouTubeDB(DBPATH)
                        	movies = db.getMovies()
                        	del db

                        	self.movielist.clear()
                        	for movie in movies:
                                	self.movielist.addItem(MovieItem(movie['id'], movie['name'], True))

                        	if self.movielist.size():
                                	self.movielist.setVisible(True)
                        	else:
                                	self.movielist.setVisible(False)
					self.setFocusId(22)
			else:
				w = None
				try:
					w = FavoritesWindow1("gui/favoriteswindow1.xml", self.movielist.getSelectedItem().id, self.movielist.getSelectedItem().name, self.youtube.get_video_url(self.movielist.getSelectedItem().id))
                                	w.doModal()
				except Exception, e:
                                        pass

                                del w


win = YouTubeWindow("gui/mainwindow.xml")
win.doModal()
win.cleanup()

del win
