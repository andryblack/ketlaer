#include "imdb_movie.hpp"

#include "graphical_movie.hpp"

#include "assert.h"

#include <vector>

using std::vector;
using std::string;

CIMDBMovie::CIMDBMovie(Multifile m)
  : real_name(""), m_strDirector(""), m_strWritingCredits(""),
    m_strTagLine(""), m_strPlotOutline(""), m_strPlot(""),
    m_strPictureURL(""), m_strTitle(""), m_strRuntime(""), m_iYear(0),
    m_fRating(0.0), m_strVotes(""), m_iTop250(0), db_id(-1),
    thumbnailed_before_failed(false)
{
  // entry
  filetype = m.filetype;
  filenames = m.filenames;
  // simplefile
  id = m.id;
  name = m.name;
  lowercase_name = m.lowercase_name;
  path = m.path;
  type = m.type;
}

CIMDBMovie::CIMDBMovie()
  : real_name(""), m_strDirector(""), m_strWritingCredits(""),
    m_strTagLine(""), m_strPlotOutline(""), m_strPlot(""),
    m_strPictureURL(""), m_strTitle(""), m_strRuntime(""), m_iYear(0),
    m_fRating(0.0), m_strVotes(""), m_iTop250(0), db_id(-1),
    thumbnailed_before_failed(false)
{}

CIMDBMovie::CIMDBMovie(const string& filename)
  : real_name(""), m_strDirector(""), m_strWritingCredits(""),
    m_strTagLine(""), m_strPlotOutline(""), m_strPlot(""),
    m_strPictureURL(""), m_strTitle(""), m_strRuntime(""), m_iYear(0),
    m_fRating(0.0), m_strVotes(""), m_iTop250(0), db_id(-1),
    thumbnailed_before_failed(false)
{
  path = filename;
  filenames.push_back(filename);
}


void CIMDBMovie::remove_values_from_db(const std::string& shared_table,
		        const std::string& movie_id, SQLDatabase *db){
  
  char *tmp = sqlite3_mprintf("DELETE FROM %q WHERE mid=%q", shared_table.c_str(), movie_id.c_str());
  db->execute(tmp);
  sqlite3_free(tmp);
}

void CIMDBMovie::save_value_to_db(const string& table, const string& value,
				  const string& shared_table, const string& movie_id,
				  bool new_movie, SQLDatabase *db, const string& idfield)
{
  SQLQuery *q = db->query(table.c_str(), ("SELECT * FROM %t WHERE name='" + string_format::escape_db_string(value) + "'").c_str());

  if (!q || q->numberOfTuples() == 0) {
    delete q;
    char *tmp = sqlite3_mprintf(" VALUES(NULL, '%q', '%q')", value.c_str(), string_format::lowercase_utf(value).c_str());
    db->execute(("INSERT INTO " + table + tmp).c_str());
    q = db->query(table.c_str(), ("SELECT * FROM %t WHERE name='" + string_format::escape_db_string(value) + "'").c_str());
    sqlite3_free(tmp);
  }

  if (!q || q->numberOfTuples() == 0) {
    std::cerr << "strange strange error, please report" << std::endl;
    return;
  }

  SQLRow &row = (*q)[0];
  string row_id = row["id"];
  delete q;
  if (!new_movie) {
    char *tmp = sqlite3_mprintf("DELETE FROM %q WHERE %s=%q AND mid=%q",
	shared_table.c_str(), idfield.c_str(), row_id.c_str(), movie_id.c_str());
//    fprintf(stderr, "--->%s<---\n", tmp);
    
    db->execute(tmp);
    sqlite3_free(tmp);
  }

  db->execute(("INSERT INTO " + shared_table + " VALUES(NULL, '" + row_id + "', '" + movie_id + "')").c_str());
}

