#include "libfspp.hpp"

#include "audio-library.hpp"

#include "renderer.hpp"
#include "audio_config.hpp"
#include "config.hpp"

#include "gsimplefile.hpp"

#include "cpp_sqlitepp.hpp"

#include "boost.hpp"

// --version
#include <getopt.h>

#include <fileref.h>
#include <tag.h>

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

#include <vector>
#include <stack>
#include <list>
#include <string>

using namespace TagLib;

using std::vector;
using std::list;
using std::string;

string get_cover_from_dir(const string& dir)
{
  Config *conf = S_Config::get_instance();

  string pic = "";

  vector<string> pictures_found;

  MyPair filetype;

  // FIXME: is this right? Do we still use .front-cover.jpg? Or just legacy?

  // check for special cover first
  if (file_exists(dir + "/.front-cover.jpg")) {
    pic = dir + "/.front-cover.jpg";
  } else {
    // determine if there is a picture in this dir...
    for (file_iterator<file_t, default_order> i (dir); i != i.end (); i.advance(false))
      {
       filetype = check_type(i->getName(), conf->p_filetypes_p());

       if (filetype != emptyMyPair)
         pictures_found.push_back(i->getName());
      }

    if (!pictures_found.empty()) {
      pic = pictures_found.front();
      if (pictures_found.size() > 1) {
	foreach (string& picture, pictures_found)
	  if (string_format::lowercase(picture).rfind("front") != string::npos) {
	    pic = picture;
	    break;
	  }
      }
    }
  }

  return pic;
}

string get_first_cover_in_dir(const string& dir)
{
  string cover = "";
  bool cover_found = false;

  string cur_dir = dir;

  list<string> dirs_to_try;

  while (!cover_found) {
    cover = get_cover_from_dir(cur_dir);

    if (!cover.empty()) {
      cover_found = true;
    }
    else {
      bool new_dir_found = false;

      for (file_iterator<file_t, default_order> i (cur_dir); i != i.end (); i.advance(false)) {
       if (isDirectory(i->getName())) {
         if (!new_dir_found) {
           new_dir_found = true;
           cur_dir = i->getName();
         } else {
           dirs_to_try.push_back(i->getName());
         }
       }
      }
      if (!new_dir_found) {
       if (!dirs_to_try.empty()) {
         cur_dir = dirs_to_try.front();
         dirs_to_try.pop_front();
       } else
         break;
      }
    }
  }

  return cover;
}

void insert_cover_into_db(const string& filename, const string& type, SQLDatabase& db)
{
  bool found_cover = false;

  SQLQuery *q2 = db.query("Covers", ("SELECT Cover FROM %t WHERE Path='" + string_format::escape_db_string(filename) + "'").c_str());
  if (q2 && q2->numberOfTuples() > 0)
    found_cover = true;
  delete q2;

  if (!found_cover) {

    string cover = "";

    if (type == "file") {
      GSimplefile s;
      s.path = filename;
      cover = s.find_cover_in_current_dir();
    } else if (type == "media-track") {
      havemydir("cddb_data", &cover);
      cover += filename;
      cover += ".jpg";
    } else if (type == "dir") {
      cover = get_first_cover_in_dir(filename);
    } else {
      assert(false);
    }

    char *tmp_insert_2 = sqlite3_mprintf("INSERT INTO Covers VALUES('%q', '%q')",
					 filename.c_str(), cover.c_str());
    db.execute(tmp_insert_2);
    sqlite3_free(tmp_insert_2);
  }
}

