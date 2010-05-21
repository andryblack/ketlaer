#include "shoutcast.hpp"
#include "audio.hpp"

#include "wget_wrapper.hpp"

#include "tinyxml.h"
#include "tinystr.h"

#include <vector>
#include <sstream>

static const char *GENREURL = "http://www.shoutcast.com/sbin/newxml.phtml";
static const char *DIRECTORYURL = "http://www.shoutcast.com/sbin/newxml.phtml?genre=";
static const char *PLAYLISTURL_01 = "http://www.shoutcast.com/sbin/tunein-station.pls?id=";
static const char *PLAYLISTURL_02 = "&filename=playlist.pls";

//static const char *DIRECTORYURL = "http://www.shoutcast.com/sbin/newtvlister.phtml?alltv=1";
//static const char *PLAYLISTURL_01 = "http://www.shoutcast.com/sbin/tunein-tvstation.pls?id=";
//static const char *PLAYLISTURL_02 = "&filename=playlist.pls";

static const char *PLURLTOKEN = "File";
static const char *PLTITLETOKEN = "Title";

static const char *XMLHEADER = "<?xml";
static const char *STATIONLIST = "stationlist";
//static const char *TUNEIN = "tunein";
static const char *STATION = "station";
static const char *GENRELIST = "genrelist";
static const char *GENRE = "genre";

static const char *NAME = "name";
static const char *ID = "id";
static const char *BR = "br";
//static const char *MT = "mt";
//static const char *CT = "ct";
//static const char *LC = "lc";
//static const char *GENRE = "genre";

using std::string;
using std::list;
using std::pair;
using std::vector;

string itostr(int i, int width, char fill, std::_Ios_Fmtflags align)
{
  std::ostringstream s;

  s.flags(align);
  s.width(width);
  s.fill(fill);
  s << i;

  return s.str();
}

list<pair<string, string> > Shoutdownloader::fetch_genrelist()
{
  std::string shoutxml;
  list<pair<string, string> > genrelist;

  if (WgetWrapper::download(GENREURL, shoutxml) > 0)
  {
    TiXmlDocument shoutxmldoc;
    int startpos = 0;
    if((startpos = shoutxml.find(XMLHEADER)) != string::npos)
    {
      if(shoutxmldoc.Parse(shoutxml.substr(startpos).c_str(), 0 , TIXML_ENCODING_UTF8))
      {
        TiXmlNode *child0 = 0;
    	while((child0 = shoutxmldoc.IterateChildren(GENRELIST, child0)))
    	{
          TiXmlNode *child1 = 0;
          while((child1 = child0->IterateChildren(GENRE, child1)))
          {
            shout_info genrelist_entry;

            genrelist_entry.name = child1->ToElement()->Attribute(NAME);
            genrelist_entry.url = DIRECTORYURL + genrelist_entry.name;

            pair<string, string> entry(genrelist_entry.name, genrelist_entry.url);
            genrelist.push_back(entry);
	  }
        }
      }
    }
  }

  return genrelist;
}

list<pair<string, string> > Shoutdownloader::fetch_toplist(string genreUrl)
{
  std::string shoutxml;
  int counter = 0;
  list<pair<string, string> > toplist;
  Audio *audio = get_class<Audio>(dgettext("mms-audio", "Audio"));

  if (WgetWrapper::download(genreUrl, shoutxml) > 0)
  {
    TiXmlDocument shoutxmldoc;
    int startpos = 0;
    if((startpos = shoutxml.find(XMLHEADER)) != string::npos)
    {
      if(shoutxmldoc.Parse(shoutxml.substr(startpos).c_str(), 0 , TIXML_ENCODING_UTF8))
      {
        TiXmlNode *child0 = 0;
    	while((child0 = shoutxmldoc.IterateChildren(STATIONLIST, child0)))
    	{
          TiXmlNode *child1 = 0;
          while((child1 = child0->IterateChildren(STATION, child1)))
          {
	    int bitrate_xml;
	    child1->ToElement()->QueryIntAttribute(BR, &bitrate_xml);

	    int bitrate_opts;
	    std::istringstream sstr(audio->get_opts()->bitrate());
	    sstr >> bitrate_opts;

	    if(bitrate_xml >= bitrate_opts)
            {
	       shout_info toplist_entry;

               toplist_entry.name = child1->ToElement()->Attribute(NAME);
               toplist_entry.id = child1->ToElement()->Attribute(ID);
               toplist_entry.url = PLAYLISTURL_01 + toplist_entry.id + PLAYLISTURL_02;

	       string bitrate = child1->ToElement()->Attribute(BR);
	       pair<string, string> entry(itostr(++counter, 3) +
					  ".  [" + bitrate + " kbps]  "
					  + toplist_entry.name,
					  toplist_entry.url);
               toplist.push_back(entry);
	    }
	  }
        }
      }
    }
  }

  return toplist;
}

pair<string, string> Shoutdownloader::fetch_playlist(string playlistname)
{
  string html;
  vector<shout_info> playlist;
  pair<string, string> playlistpair;

  if (WgetWrapper::download(playlistname, html) > 0)
  {
    string stat_url, stat_name, line;
    std::istringstream dl_html_stream(html);

    while(getline(dl_html_stream, line, '\n'))
    {
      if(line.find(PLURLTOKEN, 0) != string::npos)
      {
        int pos = line.find("=", 0) + 1;
        stat_url = line.substr(pos);
      }
      else if(line.find(PLTITLETOKEN, 0) != string::npos)
      {
        int pos = line.find("=", 0) + 1;
        stat_name = line.substr(pos);

	shout_info entry;
	entry.url = stat_url;
	entry.name = stat_name;
	playlist.push_back(entry);
      }
    }
  }

  if(playlist.size())
  {
    playlistpair.first = playlist[0].name;
    playlistpair.second = playlist[0].url;
  }

  return playlistpair;
}

Shoutcast::Shoutcast()
{
  _genrelist = Shoutdownloader::fetch_genrelist();
}

list<pair<string, string> > Shoutcast::get_genrelist()
{
  if (_genrelist.size() == 0)
    _genrelist = Shoutdownloader::fetch_genrelist();

  return _genrelist;
}

list<pair<string, string> > Shoutcast::get_toplist(string genreUrl)
{
  return Shoutdownloader::fetch_toplist(genreUrl);
}

pair<string, string> Shoutcast::get_playlist(string playlistname)
{
  return Shoutdownloader::fetch_playlist(playlistname);
}

Shoutcast* Shoutcast::_instance = 0;

Shoutcast *Shoutcast::get_instance()
{
  if(_instance == 0)
    _instance = new Shoutcast();

  return _instance;
}
