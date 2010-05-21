#ifndef IMDB_MOVIE_HPP
#define IMDB_MOVIE_HPP

#include "multifile.hpp"

#include "cpp_sqlitepp.hpp"

#include <vector>
#include <string>
#include <stdio.h>

class CIMDBMovie : public Multifile
{
private:
  void save_value_to_db(const std::string& table, const std::string& value,
			const std::string& shared_table, const std::string& movie_id,
			bool new_movie, SQLDatabase *db, const std::string& idfield = "did");

  void save_double_value_to_db(const std::string& table,
			       const std::pair<std::string, std::string>& value,
			       const std::string& shared_table, const std::string& movie_id,
			       bool new_movie, SQLDatabase *db, const std::string& idfield = "aid");

  void remove_values_from_db(const std::string& shared_table, 
      const std::string& movie_id, SQLDatabase *db);

public:
  // used for entering directories and still maintaining imdb information.
  std::string real_name;

  int id;
  int db_id;
  bool thumbnailed_before_failed;
  std::string m_strDirector;
  std::string m_strWritingCredits;
  std::vector<std::string> genres;
  std::string m_strTagLine;
  std::string m_strPlotOutline;
  std::string m_strPlot;
  std::string m_strPictureURL;
  std::string m_strTitle;
  std::string m_strRuntime;
  int m_iYear;
  float m_fRating;
  std::string m_strVotes;

  typedef std::pair<std::string, std::string> actor_pair;

  std::vector<actor_pair> actors;
  int m_iTop250;

  std::string cover_path(const std::string& imdb_dir) const;
  bool Load(bool physical, SQLDatabase& db);
  void Save(bool physical, SQLDatabase& db);
  void Delete(bool physical, SQLDatabase& db, const std::string& imdb_dir);
  Multifile to_multifile();

  CIMDBMovie(Multifile e);
  CIMDBMovie();
  CIMDBMovie(const std::string& path);

  bool operator==(const CIMDBMovie& rhs) const;
};

#endif
