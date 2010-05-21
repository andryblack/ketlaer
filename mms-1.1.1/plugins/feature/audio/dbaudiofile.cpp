#include "dbaudiofile.hpp"

#include "graphical_audio.hpp"

#include "common.hpp"

#include <fileref.h>
#include <tag.h>

using namespace TagLib;

using std::string;

bool Dbaudiofile::is_fetched() const
{
  return fetched;
}

bool Dbaudiofile::operator==(const Dbaudiofile& rhs) const
{
  return id == rhs.id && db_id == rhs.db_id;
}

string Dbaudiofile::to_string() const
{
  string result;

  if (artist.empty() || title.empty())
    result = name;
  else {

    string title_trimmed = string_format::convert(string_format::trim(title), true);
    string artist_trimmed = string_format::convert(string_format::trim(artist), true);
    string album_trimmed = string_format::convert(string_format::trim(album), true);

    if (!artist.empty())
      result += artist_trimmed;
    if (!album.empty())
      result += " - " + album_trimmed;
    if (!title.empty())
      result += " - " + title_trimmed;
  }

  return result;
}

string Dbaudiofile::short_to_string() const
{
  string result;

  if (artist.empty() || title.empty())
    result = name;
  else {

    string title_trimmed = string_format::convert(string_format::trim(title), true);
    string artist_trimmed = string_format::convert(string_format::trim(artist), true);
    string album_trimmed = string_format::convert(string_format::trim(album), true);

    // FIXME: lcd size

    if (title_trimmed.size() > 20)
      result = title_trimmed.substr(0, 20);
    else if (title_trimmed.size() + artist_trimmed.size() > 20) {
      result = (artist_trimmed + " - " + title_trimmed);
      if (result.size() > 20)
	result = result.substr(result.size()-20);
    } else {
      result = (artist_trimmed + " - " + album_trimmed + " - " + title_trimmed);
      if (result.size() > 20)
	result = result.substr(result.size()-20);
    }
  }

  return result;
}

string Dbaudiofile::get_name_from_file_in_picture_dir() const
{
  if (title.empty())
    return name;
  else
    return string_format::convert(string_format::trim(title), true);
}

void Dbaudiofile::set_values(const Dbaudiofile& s)
{
  id = s.id;
  media_id = s.media_id;
  db_id = s.db_id;
  playlist_id = s.playlist_id;
  name = s.name;
  lowercase_name = s.lowercase_name;
  path = s.path;
  type = s.type;
}

Dbaudiofile::Dbaudiofile()
  : artist(""), album(""), title(""), year(0), bitrate(0), length(0),
    rating(0), track(0), fetched(false), playlist_id(-1)
{}

Dbaudiofile::Dbaudiofile(const GSimplefile& s)
  : GSimplefile(s), artist(""), album(""), title(""), year(0), bitrate(0),
    length(0), rating(0), track(0), fetched(false), playlist_id(-1)
{}

Dbaudiofile::Dbaudiofile(int id)
  : artist(""), album(""), title(""),
    year(0), bitrate(0), length(0), rating(0), track(0), playlist_id(-1)
{
  db_id = id;

  GraphicalAudio *ga = get_class<GraphicalAudio>(dgettext("mms-audio", "Audio"));

  ga->db_mutex.enterMutex();

  SQLQuery *q = ga->db.query("Folders", ("SELECT filename, is_folder FROM %t WHERE id='" + conv::itos(id) + "'").c_str());
  if (q && q->numberOfTuples() > 0) {
    path = (*q)[0]["filename"];
    if ((*q)[0]["is_folder"] == "0")
      get_info_from_db(path, ga);
  } else
    assert(false);

  ga->db_mutex.leaveMutex();
}

Dbaudiofile::Dbaudiofile(const string& p)
  : artist(""), album(""), title(""),
    year(0), bitrate(0), length(0), rating(0), track(0)
{
  GraphicalAudio *ga = get_class<GraphicalAudio>(dgettext("mms-audio", "Audio"));

  ga->db_mutex.enterMutex();
  get_info_from_db(p, ga);
  ga->db_mutex.leaveMutex();

  path = p;
}

