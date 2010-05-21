import sqlite3
import os
import thread

DBLOCK = thread.allocate_lock()
MONTHS = {'Jan':'01', 'Feb':'02','Mar':'03', 'Apr':'04','May':'05', 'Jun':'06','Jul':'07', 'Aug':'08','Sep':'09', 'Oct':'10','Nov':'11', 'Dec':'12'} 

class AppleTrailerDB():

  def __init__(self, path):

    DBLOCK.acquire()

    self.path = path

    if not os.path.exists(self.path):
      self.__create()
    else:
      self.__clearNewMovies()

    DBLOCK.release()

  def __create(self):
    retVal = True

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()
  
      dbCur.execute('CREATE TABLE Movie (movId INTEGER PRIMARY KEY, movName VARCHAR(128), movStudio VARCHAR(128), movImage VARCHAR(255), movDate DATE, movNew INTEGER)')
      dbCur.execute('CREATE TABLE Category (catId INTEGER PRIMARY KEY, catName VARCHAR(128))')
      dbCur.execute('CREATE TABLE Genre (genId INTEGER PRIMARY KEY, genName VARCHAR(128))')
      dbCur.execute('CREATE TABLE Stream (strId INTEGER PRIMARY KEY, strUrl VARCHAR(255), strSortKey INTEGER, strSize VARCHAR(128), movId INTEGER)')
      dbCur.execute('CREATE TABLE MovieCategory (movId INTEGER, catId INTEGER, PRIMARY KEY(movId, catId))')
      dbCur.execute('CREATE TABLE MovieGenre (movId INTEGER, genId INTEGER, PRIMARY KEY(movId, genId))')
      dbCon.commit()

      dbCon.close()

    except Exception, e:
      print "create"
      print e
      retVal = False

    return retVal

  def __clearNewMovies(self):
    retVal = True

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      dbCur.execute('UPDATE Movie SET movNew = 0 WHERE movNew = 1')
      dbCon.commit()

      dbCon.close()

    except Exception, e:
      print "clearNewMovies"
      print e
      retVal = False

    return retVal

  def findMovie(self, movie):
    retVal = False

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      result = dbCur.execute('SELECT * FROM Movie WHERE movName = ' + "'" + movie + "'")
      for r in result:
        retVal = True
        break

      dbCon.close

    except Exception, e:
      print "findMovie"
      print e

    DBLOCK.release()

    return retVal  
  
  def findCategory(self, category):
    retVal = False

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      result = dbCur.execute('SELECT * FROM Category WHERE catName = ' + "'" + category + "'")
      for r in result:
        retVal = True
        break

      dbCon.close()

    except Exception, e:
      print "findCategory"
      print e

    DBLOCK.release()

    return retVal

  def findGenre(self, genre):
    retVal = False

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      result = dbCur.execute('SELECT * FROM Genre WHERE genName = ' + "'" + genre + "'")
      for r in result:
        retVal = True
        break

      dbCon.close()

    except Exception, e:
      print "findGenre"
      print e

    DBLOCK.release()

    return retVal

  def getMovieId(self, movie):
    retVal = None

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      result = dbCur.execute('SELECT movId FROM Movie WHERE movName = ' + "'" + movie + "'")
      for r in result:
        retVal =  r[0]
        break

      dbCon.close()

    except Exception, e:
      print "getMovieId"
      print e

    DBLOCK.release()

    return retVal

  def getCategoryId(self, category):
    retVal = None

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      result = dbCur.execute('SELECT catId FROM Category WHERE catName = ' + "'" + category + "'")
      for r in result:
        retVal =  r[0]
        break

      dbCon.close()

    except Exception, e:
      print "getCategoryId"
      print e

    DBLOCK.release()

    return retVal

  def getGenreId(self, genre):
    retVal = None

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      result = dbCur.execute('SELECT genId FROM Genre WHERE genName = ' + "'" + genre + "'")
      for r in result:
        retVal =  r[0]
        break

      dbCon.close()

    except Exception, e:
      print "getGenreId"
      print e

    DBLOCK.release()

    return retVal

  def insertMovie(self, name, studio, image, date):
    retVal = True

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      dateParts = date.split(' ')
      sqlDate = dateParts[3] + '-' + MONTHS[dateParts[2]] + '-' + dateParts[1]
      new = 1

      args = (name,studio,image,sqlDate,new)
      dbCur.execute('INSERT INTO Movie (movName, movStudio, movImage, movDate, movNew) VALUES (?,?,?,?,?)', args)
      dbCon.commit()

      dbCon.close()

    except Exception, e:
      retVal = False
      print "insertMovie"
      print e

    DBLOCK.release()

    return retVal

  def insertCategory(self, name):
    retVal = True

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      dbCur.execute('INSERT INTO Category (catName) VALUES (' + "'" + name + "'" + ')')
      dbCon.commit()

      dbCon.close()

    except Exception, e:
      retVal = False
      print "insertCategory"
      print e

    DBLOCK.release()

    return retVal

  def insertGenre(self, name):
    retVal = True

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      dbCur.execute('INSERT INTO Genre (genName) VALUES (' + "'" + name + "'" + ')')
      dbCon.commit()

      dbCon.close()

    except Exception, e:
      retVal = False
      print "insertGenre"
      print e

    DBLOCK.release()

    return retVal

  def insertStream(self, url, sortkey, size, movid):
    retVal = True

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      args = (url,sortkey,size,movid)
      dbCur.execute('INSERT INTO Stream (strUrl, strSortKey, strSize, movId) VALUES (?,?,?,?)', args)
      dbCon.commit()

      dbCon.close()

    except Exception, e:
      retVal = False
      print "insertStream"
      print e

    DBLOCK.release()

    return retVal

  def insertMovieCategory(self, movid, catid):
    retVal = True

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      args = (movid,catid)
      dbCur.execute('INSERT INTO MovieCategory (movId, catId) VALUES (?,?)', args)
      dbCon.commit()

      dbCon.close()

    except Exception, e:
      retVal = False
      print "insertMovieCategory"
      print e

    DBLOCK.release()

    return retVal

  def insertMovieGenre(self, movid, genid):
    retVal = True

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      args = (movid,genid)
      dbCur.execute('INSERT INTO MovieGenre (movId, genId) VALUES (?,?)', args)
      dbCon.commit()

      dbCon.close()

    except Exception, e:
      retVal = False
      print "dbInsertMovieGenre"
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

      dbCur.execute('DELETE FROM MovieCategory WHERE 1')
      dbCur.execute('DELETE FROM MovieGenre WHERE 1')
      dbCur.execute('DELETE FROM Stream WHERE 1')
      dbCur.execute('DELETE FROM Movie WHERE 1')
      dbCur.execute('DELETE FROM Category WHERE 1')
      dbCur.execute('DELETE FROM Genre WHERE 1')
      dbCur.execute('DELETE FROM Settings WHERE 1')
      dbCon.commit()

      self.dbCon.close()

    except:
      print "clear"
      retVal = False

    DBLOCK.release()

    return retVal

  def getMovieByCategory(self, category):
    retVal = []

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      result = dbCur.execute('SELECT Movie.movId, Movie.movName, Movie.movImage, Movie.movDate, Movie.movNew FROM Movie, Category, MovieCategory WHERE Movie.movId = MovieCategory.movId AND Category.catId = MovieCategory.catId AND Category.catName = ' + "'" + category + "' ORDER BY Movie.movNew DESC, Movie.movDate DESC")
      for r in result:
        retVal.append({'id':r[0], 'name':r[1], 'image':r[2], 'date':r[3], 'new':r[4]})

      dbCon.close()

    except Exception, e:
      print "getMovieByCategory"
      print e

    DBLOCK.release()

    return retVal    

  def getMovieByGenre(self, genre):
    retVal = []  

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      result = dbCur.execute('SELECT Movie.movId, Movie.movName, Movie.movImage, Movie.movDate, Movie.movNew FROM Movie, Genre, MovieGenre WHERE Movie.movId = MovieGenre.movId AND Genre.genId = MovieGenre.genId AND Genre.genName =' + "'" + genre + "' ORDER BY Movie.movNew DESC, Movie.movDate DESC")
      for r in result:
        retVal.append({'id':r[0], 'name':r[1], 'image':r[2], 'date':r[3], 'new':r[4]})

      dbCon.close()

    except Exception, e:
      print "getMovieByGenre"
      print e

    DBLOCK.release()

    return retVal

  def getMovieStream(self, movid):
    retVal = []  

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      result = dbCur.execute('SELECT strUrl, strSize FROM Stream WHERE movId = ' + str(movid) + ' ORDER BY strSortKey')
      for r in result:
        retVal.append({'url':r[0], 'size':r[1]})

      dbCon.close()

    except Exception, e:
      print "getMovieStream"
      print e

    DBLOCK.release()

    return retVal

  def getGenres(self):
    retVal = []

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      result = dbCur.execute('SELECT genId, genName FROM Genre ORDER BY genName')
      for r in result:
        retVal.append({'id':r[0], 'name':r[1]})

      dbCon.close()

    except Exception, e:
      print "getGenres"
      print e

    DBLOCK.release()

    return retVal

  def getMovieCount(self, genre):
    retVal = 0

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      result = dbCur.execute('SELECT COUNT(Movie.movId) FROM Movie, Genre, MovieGenre WHERE Movie.movId = MovieGenre.movId AND Genre.genId = MovieGenre.genId AND Genre.genName =' + "'" + genre + "'")
      for r in result:
        retVal = r[0]

      dbCon.close()

    except Exception, e:
      print "getMovieCount"
      print e

    DBLOCK.release()

    return retVal

  def getMovies(self):
    retVal = []

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      result = dbCur.execute('SELECT movName FROM Movie')
      for r in result:
        retVal.append(r[0])

      dbCon.close()

    except Exception, e:
      print "getMovies"
      print e

    DBLOCK.release()

    return retVal

  def getMoviesFromGenre(self):
    retVal = []

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      result = dbCur.execute('SELECT Movie.movName FROM Movie, Genre, MovieGenre WHERE Movie.movId = MovieGenre.movId AND Genre.genId = MovieGenre.genId')
      for r in result:
        retVal.append(r[0])

      dbCon.close()

    except Exception, e:
      print "getMovies"
      print e

    DBLOCK.release()

    return retVal

  def getMoviesFromCategory(self, category):
    retVal = []

    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      result = dbCur.execute('SELECT Movie.movName FROM Movie, Category, MovieCategory WHERE Movie.movId = MovieCategory.movId AND Category.catId = MovieCategory.catId AND Category.catName = ' + "'" + category + "'")

      for r in result:
        retVal.append(r[0])

      dbCon.close()

    except Exception, e:
      print "getMovies"
      print e

    DBLOCK.release()

    return retVal

  def show(self, table):
    DBLOCK.acquire()

    try:
      dbCon = sqlite3.connect(self.path)
      dbCon.text_factory = str
      dbCur = dbCon.cursor()

      result = dbCur.execute('SELECT * FROM ' + table)
      for r in result:
	for field in r:
	  print field

  	print ""

      dbCon.close()

    except Exception, e:
      print "show"
      print e

    DBLOCK.release()

