import os
import stat
import urllib
import threading
import thread
import re
import applelib
import appledb
import httpdownloader

class UpdateThread(threading.Thread):
  def __init__(self, db, callback, feed):
    threading.Thread.__init__(self)
    self.db = db
    self.callback = callback
    self.feed = feed
    self.trailers = applelib.Trailers(self.db)

  def getMessage(self):
    return "Updating, pls wait"

  def run(self):
    self.trailers.parse(callback=self.callback, feed=self.feed)
    del self.trailers

  def pause(self):
    self.trailers.stop()

class DownloadThread(threading.Thread):
  def __init__(self, callback, hd):
    threading.Thread.__init__(self)
    self.callback = callback
    self.hd = hd
    self.count = 0
 
  def getMessage(self):
    return "Downloading, pls wait"

  def progress(self, bc, tot):
    current = int(float(bc)/float(tot) * float(100))
    if current > self.count:
      self.count = current
      self.callback(self.count, None)

  def run(self):
    self.hd.setProgress(self.progress)
    self.hd.start()
    self.callback(None, "Apple Movie Trailers")

    del self.hd

  def pause(self):
     self.hd.stop()

class AppleTrailer():
  def __init__(self, dbpath, cachedir):
    self.cachedir = cachedir
    self.db = appledb.AppleTrailerDB(dbpath)
    self.thread = None

  def getMovieByCategory(self, callback, category):
    self.__insertByCategory(callback, category)

    return self.db.getMovieByCategory(category[1])

  def getMovieByGenre(self, genre):
    return self.db.getMovieByGenre(genre)

  def getGenres(self, callback):
    self.__insertByGenre(callback, applelib._FEEDS[5])

    return self.db.getGenres()

  def getMovieStream(self, movid):
    return self.db.getMovieStream(movid)

  def getMovieCount(self, genre):
    return self.db.getMovieCount(genre)

  def isBusy(self):
    if (self.thread == None) or (not self.thread.isAlive()):
      return False
    else:
      return True

  def getMessage(self):
    return self.thread.getMessage()

  def cleanup(self):
    if self.thread:
       del self.thread

    del self.db
  
  def __insertByCategory(self, call, cat):
    if (self.thread == None) or (not self.thread.isAlive()):
      self.thread = UpdateThread(self.db, call, cat)
      self.thread.setDaemon(True)
      self.thread.start() 

  def __insertByGenre(self, call, gen):
    if (self.thread == None) or (not self.thread.isAlive()):
      self.thread = UpdateThread(self.db, call, gen)
      self.thread.setDaemon(True)
      self.thread.start()

  def downloadTrailer(self, call, path, url):
    if (self.thread == None) or (not self.thread.isAlive()):
      self.thread = DownloadThread(call, self.getHttpDownloader(path, url))
      self.thread.setDaemon(True)
      self.thread.start()

  def getHttpDownloader(self, path, url):
    idx = url.rfind('/')
    if idx == -1:
      return None

    filename = url[(idx+1):]
    path = os.path.join(path, filename)
    
    return httpdownloader.HttpDownloader(url, path)

  def cancel(self):
    if self.thread and self.thread.isAlive():
       self.thread.pause()

  def fetchImage(self, url):
    idx = url.rfind('/')
    if idx == -1:
        return None

    fn = url[(idx+1):]
    fn = os.path.join(self.cachedir, fn)

    if not os.path.exists(fn):
	try:
          urllib.urlretrieve(url, fn)
	except Exception, e:
	  print e
	  return None

    return fn