void Dbaudiofile::get_info_from_db(const string& path, GraphicalAudio *ga)
{
  SQLQuery *q = ga->db.query("Audio", ("SELECT * FROM %t WHERE filename='" + string_format::escape_db_string(path) + "'").c_str());
  if (q && q->numberOfTuples() > 0) {

    SQLRow &row = (*q)[0];

    title = row["Title"];
    bitrate = conv::atoi(row["Bitrate"]);
    length = conv::atoi(row["Length"]);
    track= conv::atoi(row["Track"]);

    SQLQuery *q2 = ga->db.query("Artist", ("SELECT name FROM %t WHERE id='" + row["Artist"] + "'").c_str());
    if (q2 && q2->numberOfTuples() > 0) {
      SQLRow &c_row = (*q2)[0];
      artist = c_row["name"];
    }
    delete q2;

    q2 = ga->db.query("Album", ("SELECT name FROM %t WHERE id='" + row["Album"] + "'").c_str());
    if (q2 && q2->numberOfTuples() > 0) {
      SQLRow &c_row = (*q2)[0];
      album = c_row["name"];
    }
    delete q2;

  } else {

    FileRef fileref(path.c_str(), true, AudioProperties::Accurate);

    if (!fileref.isNull() && fileref.tag()) {
      artist = fileref.tag()->artist().toCString(true);
      title = fileref.tag()->title().toCString(true);
      album = fileref.tag()->album().toCString(true);
      year = fileref.tag()->year();
      track = fileref.tag()->track();
    }

    if (!fileref.isNull() && fileref.audioProperties()) {
      length = fileref.audioProperties()->length();
      bitrate = fileref.audioProperties()->bitrate();
    }

    int artist_id = 0;

    SQLQuery *q2 = ga->db.query("Artist", ("SELECT id FROM %t WHERE name='" + string_format::escape_db_string(artist) + "'").c_str());
    if (q2 && q2->numberOfTuples() > 0) {
      SQLRow &row = (*q2)[0];
      artist_id = conv::atoi(row["id"]);
    }
    delete q2;

    if (artist_id == 0) {
      char *tmp = sqlite3_mprintf("INSERT INTO Artist VALUES(NULL, '%q', '%q')", artist.c_str(), string_format::lowercase(artist).c_str());
      ga->db.execute(tmp);
      sqlite3_free(tmp);

      q2 = ga->db.query("Artist", ("SELECT id FROM %t WHERE name='" + string_format::escape_db_string(artist) + "'").c_str());
      if (q2 && q2->numberOfTuples() > 0) {
	SQLRow &row = (*q2)[0];
	artist_id = conv::atoi(row["id"]);
      }
      delete q2;
    }

    int album_id = 0;

    q2 = ga->db.query("Album", ("SELECT id FROM %t WHERE name='" + string_format::escape_db_string(album) + "'").c_str());
    if (q2 && q2->numberOfTuples() > 0) {
      SQLRow &row = (*q2)[0];
      album_id = conv::atoi(row["id"]);
    }
    delete q2;

    if (album_id == 0) {
      char *tmp = sqlite3_mprintf("INSERT INTO Album VALUES(NULL, '%q', '%q')", album.c_str(), string_format::lowercase(album).c_str());
      ga->db.execute(tmp);
      sqlite3_free(tmp);

      q2 = ga->db.query("Album", ("SELECT id FROM %t WHERE name='" + string_format::escape_db_string(album) + "'").c_str());
      if (q2 && q2->numberOfTuples() > 0) {
	SQLRow &row = (*q2)[0];
	album_id = conv::atoi(row["id"]);
      }
      delete q2;
    }

    char *tmp = sqlite3_mprintf("INSERT INTO Audio VALUES(NULL, '%q', '%q', '%q', '%q', '%q', '%q', '%q', '%q')",
				conv::itos(artist_id).c_str(), conv::itos(album_id).c_str(),
				title.c_str(), string_format::lowercase(title).c_str(), path.c_str(),
				conv::ftos(bitrate).c_str(), conv::itos(length).c_str(),
				conv::itos(track).c_str());
    ga->db.execute(tmp);
    sqlite3_free(tmp);
  }

  delete q;

  fetched = true;

#ifdef use_imms
  rating = ga->imms->get_rating(path);
#endif
}
