
#include "imdb.hpp"

#include "print.hpp"
#include "common.hpp"
#include "gettext.hpp"

#include "wget_wrapper.hpp"

#include "boost.hpp"

// saving
#include <fstream>
#include <iostream>

using std::string;

bool CIMDB::Get(const string& url, string& content)
{
  if (!WgetWrapper::download(string_format::convert_to_locale(url), content)) {
    print_warning(dgettext("mms-movie", "Unable to retrieve web page ") + url, "IMDB");
    return false;
  } else
    return true;
}

bool CIMDB::GetMovieList(const std::string& strMovies, IMDB_MOVIELIST& movielist, const string& locale){
  if (strMovies != ""){
    CIMDBUrl url;
    std::vector<string> vec = regex_tools::regex_matches(strMovies,
	"<a href=\"(/title/tt[0-9]+/)\".*?;\">[ ]*(.+?)</a>[ ]*(\\(.+?)<" , true, true);
    for (unsigned int t = 0; t < vec.size(); t+=3){
      if (regex_tools::regex_str_match(vec[t+2], "(\\(VG\\))$", true))
	continue;
      url.m_strURL=string_format::str_printf("http://us.imdb.com%s", vec[t].c_str());
      string str = string_format::str_printf("%s %s", remove_html_tags(vec[t+1]).c_str(), vec[t+2].c_str());

      /* Remove trailing ordinal numbers .*/
      /* fprintf(stderr, "\"%s\"\n", str.c_str()); */
      str = regex_tools::regex_replace(str, "", "^\\&nbsp;{1,2}[0-9]\\.", false, false);
      url.m_strTitle = string_format::ConvertHTMLToAnsi(str.c_str(),locale.c_str());
      movielist.push_back(url);
    }
    return (vec.size() > 0);
  }
  else
    return false;
}


bool CIMDB::FindMovie(const string &strMovie,IMDB_MOVIELIST& movielist)
{

  /* We're looking up on IMDB
   * using a slightly different approach than the one used before */

  if (strMovie.size() == 0)
    return false;

  movielist.clear();
  string strURL,strHTML;
  GetURLOnIMDB(strMovie,strURL);
  
  if (!Get(strURL,strHTML))
    return false;

  if (strHTML.size()==0)
    return false;

  /* retrieve page encoding */
  string encoding = GetPageEncoding(strHTML);

  if(regex_tools::regex_str_match(strHTML,"(<a href=\".+?add=[0-9]+\")", true)){

    /* We've been redirected to the actual movie page 
     * 
     * Movie pages (this one is "the Matrix") contains something like this:
     * <a href="/mymovies/list?pending&amp;add=0133093"
     */

    CIMDBUrl url;
    string str = regex_tools::extract_substr(strHTML, "^<title>(.+?)</title>", true);

    if (str == "") /* No movie title? */
      return false;

    url.m_strTitle = string_format::ConvertHTMLToAnsi(str.c_str(), encoding.c_str());
    url.m_strTitle = string_format::trim(url.m_strTitle);
    url.m_strURL   = strURL;
    movielist.push_back(url);
    return true;
  }

  /* Process Exact matches */
  string str = regex_tools::extract_substr(strHTML, "<p><b>Titles \\(Exact Matches\\).*?<table>(.*?)</table>" , true);
  GetMovieList(str, movielist, encoding);
  
  /* Process Popular titles */
  str = regex_tools::extract_substr(strHTML, "<p><b>Popular Titles</b>(.*?)</table>" , true);
  GetMovieList(str, movielist, encoding);

  /* Process partial matches */
  str = regex_tools::extract_substr(strHTML, "<p><b>Titles \\(Partial Matches\\).*?<table>(.*?)</table>" , true);
  GetMovieList(str, movielist, encoding);

  /* Process Approx matches */
  str = regex_tools::extract_substr(strHTML, "<p><b>Titles \\(Approx Matches\\).*?<table>(.*?)</table>" , true);
  GetMovieList(str, movielist, encoding);

 return true;
}