void CIMDBMovie::save_double_value_to_db(const string& table, const std::pair<string, string>& value,
					 const string& shared_table, const string& movie_id,
					 bool new_movie, SQLDatabase *db, const string& idfield)
{
  SQLQuery *q = db->query(table.c_str(), ("SELECT * FROM %t WHERE name='" + string_format::escape_db_string(value.first) + "'").c_str());

  if (!q || q->numberOfTuples() == 0) {
    delete q;
    char *tmp = sqlite3_mprintf(" VALUES(NULL, '%q', '%q', '%q', '%q')", value.first.c_str(),
				string_format::lowercase_utf(value.first).c_str(),
				value.second.c_str(), string_format::lowercase_utf(value.second).c_str());
    db->execute(("INSERT INTO " + table + tmp).c_str());
    q = db->query(table.c_str(), ("SELECT * FROM %t WHERE name='" + string_format::escape_db_string(value.first) + "'").c_str());
    sqlite3_free(tmp);
  }

  if (!q || q->numberOfTuples() == 0) {
    std::cerr << "strange strange error, please report" << std::endl;
    return;
  }

  SQLRow &row = (*q)[0];
  string row_id = row["id"];
  delete q;
  if (!new_movie) {
    char *tmp = sqlite3_mprintf("DELETE FROM %q WHERE %s=%q AND mid=%q",
	shared_table.c_str(), idfield.c_str(), row_id.c_str(), movie_id.c_str());
//    fprintf(stderr, "--->%s<---\n", tmp);
    db->execute(tmp);
    sqlite3_free(tmp);
  }
  db->execute(("INSERT INTO " + shared_table + " VALUES(NULL, '" + row_id + "', '" + movie_id + "')").c_str());
}

void CIMDBMovie::Delete(bool physical, SQLDatabase& db, const string& imdb_dir)
{
  string query = "DELETE FROM Movie WHERE title='" + string_format::escape_db_string(m_strTitle) + "'";

  if (physical) {
    string mpath = (path.empty()) ? filenames.front() : path;
    query = "DELETE FROM HDMovie WHERE path='" + string_format::escape_db_string(mpath) + "'";
  }

  db.execute(query.c_str());

  if (file_exists(cover_path(imdb_dir)))
    run::external_program("rm '" + cover_path(imdb_dir) + "'");
}

