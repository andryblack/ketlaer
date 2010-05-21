#ifndef IMDB_HPP
#define IMDB_HPP

#include "imdb_movie.hpp"

#include <vector>
#include <string>

class CIMDBUrl
{
public:
  std::string m_strURL;
  std::string m_strTitle;
};
typedef std::vector<CIMDBUrl> IMDB_MOVIELIST;

class CIMDB
{
public:
  // IMDB
  bool FindMovie(const std::string& strMovie, IMDB_MOVIELIST& movielist);
  bool GetDetails(const CIMDBUrl& url, CIMDBMovie& movieDetails);

  // OFDB
  bool FindMovieOnOFDB(const std::string& strMovie, IMDB_MOVIELIST& movielist);
  bool GetDetailsOnOFDB(const CIMDBUrl& url, CIMDBMovie& movieDetails);

  // www.cinematografo.it
  bool FindMovieOnCMO(const std::string& strMovie, IMDB_MOVIELIST& movielist);
  bool GetDetailsOnCMO(const CIMDBUrl& url, CIMDBMovie& movieDetails);


  // for downloading covers
  bool Download(const std::string& strURL, const std::string& strFileName);

protected:

  // OFDB
  void GetURLOnOFDB(const std::string &strMovie, std::string& strURL);

  // IMDB
  void GetURLOnIMDB(const std::string& strMovie,std::string& strURL);

  // www.cinematografo.it
  void GetURLOnCMO(const std::string& strMovie,std::string& strURL);

  // Common routines
  bool GetMovieList(const std::string& strMovies, IMDB_MOVIELIST& movielist, const std::string& locale="UTF-8");
  void GetURLCommon(const std::string &strMovie, std::string& strURL);
  void remove_newlines(std::string &url_text);
  std::string remove_html_tags(const std::string& strHTML);
  bool Get(const std::string& url, std::string& content);
  std::string GetPageEncoding(const std::string& strHTML); 


};

#endif
