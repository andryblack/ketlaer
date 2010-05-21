import sqlite3
import os
import thread

DBLOCK = thread.allocate_lock()

class YouTubeDB():

  def __init__(self, path):

    DBLOCK.acquire()

    self.path = path

    if not os.path.exists(self.path):
      self.__create()

    DBLOCK.release()

  def __create(self):
    retVal = True

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()
  
      dbCur.execute('CREATE TABLE Favorites (id INTEGER PRIMARY KEY, movId VARCHAR(128), movName VARCHAR(128), movDate TIMESTAMP DEFAULT CURRENT_TIMESTAMP)')
      dbCon.commit()

      dbCon.close()

    except Exception, e:
      print "create"
      print e
      retVal = False

    return retVal

  def insertMovie(self, id, name):
    retVal = True

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      args = (id, name)
      dbCur.execute('INSERT INTO Favorites (movId, movName) VALUES (?,?)', args)
      dbCon.commit()

      dbCon.close()

    except Exception, e:
      retVal = False
      print "insertMovie"
      print e

    DBLOCK.release()

    return retVal

  def deleteMovie(self, id):
    retVal = True

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      dbCur.execute('DELETE FROM Favorites WHERE movId=' + "'" + id + "'")
      dbCon.commit()

      dbCon.close()

    except Exception, e:
      retVal = False
      print "deleteMovie"
      print e

    DBLOCK.release()

    return retVal

  def clear(self):
    retVal = True

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      dbCur.execute('DELETE FROM Favorites WHERE 1')
      dbCon.commit()

      self.dbCon.close()

    except:
      print "clear"
      retVal = False

    DBLOCK.release()

    return retVal

  def getMovies(self):
    retVal = []

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      result = dbCur.execute('SELECT movId, movName FROM Favorites ORDER BY movDate DESC')
      for r in result:
        retVal.append({'id':r[0], 'name':r[1]})

      dbCon.close()

    except Exception, e:
      print "getMovies"
      print e

    DBLOCK.release()

    return retVal
