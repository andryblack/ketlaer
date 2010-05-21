import urllib2
import os.path

class HttpDownloader:
   def __init__(self, url, file):
      self.__cursize = 0
      self.__url = url
      self.__file = file
      self.__progress = None
      self.__report = None
      self.__done = False

      try:
         res = urllib2.urlopen(self.__url)
         self.__totsize = int(res.info()['Content-Length'])
         res.close()

         if os.path.exists(self.__file):
            if os.path.getsize(self.__file) >= self.__totsize:
	       self.__done = True
	    else:
	       self.__cursize = os.path.getsize(self.__file)

      except Exception, e:
	 print e

   def getUrl(self):
      return self.__url

   def setProgress(self, p):
      self.__progress = p

   def start(self):
      if not self.__done:
	 try:
            range = {'Range' : 'bytes=' + str(self.__cursize) + '-'}
            req = urllib2.Request(self.__url, headers=range)
            res = urllib2.urlopen(req)
	    data = open(self.__file, 'ab')
            self.__running = True
	    while (not self.__done) and (self.__cursize < self.__totsize):
	       data.write(res.read(4096))
	       self.__cursize += 4096
	       if self.__progress:
	          self.__progress(self.__cursize, self.__totsize)

	    res.close()
	    data.close()

	 except Exception, e:
	    print e

   def stop(self):
      if not self.__done:
         self.__done = True