bool CIMDB::GetDetails(const CIMDBUrl& url, CIMDBMovie& movieDetails)
{
  string strURL = url.m_strURL;
  string strHTML;
  movieDetails.m_strTitle=string_format::trim(url.m_strTitle);
  movieDetails.m_strDirector=" ";
  movieDetails.m_strWritingCredits=" ";
  movieDetails.genres.clear();
  movieDetails.m_strTagLine=" ";
  movieDetails.m_strPlotOutline=" ";
  movieDetails.m_strPlot=" ";
  movieDetails.m_strPictureURL="";
  movieDetails.m_strRuntime="";
  movieDetails.m_iYear=0;
  movieDetails.m_fRating=0.0;
  movieDetails.m_strVotes="";
  movieDetails.actors.clear();
  movieDetails.m_iTop250=0;

  if (!Get(strURL,strHTML)|| strHTML.size() == 0)
    return false;
  
  /* retrieve page encoding */
  string encoding = regex_tools::extract_substr(strHTML,
      "<meta http-equiv=\"content-type\"[ ]*content=\"text/html;[ ]*charset=(.+?)\"[ ]*/?>", true);
  if(encoding.size() == 0)
    encoding = "UTF-8";

  /* get year */
  string str = regex_tools::extract_substr(strHTML, "\\(<a href=\"/Sections/Years/[0-9]+/\">([0-9]+)</a>\\)", true);

  if (str.size() > 0)
    movieDetails.m_iYear=atoi(str.c_str());

  /* get directors */
  str = regex_tools::extract_substr(strHTML, "<h5>Director.*?:</h5>(.+?)</div>", true);

  if (str.size()>0){
    std::vector<string> vec = regex_tools::regex_matches(str, "<a href=\"/name/nm[0-9]*/\">(.+?)</a>");

    for (unsigned int t = 0; t < vec.size(); t++){
      str = string_format::ConvertHTMLToAnsi(vec[t].c_str(), encoding.c_str());
      if (t > 0 && str.size() > 0)
	movieDetails.m_strDirector+=", ";
      movieDetails.m_strDirector+=str;
    }
  }

  /* get writers */
  str = regex_tools::extract_substr(strHTML, "<h5>Writer.*?:</h5>(.+?)</div>", true);
  
  if (str.size() > 0){
    std::vector<string> vec = regex_tools::regex_matches(str, "<a href=\"/name/nm[0-9]*/\">(.+?)</a>");
    for (unsigned int t = 0; t < vec.size(); t++){
      str = string_format::ConvertHTMLToAnsi(vec[t].c_str(), encoding.c_str());
      if (t > 0 && str.size() > 0)
	movieDetails.m_strWritingCredits+=", ";
      movieDetails.m_strWritingCredits+=str;
    }
  }

  /* get genres */
  str = regex_tools::extract_substr(strHTML, "<h5>Genre:</h5>(.+?)</div>", true);

  if (str.size() > 0)
    movieDetails.genres = regex_tools::regex_matches(str, "<a href=\"/Sections/Genres/.+?/\">(.+?)</a>");

  /* get runtime */
  movieDetails.m_strRuntime = string_format::trim(regex_tools::extract_substr(strHTML,"<h5>Runtime:</h5>(.+?)</div>", true));

  // some movies have alternate run times, but there's no space on screen to display more than just one */
  movieDetails.m_strRuntime = regex_tools::regex_replace(movieDetails.m_strRuntime, "", "\\|.*", true, false);

  /* get tagline */
  movieDetails.m_strTagLine = string_format::ConvertHTMLToAnsi(regex_tools::extract_substr(strHTML,"<h5>Tagline:</h5>(.+?)<", true).c_str(), encoding.c_str());

  /* cast */
  str = regex_tools::extract_substr(strHTML,"st</h3>.*?<table.*?>(.*?)</table>", true);
  if (str.size() > 0){
    /* clean from some tags... */
    str = regex_tools::regex_replace(str, "", "<a href=\"/character/ch[0-9]+/\">", true, true);
    str = regex_tools::regex_replace(str, "", "<a href=\"/name/nm[0-9]+/\">", true, true);
    std::vector<string> vec = regex_tools::regex_matches(str, "<td class=\"nm\">(.+?)</a></td>.*?<td class=\"char\">(.+?)</", true, true);
    
    for (unsigned int t = 0; t < vec.size(); t+=2){
      string nm = string_format::ConvertHTMLToAnsi(remove_html_tags(vec[t]), encoding);
      string ch = string_format::ConvertHTMLToAnsi(remove_html_tags(vec[t+1]), encoding);
      movieDetails.actors.push_back(std::make_pair(nm, ch));
    }
  }

  /* top 250? */
  str = regex_tools::extract_substr(strHTML, "<a href=\"/chart/top?.+?\">Top 250: #([0-9]+)</a>", true);
  if (str.size() > 0)
    movieDetails.m_iTop250=atoi(str.c_str());

  /* Poster */
  str = regex_tools::extract_substr(strHTML, "<a name=\"poster\".+?src=\"(http://.+?)\"", true);

  if (str.size() == 0)
     str = regex_tools::extract_substr(strHTML, "title=\"Poster Not Submitted\">.*?src=\"(http:.+?)\"", true);
  
  movieDetails.m_strPictureURL = str;

  /* Plot */
  str = regex_tools::extract_substr(strHTML, "<h5>Plot:</h5>(.+?)<", true);
  regex_tools::regex_replace(str, "", "\\|[ ]*$", true, true);
  movieDetails.m_strPlot = string_format::ConvertHTMLToAnsi(str, encoding);


  /* User rating */
  str = regex_tools::extract_substr(strHTML, "<h5>User Rating:</h5>.*?<b>(.+?)/.*</b>", true);
  if (str.size() > 0){
    /* movieDetails.m_fRating = static_cast<float>(atof(str.c_str())); this won't work: atof uses the locale settings for the dec. 
     *  separator, which might differ from '.', so we do a little trick
     */
    float f = 0.0;
    std::string str2 = regex_tools::extract_substr(str, "([0-9]+\\.)", true);
    movieDetails.m_fRating = static_cast<float>(atoi(str2.c_str()));
    str2 = regex_tools::extract_substr(str, "\\.([0-9]+)", true);
    if(str2.size() > 0){
      f = static_cast<float>(atoi(str2.c_str()));
      while (f >=1.0)
	f/=10;
    }
    movieDetails.m_fRating +=f;
  }

  /* votes and we're done */
  if (movieDetails.m_fRating > 0.0){
    str = regex_tools::extract_substr(strHTML, ";<a href=\"ratings\" class=.*?>(.+?) votes</a>", true);
    if (str.size() > 0)
      movieDetails.m_strVotes = str;
    else
      movieDetails.m_strVotes = "0";
  }
  return true;

}

