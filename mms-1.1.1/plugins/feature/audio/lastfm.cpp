#include "lastfm.hpp"

#include "wget_wrapper.hpp"

#include "audio_config.hpp"
#include "common-feature.hpp"

using std::string;
using std::vector;

string LastFM::domain = "http://ws.audioscrobbler.com";
string LastFM::key = "b25b959554ed76058ac220b7b2e0a026";

void LastFM::end_of_song(const string& artist, const string& album,
			 const string& title, int length, int timeplayed)
{
  AudioConfig *audio_conf = S_AudioConfig::get_instance();
  if (audio_conf->p_last_fm())
    if (timeplayed > 20 ) /* hardcoded 20 seconds delay before submission */
      run::external_program(audio_conf->p_last_fm_path() + " --artist \"" + artist + "\" --album \"" + album + "\" --title \"" + title + "\" --length " + conv::itos(length));
}

vector<string> LastFM::lookup_and_insert_genres(const string& artist, SQLDatabase& db, ost::Mutex& db_mutex)
{
  vector<string> genres = lookup_genres(artist);
  insert_genres(genres, artist, db, db_mutex);
  return genres;
}

vector<string> LastFM::lookup_genres(const string& artist)
{
  string content;
  WgetWrapper::download(domain + "/2.0/?method=artist.gettoptags&artist=" + artist + "&api_key=" + key, content);
  vector<string> genres;

  while (genres.size() < 3) {
    int name_start = content.find("<name>");
    if (name_start != string::npos) {
      int name_end = content.find("</name>");
      if (name_end != string::npos) {
	genres.push_back(content.substr(name_start + 6, name_end - name_start - 6));
	content = content.substr(name_end+6);
      }
      else
	break;
    }
    else
      break;
  }

  return genres;
}

void LastFM::insert_genres(const vector<string>& genres, const string& artist,
			   SQLDatabase& db, ost::Mutex& db_mutex)
{
  if (genres.size() == 0)
    return;

  db_mutex.enterMutex();

  // first find the artist id
  string artist_id;

  SQLQuery *q = db.query("Artist", ("SELECT id FROM %t WHERE lname = '" + string_format::lowercase(artist) + "'").c_str());
  if (q->numberOfTuples() > 0) {
    artist_id = (*q)[0]["id"];
    delete q;
  } else {
    delete q;
    db_mutex.leaveMutex();
    return;
  }

  foreach (const string& genre, genres) {
    q = db.query("Genre", ("SELECT id FROM %t WHERE name = '" + genre + "'").c_str());

    int id;

    if (q->numberOfTuples() == 0) {
      char *tmp = sqlite3_mprintf("INSERT INTO Genre VALUES(NULL, '%q')", genre.c_str());
      db.execute(tmp);
      sqlite3_free(tmp);

      id = db.last_index();
    } else {
      id = conv::atoi((*q)[0]["id"]);
    }

    delete q;

    char *tmp = sqlite3_mprintf("INSERT INTO GAudio VALUES(NULL, '%q', '%q')", 
				artist_id.c_str(), conv::itos(id).c_str());
    db.execute(tmp);
    sqlite3_free(tmp);
  }

  db_mutex.leaveMutex();
}

std::vector<LastFM::Track> LastFM::lookup_similar_tracks(const std::string& artist, const std::string& track)
{
  string content;
  WgetWrapper::download(domain + "/2.0/?method=track.getsimilar&artist=" + artist + "&track=" + track + "&api_key=" + key, content);

  return extract_tracks(content);
}


std::vector<LastFM::Track> LastFM::extract_tracks(std::string& content)
{
  vector<Track> tracks;

  while (true) {
    int name_start = content.find("<name>");
    if (name_start != string::npos) {
      int name_end = content.find("</name>");
      if (name_end != string::npos) {
	Track t;
	t.title = content.substr(name_start + 6, name_end - name_start - 6);
	content = content.substr(name_end+6);

	int artist_start = content.find("<name>");
	if (artist_start != string::npos) {
	  int artist_end = content.find("</name>");
	  if (artist_end != string::npos) {
	    t.artist = content.substr(artist_start + 6, artist_end - artist_start - 6);
	    content = content.substr(artist_end+6);
	    tracks.push_back(t);
	  }
	}
      }
    } else
      break;
  }

  return tracks;
}

std::vector<LastFM::Track> LastFM::lookup_loved_tracks(const std::string& lover)
{
  string content;
  WgetWrapper::download(domain + "/2.0/?method=user.getlovedtracks&user=" + lover + "&api_key=" + key, content);
  
  return extract_tracks(content);
}

std::vector<LastFM::Track> LastFM::lookup_top_tracks(const std::string& username)
{
  string content;
  WgetWrapper::download(domain + "/2.0/?method=user.gettoptracks&user=" + username + "&api_key=" + key, content);
  
  return extract_tracks(content);
}