void insert_file_into_db(const string& filename, const string& parent, SQLDatabase& db)
{
  std::cout << "checking filename:" << filename << std::endl;

  bool is_dir = isDirectory(filename);

  string good_filename;

  if (is_dir)
    good_filename = string_format::unique_folder_name(filename);
  else
    good_filename = filename;

  // check if already exists
  SQLQuery *q = db.query("Folders", ("SELECT id FROM %t WHERE filename='" + string_format::escape_db_string(good_filename) + "'").c_str());
  if (q && q->numberOfTuples() > 0) {
    delete q;
    return;
  } else
    delete q;

  string parent_id = "";

  string good_parent = string_format::unique_folder_name(parent);

  q = db.query("Folders", ("SELECT id FROM %t WHERE filename='" + string_format::escape_db_string(good_parent) + "'").c_str());
  if (q && q->numberOfTuples() > 0) {
    assert(q->numberOfTuples() == 1);
    SQLRow &row = (*q)[0];
    parent_id = row["id"];
  }
  delete q;

  if (parent_id.empty()) {

    string par = good_parent;

    if (good_parent[good_parent.size()-1] == '/')
      par = good_parent.substr(0, good_parent.size()-1);

    string name = par.substr(par.rfind('/')+1);

    char *tmp = sqlite3_mprintf("INSERT INTO Folders VALUES(NULL, '0', '%q', '%q', '%q', '1')",
				good_parent.c_str(), name.c_str(), string_format::lowercase(name).c_str());
    db.execute(tmp);
    sqlite3_free(tmp);

    q = db.query("Folders", ("SELECT id FROM %t WHERE filename='" + string_format::escape_db_string(good_parent) + "'").c_str());
    if (q && q->numberOfTuples() > 0) {
      assert(q->numberOfTuples() == 1);
      SQLRow &row = (*q)[0];
      parent_id = row["id"];
    }
    delete q;
  }

  assert(!parent_id.empty());

  string::size_type i;
  if ((i = filename.rfind('/')) == string::npos)
    i = 0;
  string name = filename.substr((i != 0) ? i+1 : i);
  string path = filename.substr(0, i+1);

  if (!is_dir && (i = name.rfind('.')) != string::npos)
    name = name.substr(0, i);

  char *tmp_insert = sqlite3_mprintf("INSERT INTO Folders VALUES(NULL, '%q', '%q', '%q', '%q', '%q')",
				     parent_id.c_str(), good_filename.c_str(), name.c_str(),
				     string_format::lowercase(name).c_str(), conv::itos(is_dir).c_str());
  db.execute(tmp_insert);
  sqlite3_free(tmp_insert);

  // and cover

  if (is_dir)
    path = good_filename;

  insert_cover_into_db(path, is_dir ? "dir" : "file", db);
}

// stupid workaround
void Mms::clean_up()
{}

void create_playlist_db(SQLDatabase& db)
{
  vector<string> fields;
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("audio_id INTEGER");
  db.createTable("Playlist", fields);
}

void create_indexes(SQLDatabase& db)
{
  db.execute("CREATE INDEX idx_album_name ON Album (lname)");

  db.execute("CREATE INDEX idx_artist_name ON Artist (lname)");

  db.execute("CREATE INDEX idx_folders_name ON Folders (name)");
  db.execute("CREATE INDEX idx_folders_lname ON Folders (lname)");
  db.execute("CREATE INDEX idx_folders_parent ON Folders (parent)");
  db.execute("CREATE INDEX idx_folders_filename ON Folders (filename)");

  db.execute("CREATE INDEX idx_audio_filename ON Audio (filename)");
  db.execute("CREATE INDEX idx_audio_title ON Audio (lTitle)");
  db.execute("CREATE INDEX idx_audio_artist ON Audio (Artist)");
  db.execute("CREATE INDEX idx_audio_album ON Audio (Album)");
}