void CIMDB::remove_newlines(string &url_text)
{
  string::size_type pos;

  while (url_text[0] == '\n')
    url_text = url_text.substr(1);

  while ((pos = url_text.rfind("\n")) != string::npos)
    url_text = url_text.substr(0, pos);
}

string CIMDB::remove_html_tags(const std::string& strHTML){
  return regex_tools::regex_replace(strHTML, "", "<[^>]*>", true, true);
}

bool CIMDB::FindMovieOnOFDB(const string &strMovie, IMDB_MOVIELIST& movielist)
{
  if (strMovie.size() == 0)
    return false;

  movielist.clear();
  string strURL,strHTML;
  GetURLOnOFDB(strMovie,strURL);
  if (!Get(strURL,strHTML))
    return false;
  
  if (strHTML.size()==0)
    return false;

  /* retrieve page encoding */
  string encoding = GetPageEncoding(strHTML);

  string str = regex_tools::extract_substr(strHTML, "(<b>Titel:</b>.*?)$", true);
  if (str.size() == 0)
    return false;

  /* retrieve url, title and year */
  std::vector<string> vec = regex_tools::regex_matches(str,
      "<a href=\"(.+?)\" onmouseover=\".+?\">(.+?)<.*?</font>[ ]*(\\(.+?)</a", true, true);
  CIMDBUrl url;
  for (unsigned int t = 0; t < vec.size(); t+=3){
    str = string_format::convert_to_locale(vec[t], encoding);
    url.m_strURL=string_format::str_printf("http://www.ofdb.de/%s", str.c_str());
    str = string_format::str_printf("%s %s", vec[t+1].c_str(), vec[t+2].c_str());
    url.m_strTitle = string_format::ConvertHTMLToAnsi(str.c_str(),encoding.c_str());
    movielist.push_back(url);
  }

  return (vec.size() > 0);

}

