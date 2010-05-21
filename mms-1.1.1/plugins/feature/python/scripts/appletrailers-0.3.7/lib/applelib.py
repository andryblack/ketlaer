# -*- coding: iso-8859-1 -*-
# -----------------------------------------------------------------------
# applelib.py - Module for parsing apple's trailer site
# -----------------------------------------------------------------------
#
# Notes:
# Todo:
#
# -----------------------------------------------------------------------
# Copyright (C) 2006 Pierre Ossman
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MER-
# CHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
# Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
# -----------------------------------------------------------------------

import sys
import os
import re
import urllib
import urlparse
import pickle

_DEFAULT_URL = 'http://www.apple.com/trailers/'

_FEEDS = (
        ( 'home/feeds/just_added.json', 'Just Added' ),
        ( 'home/feeds/exclusive.json', 'Exclusive' ),
        ( 'home/feeds/just_hd.json', 'HD' ),
        ( 'home/feeds/upcoming.json', 'Featured' ),
        ( 'home/feeds/most_pop.json', 'Most Popular' ),
        ( 'home/feeds/genres.json', None ),
        ( 'home/feeds/studios.json', None),
        )

_PATTERN = re.compile('.*(320|480|640|480p|720p|1080p)\.mov')

_FORMAT = {
      	"320":("Standard [320]", 5),
	"480":("Standard [480]", 10),
	"640":("Standard [640]", 20),
	"480p":("HD [480p]", 35),
	"720p":("HD [720p]", 45),
        "1080p":("HD [1080p]", 60)
       }

suburls = [
    "small.html",
    "small/small.html",
    "low.html",
    "trailer_small.html",
    "small_tt.html",
    "small_oet.html",
    "teaser1_small.html",
    "teaser2_small.html",
    "medium.html",
    "medium/medium.html",
    "med.html",
    "trailer_medium.html",
    "medium_tt.html",
    "medium_oet.html",
    "teaser1_medium.html",
    "teaser2_medium.html",
    "large.html",
    "large/large.html",
    "high.html",
    "trailer_large.html",
    "large_tt.html",
    "large_oet.html",
    "teaser1_large.html",
    "teaser2_large.html",
    "hd/"
      ]

class Trailers:
    def __init__(self, db):
        self.titles = {}
        self.db = db
	self.done = False

    def getRealUrl(self, url):
      idx = url.rfind('/')
      if idx == -1:
        return None

      try:
        con = urllib.urlopen(url)
        buf = con.read(128)
        con.close()
      except Exception, e:
        print e
        return None

      pattern = re.compile('[a-zA-Z0-9_-]+\.mov')
      match = pattern.search(buf)
      if match:
        filename = match.group(0)
        return url[:(idx+1)] + filename

      return None

    def stop(self):
	self.done = True

    def parse(self, callback = None, url = _DEFAULT_URL, feed = _FEEDS[0]):
        self._url = url
        feed_count = 0

        titles = get_titles(feed[0])
	
	new = []
	movies = []
        if feed[1]:
	     movies = self.db.getMoviesFromCategory(feed[1])
	else:
	     movies = self.db.getMoviesFromGenre()

	for title in titles:
	     if title not in movies:
	        new.append(title)

        title_count = 0
        if new:
          for title in new:
	     if not self.done:
	        if callback is not None:
		   if feed[1] is not None:
	              callback(title_count, len(new), feed[1], None)
		   else:
		      callback(title_count, len(new), 'Genres', None)

		title_count += 1

		if not self.db.findMovie(title):

                   self.db.insertMovie(title, titles[title]['studio'], titles[title]['poster'], titles[title]['date'])

                   for streams in get_streams(titles[title]):
		      realUrl = self.getRealUrl(streams[0])
		      if not realUrl:
			realUrl = streams[0]
                      self.db.insertStream(realUrl, streams[1][1], streams[1][0], self.db.getMovieId(title))

		if feed[1]:
		   if not self.db.findCategory(feed[1]):
                      self.db.insertCategory(feed[1])

		   self.db.insertMovieCategory(self.db.getMovieId(title), self.db.getCategoryId(feed[1]))
		else:
		   for genre in titles[title]['genre']:
                      if not self.db.findGenre(genre):
                         self.db.insertGenre(genre)

                      self.db.insertMovieGenre(self.db.getMovieId(title), self.db.getGenreId(genre))

	  if callback is not None:
              callback(None, None, None, 'Apple Movie Trailers')

def correct(s):
  return s.replace("null","None").replace("true","True").replace("false","True")

def download(url):
  buf = ""
  try:
    f = urllib.urlopen(url)
    buf = f.read()
    f.close()
  except:
    #print "appletrailerlib: warning: cannot download url: "+str(url)
    pass
  return buf

def find_movs(data):
  movs = []
  i = -4
  while( data.find(".mov",i+4) != -1 ):
    i = data.find(".mov",i+4)
    j = i-1
    while(data[j]!='\'' and data[j]!='"'):
      j -= 1
    movs.append(data[j+1:i+4])
  return movs

def parse_subpage(url,streams):
  data = download(url)
  movs = find_movs(data)
  for m in movs:
    match = _PATTERN.search(m)
    if match:
      format = match.group(1)
    else:
      format = None

    if format:
      streams.append((m, _FORMAT[format]))

def parse_title_page(url):
  streams = []

  data = download(url)
  movs = find_movs(data)
  for m in movs:
    match = _PATTERN.search(m)
    if match:
      format = match.group(1)
    else:
      format = None
    
    if format:
      streams.append((m, _FORMAT[format]))

  if not streams:
    for u in suburls:
      i = data.find(u)
      if i!=-1:
        parse_subpage(urlparse.urljoin(url,u),streams)

  # sometimes the links are not found in the front page
  # try all possibilities - not very efficient
  #if not streams:
  #  for u in suburls:
  #    parse_subpage(urlparse.urljoin(url,u),streams)

  # find duplicates
  temp = {}
  for s in streams:
    if not temp.has_key(s[1][0]):
      temp[s[1][0]] = s

  return temp.values()

def get_titles(feed):
  url = urlparse.urljoin(_DEFAULT_URL, feed)
  buf = correct(download(url))
  data = eval(buf)
  titles = {}
  for d in data:
    try:
      titles[d["title"]] = { "poster":d["poster"], "location":urlparse.urljoin(_DEFAULT_URL,d["location"]), "genre":d["genre"], "date":d['trailers'][0]['postdate'], "studio":d["studio"] }
    except:
      print "Could not add title:", d["title"]

  return titles

def get_streams(title):
  return parse_title_page(title["location"])