void CIMDBMovie::Save(bool physical, SQLDatabase& db)
{
  string query = "SELECT * FROM %t WHERE title='" + string_format::escape_db_string(m_strTitle) + "'";
  string table = "Movie";

  if (physical) {
    string mpath = (path.empty()) ? filenames.front() : path;
    query = "SELECT * FROM %t WHERE path='" + string_format::escape_db_string(mpath) + "'";
    table = "HDMovie";
  }

  // try to find the movie
  SQLQuery *q = db.query(table.c_str(), query.c_str());

  if (q && q->numberOfTuples() > 0) {

    SQLRow &movie = (*q)[0];

    movie.set("title",  m_strTitle.c_str());
    movie.set("ltitle", string_format::lowercase_utf(m_strTitle).c_str()); 
    movie.set("runtime", m_strRuntime.c_str());
    movie.set("tagline", m_strTagLine.c_str());
    movie.set("plot", m_strPlot.c_str());
    movie.set("lplot", string_format::lowercase_utf(m_strPlot).c_str());
    movie.set("rating", conv::ftos(m_fRating).c_str());
    movie.set("votes", m_strVotes.c_str());
    movie.set("year", conv::itos(m_iYear).c_str());
    movie.set("top250", conv::itos(m_iTop250).c_str());

    string mid = movie["id"];

    if (!movie.commit())
      std::cerr << "failed to update db!" << std::endl;

    delete q;

    remove_values_from_db("DMovie", mid, &db);
    save_value_to_db("Directors", m_strDirector, "DMovie", mid, false, &db);

    remove_values_from_db("WCMovie", mid, &db);
    save_double_value_to_db("WritingCredits", std::make_pair(m_strWritingCredits, "FIXME"), "WCMovie", mid, false, &db, "wcid");

    remove_values_from_db("GMovie", mid, &db);
    foreach (string& genre, genres)
      save_value_to_db("Genre", genre, "GMovie", mid, false, &db, "gid");
    
    remove_values_from_db("AMovie", mid, &db);
    foreach (actor_pair& actor, actors)
      save_double_value_to_db("Actors", actor, "AMovie", mid, false, &db, "aid");

  } else {

    delete q;

    if (physical) {

      string insert_string = "INSERT INTO " + table + " VALUES(NULL, '%q', '%q', '%q', 'FIXME', '%q', '%q', '%q', '%q', '%q', '%q', '%q', '%q', '%q')";

      char *tmp = sqlite3_mprintf(insert_string.c_str(), m_strTitle.c_str(),
				  string_format::lowercase_utf(m_strTitle).c_str(),
				  (path.empty()) ? filenames.front().c_str() : path.c_str(),
				  string_format::trim(m_strRuntime).c_str(),  m_strTagLine.c_str(),
				  string_format::lowercase_utf(m_strTagLine).c_str(), m_strPlot.c_str(),
				  string_format::lowercase_utf(m_strPlot).c_str(), conv::ftos(m_fRating).c_str(),
				  m_strVotes.c_str(), conv::itos(m_iYear).c_str(), conv::itos(m_iTop250).c_str());
      db.execute(tmp);

      sqlite3_free(tmp);

    } else {

      string insert_string = "INSERT INTO " + table + " VALUES(NULL, '%q', '%q', 'FIXME', '%q', '%q', '%q', '%q', '%q', '%q', '%q', '%q', '%q')";

      char *tmp = sqlite3_mprintf(insert_string.c_str(), m_strTitle.c_str(),
				  string_format::lowercase_utf(m_strTitle).c_str(),
				  string_format::trim(m_strRuntime).c_str(),
				  m_strTagLine.c_str(), string_format::lowercase_utf(m_strTagLine).c_str(),
				  m_strPlot.c_str(), string_format::lowercase_utf(m_strPlot).c_str(),
				  conv::ftos(m_fRating).c_str(), m_strVotes.c_str(),
				  conv::itos(m_iYear).c_str(), conv::itos(m_iTop250).c_str());
      db.execute(tmp);

      sqlite3_free(tmp);
    }

    SQLQuery *qtitle = db.query(table.c_str(), ("SELECT * FROM %t WHERE title='" + string_format::escape_db_string(m_strTitle) + "'").c_str());
    SQLRow &movie = (*qtitle)[0];
    string mid = movie["id"];
    delete qtitle;
    remove_values_from_db("DMovie", mid, &db);
    save_value_to_db("Directors", m_strDirector, "DMovie", mid, true, &db);
    
    remove_values_from_db("WCMovie", mid, &db);
    save_double_value_to_db("WritingCredits", std::make_pair(m_strWritingCredits, "FIXME"), "WCMovie", mid, true, &db, "wcid");

    remove_values_from_db("GMovie", mid, &db);
    foreach (string& genre, genres)
      save_value_to_db("Genre", genre, "GMovie", mid, true, &db, "gid");

    remove_values_from_db("AMovie", mid, &db);
    foreach (actor_pair& actor, actors)
      save_double_value_to_db("Actors", actor, "AMovie", mid, true, &db, "aid");
  }
}