bool CIMDB::GetDetailsOnOFDB(const CIMDBUrl& url, CIMDBMovie& movieDetails)
{
  string strHTML;
  string strURL = url.m_strURL;

  if (!Get(strURL, strHTML) || strHTML.size() == 0)
    return false;

  string encoding = regex_tools::extract_substr(strHTML,
      "<meta http-equiv=\"content-type\"[ ]*content=\"text/html;[ ]*charset=(.+?)\"[ ]*/?>", true);
  if(encoding.size() == 0)
    encoding = "UTF-8";


  /* OFDB doesn't provide all the details we need, but the movie page
   * contains a link to the imdb page */
  string str = regex_tools::extract_substr(strHTML, "<a href=\"http://german.imdb.com/Title\\?(.+?)\"", true);

  if (str.size() > 0){
    string uri = string_format::str_printf("http://www.imdb.com/title/tt%s", str.c_str());
    CIMDBUrl tmp_url = url;
    tmp_url.m_strURL = uri;

    GetDetails(tmp_url, movieDetails);
  }

  movieDetails.m_strTitle = url.m_strTitle;

  /* Back to OFDB */

  /* from OFDB we fetch: genres, tagline, plot, rating and movie poster */

  
  /* genres */
  str = regex_tools::extract_substr(strHTML, ">Genre\\(s\\):(.+?)</tr>", true);
  if (str.size() > 0){
    std::vector<string> vec = regex_tools::regex_matches(str, "<a href=\"view.php.*?\">(.+?)</a>", true, true);
    if (vec.size() > 0)
      movieDetails.genres.clear(); /* we don't need genres from IMDB */

    for (unsigned int t = 0; t < vec.size(); t++)
      movieDetails.genres.push_back(string_format::ConvertHTMLToAnsi(string_format::trim(vec[t]).c_str(), encoding.c_str()));
  }

  movieDetails.m_strTagLine = "";
  movieDetails.m_strPlotOutline = string_format::ConvertHTMLToAnsi(
      regex_tools::extract_substr(strHTML,"<b>Inhalt:</b>[ ]?(.+?)<", true).c_str(), encoding.c_str());

  /* Plot */
  str = regex_tools::extract_substr(strHTML,"<a href=\"(plot/.+?)\">", true);

  if (str.size() > 0){
    string strPlotURL = "http://www.ofdb.de/" + str;
    string strPlotHTML;
    if (Get(strPlotURL, strPlotHTML)){
      if (strPlotHTML.size() != 0) {
	str = regex_tools::extract_substr(strPlotHTML, "Eine Inhaltsangabe von(.*?)</font>", true);
	str = regex_tools::extract_substr(str, ".*<br>(.+)", true);
	str = remove_html_tags(str);
	if (str.size() > 0)
	  movieDetails.m_strPlot = string_format::ConvertHTMLToAnsi(str.c_str(), encoding.c_str());
      }
    }
  }

  /* Poster */
  movieDetails.m_strPictureURL = regex_tools::extract_substr(strHTML,"<img src=\"(http://img.ofdb.de/film/.+?)\"", true);
  movieDetails.m_strPictureURL = string_format::ConvertHTMLToAnsi(movieDetails.m_strPictureURL.c_str(), encoding.c_str());

  return true;
}


void CIMDB::GetURLOnOFDB(const string &strMovie, string& strURL)
{
  GetURLCommon(strMovie, strURL);
  strURL = "http://www.ofdb.de/view.php?page=suchergebnis&SText=" + string_format::sanitizeurl(strURL);
}

void CIMDB::GetURLOnIMDB(const string &strMovie, string& strURL)
{
  GetURLCommon(strMovie, strURL);
  strURL = string_format::convert_locale(strURL, "UTF-8", "ISO-8859-1");
  strURL = "http://us.imdb.com/Tsearch?title=" +
    string_format::sanitizeurl(strURL);
}

