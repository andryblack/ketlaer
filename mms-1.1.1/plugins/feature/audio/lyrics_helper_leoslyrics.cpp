#include "lyrics_fetch_helper.hpp"

#include "wget_wrapper.hpp"

#include "tinyxml.h"
#include "tinystr.h"

#include <cc++/url.h>

using std::string;

std::string LFH_leoslyrics::get_hid() 
{
  string url = "http://api.leoslyrics.com/api_search.php?auth=MMS&artist="
	+ string_format::sanitizeurl(artist)
	+ "&songtitle="
	+ string_format::sanitizeurl(title);

  std::string searchxml, hid = "";

  if (WgetWrapper::download(url, searchxml))
  {
    TiXmlDocument searchxmldoc;
    int startpos = 0;
    string match;
    if((startpos = searchxml.find("<?xml")) != string::npos)
    {
      if(searchxmldoc.Parse(searchxml.substr(startpos).c_str(), 0 , TIXML_ENCODING_UTF8))
      {
	TiXmlElement* root_element = searchxmldoc.RootElement();
	TiXmlElement* element;

	if (root_element == 0)
	  return "";

	for (element = root_element->FirstChildElement("searchResults"); element != 0;
	     element = element->NextSiblingElement("searchResults")) {
	  element = element->FirstChildElement("result");
	  if (element != 0)
	    match = element->Attribute("exactMatch");
	  if (element != 0 && match == "true") {
	    hid = element->Attribute("hid");
            break;
          }
	  if (element == 0)
	    break;
	}

	if (!hid.empty())
	  return hid;
      }
    }
  }
  return "";
}

bool LFH_leoslyrics::fetch()
{
  std::string hid = get_hid();
  if (hid.empty()) 
    return false;

  char hid_encoded[100];

  ost::urlEncode(hid.c_str(), hid_encoded, sizeof(hid_encoded));
  string url = "http://api.leoslyrics.com/api_lyrics.php?auth=MMS&hid=" + string(hid_encoded);


  std::string searchxml;
  int ret;

  if (WgetWrapper::download(url, searchxml) > 0)
  {
    TiXmlDocument searchxmldoc;
    int startpos = 0;
    if((startpos = searchxml.find("<?xml")) != string::npos)
    {
      if(searchxmldoc.Parse(searchxml.substr(startpos).c_str(), 0 , TIXML_ENCODING_UTF8))
      {
	TiXmlNode* root_element = searchxmldoc.RootElement();
	TiXmlNode* element;
	TiXmlNode* response;

	if (root_element == 0)
    	  return false;

	response = root_element->FirstChildElement("response");
	ret = conv::atoi(response->ToElement()->Attribute("code"));

	if (ret == 0) {
	  for (element = root_element->FirstChildElement("lyric"); element != 0;
          element = element->NextSiblingElement("lyric")) {
	    element = element->FirstChildElement("text");
	    element = element->FirstChild();
	    str_lyric = string_format::trim(string_format::ValidateU8(element->Value()));
            return true;
          }
        }
      }
    }
  }
  return false;
}