bool CIMDBMovie::Load(bool physical, SQLDatabase& db)
{
  string query = "SELECT * FROM %t WHERE title='" + string_format::escape_db_string(m_strTitle) + "'";
  string table = "Movie";

  if (physical) {
    string mpath = (path.empty()) ? filenames.front() : path;
    query = "SELECT * FROM %t WHERE path='" + string_format::escape_db_string(mpath) + "'";
    table = "HDMovie";
  }

  // try to find the movie
  SQLQuery *q = db.query(table.c_str(), query.c_str());

  if (q && q->numberOfTuples() > 0) {
    SQLRow &movie = (*q)[0];
    m_strTitle = movie["title"];
    m_strRuntime = movie["runtime"];
    m_strTagLine = movie["tagline"];
    m_strPlot = movie["plot"];
    std:: string str = movie["rating"];
    m_fRating = 0.0;
    if (str.size() > 0){
      float f = 0.0;
      std::string str2 = regex_tools::extract_substr(str, "([0-9]+\\.)", true);
      m_fRating = static_cast<float>(atoi(str2.c_str()));
      str2 = regex_tools::extract_substr(str, "\\.([0-9]+)", true);
      if(str2.size() > 0){
	f = static_cast<float>(atoi(str2.c_str()));
	while (f >=1.0)
	  f/=10;
      }
      m_fRating +=f;
    }
    m_strVotes = movie["votes"];
    m_iYear = conv::atoi(movie["year"]);
    m_iTop250 = conv::atoi(movie["top250"]);

    string mid = movie["id"];

    delete q;

    q = db.query("DMovie", ("SELECT * FROM %t WHERE mid='" + mid + "'").c_str());

    for (unsigned int i = 0; i < q->numberOfTuples(); ++i) {
      SQLRow &d = (*q)[i];
      SQLQuery *q2 = db.query("Directors", ("SELECT * FROM %t WHERE id='" + d["did"] + "'").c_str());
      SQLRow &row = (*q2)[0];
      m_strDirector = row["name"];
      delete q2;
    }

    delete q;

    q = db.query("WCMovie", ("SELECT * FROM %t WHERE mid='" + mid + "'").c_str());

    for (unsigned int i = 0; i < q->numberOfTuples(); ++i) {
      SQLRow &d = (*q)[i];
      SQLQuery *q2 = db.query("WritingCredits", ("SELECT * FROM %t WHERE id='" + d["wcid"] + "'").c_str());
      SQLRow &row = (*q2)[0];
      m_strWritingCredits = row["name"];
      delete q2;
    }

    delete q;

    q = db.query("GMovie", ("SELECT * FROM %t WHERE mid='" + mid + "'").c_str());

    for (unsigned int i = 0; i < q->numberOfTuples(); ++i) {
      SQLRow &d = (*q)[i];
      SQLQuery *q2 = db.query("Genre", ("SELECT * FROM %t WHERE id='" + d["gid"] + "'").c_str());
      SQLRow &row = (*q2)[0];
      genres.push_back(row["name"]);
      delete q2;
    }

    delete q;

    q = db.query("AMovie", ("SELECT * FROM %t WHERE mid='" + mid + "'").c_str());

    for (unsigned int i = 0; i < q->numberOfTuples(); ++i) {
      SQLRow &d = (*q)[i];
      SQLQuery *q2 = db.query("Actors", ("SELECT * FROM %t WHERE id='" + d["aid"] + "'").c_str());
      SQLRow &row = (*q2)[0];
      actors.push_back(std::make_pair(row["name"], row["role"]));
      delete q2;
    }

    delete q;

    return true;

  } else{
    if (q)
      delete q;

    return false;
  }
}

string CIMDBMovie::cover_path(const string& imdb_dir) const
{
  // Try to find a cover in the movie folder to enable users to overwrite the
  // imdb cover
  string cover = find_cover_in_current_dir();

  if (!cover.empty())
    return cover;
  else
    // cover not found in directory of movie, use the one from imdb
    return imdb_dir + name + "_cover.jpg";
}

Multifile CIMDBMovie::to_multifile()
{
  Multifile m;

  // entry
  m.filetype = filetype;
  m.filenames = filenames;
  // simplefile
  m.id = id;
  m.name = name;
  m.lowercase_name = lowercase_name;
  m.path = path;
  m.type = type;

  return m;
}

bool CIMDBMovie::operator==(const CIMDBMovie& rhs) const
{
  return db_id == rhs.db_id;
}