/* www.cinematografo.it */
/* this site does not provide information like user ratings */
bool CIMDB::FindMovieOnCMO(const std::string& strMovie, IMDB_MOVIELIST& movielist){
  if (strMovie.size() == 0)
    return false;

  movielist.clear();
  string strURL,strHTML;
  GetURLOnCMO(strMovie,strURL);
  if (!Get(strURL,strHTML))
    return false;

  if (strHTML.size()==0)
    return false;

  /* retrieve page encoding */
  string encoding = GetPageEncoding(strHTML);

  string str = regex_tools::extract_substr(strHTML, "(<a href=\"schedafilm.jsp.+?)</ul>", true);
  if (str.size() == 0)
    return false;

  /* format of movie entries
   * <a href="schedafilm.jsp?codice=25700"><b>E... la vita è bella</b>&nbsp;&nbsp;[1985]</a>
   */

  /* retrieve url, title and year */
  std::vector<string> vec = regex_tools::regex_matches(str,
      "<a href=\"(schedafilm.jsp.+?)\"><b>(.+?)</b>.*?\\[(.+?)]", true, true);
  
  CIMDBUrl url;
  for (unsigned int t = 0; t < vec.size(); t+=3){
    str = string_format::convert_to_locale(vec[t], encoding);
    url.m_strURL=string_format::str_printf("http://www.cinematografo.it/bancadati/consultazione/%s&completa=si",
	str.c_str());

    str = string_format::str_printf("%s (%s)", vec[t+1].c_str(), vec[t+2].c_str());
    url.m_strTitle = string_format::ConvertHTMLToAnsi(str.c_str(),encoding.c_str());
    movielist.push_back(url);
  }

  return (movielist.size() > 0);
}

   
bool CIMDB::GetDetailsOnCMO(const CIMDBUrl& url, CIMDBMovie& movieDetails){
  string strURL = url.m_strURL;
  string strHTML;
  movieDetails.m_strTitle=string_format::trim(url.m_strTitle);
  movieDetails.m_strDirector=" ";
  movieDetails.m_strWritingCredits=" ";
  movieDetails.genres.clear();
  movieDetails.m_strTagLine=" ";
  movieDetails.m_strPlotOutline=" ";
  movieDetails.m_strPlot=" ";
  movieDetails.m_strPictureURL="";
  movieDetails.m_strRuntime="";
  movieDetails.m_iYear=0;
  movieDetails.m_fRating=0.0;
  movieDetails.m_strVotes="0";
  movieDetails.actors.clear();
  movieDetails.m_iTop250=0;
  if (!Get(strURL,strHTML)|| strHTML.size() == 0)
    return false;

  /* retrieve page encoding */
  string encoding = GetPageEncoding(strHTML);

  /* retrieve year */
  string str = regex_tools::extract_substr(strHTML, "<font class=.+?>Anno</font>.*?<td class=.*?>.*?([0-9]{2,4}.*?)</td>", true);
  if (str.size() > 0)
    movieDetails.m_iYear=atoi(str.c_str());

  /* retrieve running time */
  str = regex_tools::extract_substr(strHTML,
      "<font class=.+?>Durata</font>.*?<td valign=\"top\" class=.+?>(.*?)</td>", true);
  str=string_format::trim(str);

  if (str.size() > 0)
    movieDetails.m_strRuntime= str + " min";

  /* retrieve plot */
  str = regex_tools::extract_substr(strHTML,
      ">Trama</font>(.*?)<br><br>", true);

  movieDetails.m_strPlot = string_format::trim(string_format::ConvertHTMLToAnsi(remove_html_tags(str), encoding));

  /* retrieve genres */
  str = regex_tools::extract_substr(strHTML,">Genere</font>(.+?)</tr>", true);
  str = string_format::trim(remove_html_tags(str));

  std::vector<string> vec = regex_tools::regex_matches(str, "([^,]+),?", true, true);
  for (unsigned int t = 0; t < vec.size(); t++){
    str = string_format::trim(string_format::ConvertHTMLToAnsi(vec[t], encoding));
    if (str.size() > 0)
      movieDetails.genres.push_back(str);
  }

  /* get directors */
  str = regex_tools::extract_substr(strHTML, ">Regia</font>(.+?)<td colspan", true);
  vec = regex_tools::regex_matches(str, "target='_self'>(.+?)<", true, true);
  for (unsigned int t = 0; t < vec.size(); t++){
    str = string_format::trim(string_format::ConvertHTMLToAnsi(vec[t], encoding));
    if (t > 0 && str.size() > 0)
      movieDetails.m_strDirector+=", ";
    
    movieDetails.m_strDirector+=str;
  }

  /* get writers */
  str = regex_tools::extract_substr(strHTML, ">Sceneggiatura</font>(.+?)<td colspan", true);
  vec = regex_tools::regex_matches(str, "target='_self'>(.+?)<", true, true);
  for (unsigned int t = 0; t < vec.size(); t++){
    str = string_format::trim(string_format::ConvertHTMLToAnsi(vec[t], encoding));
    if (t > 0 && str.size() > 0)
      movieDetails.m_strWritingCredits+=", ";

    movieDetails.m_strWritingCredits+=str;
  }

  /* Actors */
  str = regex_tools::extract_substr(strHTML, ">Attori</font>(.+?)<td colspan", true);
  vec = regex_tools::regex_matches(str, "target='_self'>(.+?)<.+?>(.*?)</font></td>", true, true);
  for (unsigned int t = 0; t < vec.size(); t+=2){
    string nm = string_format::ConvertHTMLToAnsi(remove_html_tags(vec[t]), encoding);
    string ch = string_format::ConvertHTMLToAnsi(remove_html_tags(vec[t+1]), encoding);
    movieDetails.actors.push_back(std::make_pair(nm, ch));
  }

  /* Movie poster */
  str = regex_tools::extract_substr(strHTML, "class=\"imgScheda\" src=\"(.+?)\"", true);

  if (str.size() > 0){
    if (!regex_tools::regex_str_match(str, "^http:", false))
      str = "http://www.cinematografo.it/bancadati/consultazione/" + str;
    movieDetails.m_strPictureURL = str;
  }
  else {
    /* TODO: www.cinematografo.it doesn't have a generic poster for movies 
     * that do not have their own one. As a result, users may get an empty 
     * frame on screen and a lot of warnings on console. How do we handle 
     * this? */
  }

  return true;
}