void create_db(SQLDatabase& db)
{
  vector<string> fields;
  fields.push_back("version INTEGER NOT NULL");
  db.createTable("Schema", fields);

  db.execute("INSERT INTO Schema VALUES('4')");

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("parent INTEGER");
  fields.push_back("filename TEXT");
  fields.push_back("name TEXT");
  fields.push_back("lname TEXT");
  fields.push_back("is_folder INTEGER"); // 1 = true
  db.createTable("Folders", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("name TEXT");
  fields.push_back("lname TEXT");
  db.createTable("Artist", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("name TEXT");
  fields.push_back("lname TEXT");
  db.createTable("Album", fields);

  // FIXME: use the next 3 rows for something

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("name INTEGER");
  db.createTable("Year", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("name TEXT");
  db.createTable("Genre", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("aid INTEGER NOT NULL");
  fields.push_back("gid INTEGER NOT NULL");
  db.createTable("GAudio", fields);

  fields.clear();
  fields.push_back("Path TEXT PRIMARY KEY");
  fields.push_back("Cover TEXT");
  db.createTable("Covers", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("Artist INTEGER");
  fields.push_back("Album INTEGER");
  fields.push_back("Title TEXT");
  fields.push_back("lTitle TEXT");
  fields.push_back("filename TEXT");
  fields.push_back("Bitrate FLOAT");
  fields.push_back("Length INTEGER");
  fields.push_back("Track INTEGER");
  db.createTable("Audio", fields);

  create_playlist_db(db);

  create_indexes(db);
}

void check_db(SQLDatabase& db)
{
  if (db.hasTable("Schema")) {
    // check for correct version
    SQLQuery *q = db.query("Schema", "SELECT version FROM %t");
    if (q && q->numberOfTuples() > 0) {
      SQLRow &row = (*q)[0];
      if (row["version"] != "4") {

	Print print(gettext("Upgrading audio database schema"), Print::INFO);

	SQLQuery *dq = db.query("sqlite_master", "SELECT name FROM %t WHERE type='table'");
	if (dq) {
	  for (int i = 0; i < dq->numberOfTuples(); ++i) {
	    db.execute(("DROP TABLE " + (*dq)[i]["name"]).c_str());
	  }
	}

	delete dq;

	dq = db.query("sqlite_master", "SELECT name FROM %t WHERE type='index'");
	if (dq) {
	  for (int i = 0; i < dq->numberOfTuples(); ++i) {
	    db.execute(("DROP INDEX " + (*dq)[i]["name"]).c_str());
	  }
	}

	delete dq;

	create_db(db);
      }
    }
    delete q;
  } else {
    // create the whole db
    create_db(db);
  }
}

static struct option longopts[] =
{
  { "conf", 1, 0, 'c' },
  { "personal", 1, 0, 'p' },
  { "help", 0, 0, 'h' },
  { "directories", 1, 0, 'd' },
  {0, 0, 0, 0}
};

void set_default(Config *conf)
{
  string homedir = "";

  havemydir("", &homedir);
  conf->s_var_data_dir(homedir);
}

void set_config(Config *conf, string config)
{
  if (config[config.size()-1] != '/')
    config += '/';

  conf->s_homedir(config);
}

void set_personal(Config *conf, string personal)
{
  if (personal[personal.size()-1] != '/')
    personal += '/';

  conf->s_var_data_dir(personal);
}

void check_path(SQLDatabase& db, std::list<std::string>&directories)
{
  Config *conf = S_Config::get_instance();
  AudioConfig *audio_conf = S_AudioConfig::get_instance();

  std::stack<string> queued_dirs;

  std::cout << "starting to extract metadata from audio files" << std::endl;

  list<string> audio_folders;
  list<string> audio_dirs;
  if (directories.size())
    audio_dirs = directories;
  else
    audio_dirs = audio_conf->p_audio_dirs(); // make compiler happy

  // make sure audio folders are not malformed
  foreach (string& dir, audio_dirs) {
    if (dir[dir.size()-1] != '/') {
      audio_folders.push_back(dir + '/');
    } else
      audio_folders.push_back(dir);
  }

  list<string> audio_dirs_no_watch;
  if (directories.empty())
    audio_dirs_no_watch = audio_conf->p_audio_dirs_no_watch(); // make compiler happy
  // make sure audio folders are not malformed
  foreach (string& dir, audio_dirs_no_watch) {
    if (dir[dir.size()-1] != '/') {
      audio_folders.push_back(dir + '/');
    } else
      audio_folders.push_back(dir);
  }

  foreach (string& dir, audio_folders)
    queued_dirs.push(dir);

  while (!queued_dirs.empty()) {
    string cur_dir = queued_dirs.top();
    queued_dirs.pop();
    for (file_iterator<file_t, default_order> i (cur_dir); i != i.end (); i.advance(false)) {

      string cur_file = string_format::remove_doubles(i->getName());

      if (isDirectory(cur_file)) {
	insert_file_into_db(cur_file, cur_dir, db);
	queued_dirs.push(cur_file);
      } else {
	if (check_type(cur_file, audio_conf->p_filetypes_a()) == emptyMyPair)
	  continue;

	insert_file_into_db(cur_file, cur_dir, db);

	SQLQuery *q = db.query("Audio", ("SELECT * FROM %t WHERE filename='" + string_format::escape_db_string(cur_file) + "'").c_str());

	if (!q || q->numberOfTuples() == 0) {

	  std::cout << "file:" << cur_file << " not found in db, inserting" << std::endl;

	  FileRef fileref(cur_file.c_str(), true, AudioProperties::Accurate);

	  string artist = "", album = "", title = "";
	  int year = 0, track = 0, length = 0, bitrate = 0;

	  if (!fileref.isNull() && fileref.tag()) {
	    artist = fileref.tag()->artist().toCString();
	    album = fileref.tag()->album().toCString();
	    title = fileref.tag()->title().toCString();
	    year = fileref.tag()->year();
	    track = fileref.tag()->track();
	  }

	  if (!fileref.isNull() && fileref.audioProperties()) {
	    length = fileref.audioProperties()->length();
	    bitrate = fileref.audioProperties()->bitrate();
	  }

	  int artist_id = 0;

	  SQLQuery *q2 = db.query("Artist", ("SELECT id FROM %t WHERE name='" + string_format::escape_db_string(artist) + "'").c_str());
	  if (q2 && q2->numberOfTuples() > 0) {
	    SQLRow &row = (*q2)[0];
	    artist_id = conv::atoi(row["id"]);
	  }
	  delete q2;

	  if (artist_id == 0) {
	    char *tmp = sqlite3_mprintf("INSERT INTO Artist VALUES(NULL, '%q', '%q')", artist.c_str(), string_format::lowercase(artist).c_str());
	    db.execute(tmp);
	    sqlite3_free(tmp);

	    q2 = db.query("Artist", ("SELECT id FROM %t WHERE name='" + string_format::escape_db_string(artist) + "'").c_str());
	    if (q2 && q2->numberOfTuples() > 0) {
	      SQLRow &row = (*q2)[0];
	      artist_id = conv::atoi(row["id"]);
	    }
	    delete q2;
	  }

	  int album_id = 0;

	  q2 = db.query("Album", ("SELECT id FROM %t WHERE name='" + string_format::escape_db_string(album) + "'").c_str());
	  if (q2 && q2->numberOfTuples() > 0) {
	    SQLRow &row = (*q2)[0];
	    album_id = conv::atoi(row["id"]);
	  }
	  delete q2;

	  if (album_id == 0) {
	    char *tmp = sqlite3_mprintf("INSERT INTO Album VALUES(NULL, '%q', '%q')", album.c_str(), string_format::lowercase(album).c_str());
	    db.execute(tmp);
	    sqlite3_free(tmp);

	    q2 = db.query("Album", ("SELECT id FROM %t WHERE name='" + string_format::escape_db_string(album) + "'").c_str());
	    if (q2 && q2->numberOfTuples() > 0) {
	      SQLRow &row = (*q2)[0];
	      album_id = conv::atoi(row["id"]);
	    }
	    delete q2;
	  }

	  char *tmp = sqlite3_mprintf("INSERT INTO Audio VALUES(NULL, '%q', '%q', '%q', '%q', '%q', '%q', '%q', '%q')",
				      conv::itos(artist_id).c_str(), conv::itos(album_id).c_str(),
				      title.c_str(), string_format::lowercase(title).c_str(), cur_file.c_str(),
				      conv::ftos(bitrate).c_str(), conv::itos(length).c_str(),
				      conv::itos(track).c_str());
	  db.execute(tmp);
	  sqlite3_free(tmp);
	} else
	  std::cout << "file:" << cur_file << " found in db" << std::endl;

	delete q;
      }
    }
  }
}

int main(int argc,char* argv[])
{
  int otherinstance = -1;
  for (int t = 0; t < 5; t++){
    otherinstance = filemutex::lock();
    if (!otherinstance){
      break;
    }
    usleep(500*1000);
  }

  if (otherinstance < 0){
    print_critical(gettext("An instance of MMS or another instance of movie/pic/audio library is already running\n"));
    exit(1);
  }

  else if(otherinstance > 0){
    fprintf(stderr, gettext("File lock is currently owned by another process (pid %d). Aborting\n"), otherinstance);
    exit(1);
  }

  Config *config = S_Config::get_instance();

  string homedir = "", var_data = "";

#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms", nl_langinfo(CODESET));
  textdomain("mms");
#endif

  set_config(config, mms_config); // config
  set_default(config); // personal

  int c;
  int min_argc = 1;
  std::list<std::string>directories;
  directories.clear();

  while ((c = getopt_long(argc, argv, "c:p:d:h",
			  longopts, NULL)) != -1)
    switch (c) {
    case 'c': // config
      min_argc += 2;
      homedir = optarg;
      set_config(config, homedir);
      break;
    case 'p': // personal
      min_argc += 2;
      var_data = optarg;
      set_personal(config, var_data);
      break;
    case 'd':
      min_argc += 2;// what is this for?
      directories.push_back(string(optarg));
      break;
    case 'h':
      Print print (Print::INFO);
      print.add_line(gettext("My Media System audio library creation tool"));
      print.add_line("");
      print.add_line(gettext("Usage: ./mms-audio-library"));
      print.add_line("");
      print.add_line(gettext("Options:"));
      print.add_line(gettext(" -c <absolute> : use alternative config directory"));
      print.add_line(gettext(" -p <absolute> : use alternative personal directory (db, playlist)"));
      print.add_line(gettext(" -d <path> : Override watch paths in configuration file with <path> (can be used multiple times)"));
      print.print();
      exit(0);
      break;
    }

  Print print(gettext("Using ") + config->p_homedir() + gettext(" as configuration directory"), Print::INFO);
  Print print2(gettext("Using ") + config->p_var_data_dir() + gettext(" as personal directory"), Print::INFO);

  if (homedir == ""){  //configuration files
    if (!(homedir = s_getenv("HOME")).empty() && dir_exists(homedir + "/.mms"))
      set_config(config, homedir + "/.mms");
    else
      set_config(config, mms_config); // hardcoded default config
  }

  // now pass the configuration file using the correct path
  config->parse_configuration_file();

  S_AudioConfig::get_instance()->parse_configuration_file(config->p_homedir());

  if (config->p_temp_image_loc() == "") {
    string cache_dir;
    recurse_mkdir(config->p_var_data_dir(), "cache/", &cache_dir);
    config->s_temp_image_loc(cache_dir);
  }

  SQLDatabase db((config->p_var_data_dir() + "audio.db").c_str());

  std::cout << "storing information in: " << (config->p_var_data_dir() + "audio.db") << std::endl;

  check_db(db);

  check_path(db, directories);
}