void CIMDB::GetURLOnCMO(const std::string& strMovie,std::string& strURL){
  GetURLCommon(strMovie, strURL);
  strURL = string_format::convert_locale(strURL, "UTF-8", "ISO-8859-1");
  strURL = "http://www.cinematografo.it/bancadati/consultazione/trovatitoli.jsp?startrighe=0&endrighe=20&tipo=CONTIENEPAROLE&word=" + 
    string_format::sanitizeurl(strURL);
}



bool CIMDB::Download(const string& strURL, const string& strFileName)
{
  WgetWrapper::download_to_file(strURL, strFileName);
  return true;
}


void CIMDB::GetURLCommon(const string &strMovie, string& strURL)
{
  strURL = string_format::convert(strMovie);
  /* remove -CDx stuff.... */
  strURL = regex_tools::regex_replace(strURL, "", "[\\.\\- ]CD[0-9]{1,2}", true, true);

  /* skip file extension (.avi, ,.mpeg, etc.) */
  strURL = regex_tools::regex_replace(strURL, "", "(\\..{2,4})$", false, false);

  /* skip everthing between () and [] */
  strURL =  regex_tools::regex_replace(strURL, "", "([ ]*\\[.*?\\]|\\(.*?\\)[ ]*)", true, false);

  /* convert some characters to spaces */
  strURL = regex_tools::regex_replace(strURL, " ", "([ _\\(\\)\\[\\]\\-;&]+)", true, false);
  /* trim string, convert spaces to '+' and remove doubles in the process */
  strURL = string_format::trim(strURL);
  strURL = regex_tools::regex_replace(strURL, " ", "([ ]+)", true, false);

}

string CIMDB::GetPageEncoding(const string& strHTML){
  string encoding = regex_tools::extract_substr(strHTML,
      "<meta http-equiv=\"content-type\"[ ]*content=\"text/html;[ ]*charset=(.+?)\"[ ]*/?>", true);
  if(encoding.size() == 0)
    encoding = "UTF-8";

  return encoding;
}
