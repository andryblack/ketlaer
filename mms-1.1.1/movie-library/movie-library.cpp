#include "libfspp.hpp"

#include "movie-library.hpp"

#include "renderer.hpp"
#include "movie_config.hpp"
#include "config.hpp"

#include "updater.hpp"
#ifndef use_ffmpeg_thumbnailer
#include "mplayer.hpp"
#else
#include "thumbnailer.h"
#endif
#include "gsimplefile.hpp"

#include "cpp_sqlitepp.hpp"

#include "plugins.hpp"

#include "boost.hpp"

// --version
#include <getopt.h>

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

#include <vector>
#include <stack>
#include <list>
#include <string>

using std::vector;
using std::list;
using std::string;

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
    char *tmp = sqlite3_mprintf("INSERT INTO Folders VALUES(NULL, '0', '%q', '', '', '0', '1')", good_parent.c_str());
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

  if (!is_dir && (i = name.rfind('.')) != string::npos)
    name = name.substr(0, i);

  char *tmp_insert = sqlite3_mprintf("INSERT INTO Folders VALUES(NULL, '%q', '%q', '%q', '%q', '0', '%q')",
				     parent_id.c_str(), good_filename.c_str(), name.c_str(),
				     string_format::lowercase(name).c_str(), conv::itos(is_dir).c_str());
  db.execute(tmp_insert);
  sqlite3_free(tmp_insert);
}


#ifndef use_ffmpeg_thumbnailer
bool create_thumbnail(const string& file, const string& thumbnail, int width, int height, Mplayer& thumbnailer)
{
  return thumbnailer.create_thumbnail(file, thumbnail, width, height);
}

#else
bool create_thumbnail(const string& file, const string& thumbnail, int width, int height, thumbnailer::thumbnailer& thumb){
  std::string _dir = thumbnail.substr(0, thumbnail.rfind('/')+1);
  abs_recurse_mkdir(_dir, NULL);
  thumbnailer::movie_specs ms;
  thumb.openmovie(file,&ms);
  bool ret = thumb.createthumbnail(ms.length*1000/8,width,height,true, thumbnail);
  thumb.closemovie();
  return ret;
}
#endif

void create_indexes(SQLDatabase& db)
{
  db.execute("CREATE INDEX idx_folders_name ON Folders (name)");
  db.execute("CREATE INDEX idx_folders_lname ON Folders (lname)");
  db.execute("CREATE INDEX idx_folders_parent ON Folders (parent)");
  db.execute("CREATE INDEX idx_folders_filename ON Folders (filename)");

  db.execute("CREATE INDEX idx_directors_name ON Directors (lname)");

  db.execute("CREATE INDEX idx_writingcredits_name ON WritingCredits (lname)");
  db.execute("CREATE INDEX idx_writingcredits_role ON WritingCredits (lrole)");

  db.execute("CREATE INDEX idx_genre_name ON Genre (lname)");

  db.execute("CREATE INDEX idx_actors_name ON Actors (lname)");
  db.execute("CREATE INDEX idx_actors_role ON Actors (lrole)");

  db.execute("CREATE INDEX idx_movie_title ON Movie (ltitle)");
  db.execute("CREATE INDEX idx_movie_tagline ON Movie (ltagline)");
  db.execute("CREATE INDEX idx_movie_plot ON Movie (lplot)");
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
  fields.push_back("is_thumbnailed INTEGER");
  fields.push_back("is_folder INTEGER"); // 1 = true
  db.createTable("Folders", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("name TEXT");
  fields.push_back("lname TEXT");
  db.createTable("Directors", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("did INTEGER NOT NULL");
  fields.push_back("mid INTEGER NOT NULL");
  db.createTable("DMovie", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("name TEXT");
  fields.push_back("lname TEXT");
  fields.push_back("role TEXT");
  fields.push_back("lrole TEXT");
  db.createTable("WritingCredits", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("wcid INTEGER NOT NULL");
  fields.push_back("mid INTEGER NOT NULL");
  db.createTable("WCMovie", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("name TEXT");
  fields.push_back("lname TEXT");
  db.createTable("Genre", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("gid INTEGER NOT NULL");
  fields.push_back("mid INTEGER NOT NULL");
  db.createTable("GMovie", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("name TEXT");
  fields.push_back("lname TEXT");
  fields.push_back("role TEXT");
  fields.push_back("lrole TEXT");
  db.createTable("Actors", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("aid INTEGER NOT NULL");
  fields.push_back("mid INTEGER NOT NULL");
  db.createTable("AMovie", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("title TEXT");
  fields.push_back("ltitle TEXT");
  fields.push_back("path TEXT");
  fields.push_back("url TEXT");
  fields.push_back("runtime TEXT");
  fields.push_back("tagline TEXT");
  fields.push_back("ltagline TEXT");
  fields.push_back("plot TEXT");
  fields.push_back("lplot TEXT");
  fields.push_back("rating REAL");
  fields.push_back("votes INTEGER");
  fields.push_back("year INTEGER");
  fields.push_back("top250 INTEGER");

  db.createTable("HDMovie", fields);

  create_indexes(db);
}

void check_db(SQLDatabase& db)
{
  if (db.hasTable("Schema")) {
    // check for correct version
    SQLQuery *q = db.query("Schema", "SELECT version FROM %t");
    if (q && q->numberOfTuples() > 0) {
      SQLRow &row = (*q)[0];
      if (row["version"] == "4")
	; // correct version
      else if (row["version"] == "3") {
	Print print(gettext("Upgrading movie database schema"), Print::INFO);
	db.execute("ALTER TABLE Folders ADD is_thumbnailed INTEGER");
	db.execute("UPDATE Schema SET version=4");
      }	else if (row["version"] != "3") {

	Print print(gettext("Upgrading movie database schema"), Print::INFO);

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

// stupid workaround
void Mms::clean_up()
{}

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
  MovieConfig *movie_conf = S_MovieConfig::get_instance();
#ifndef use_ffmpeg_thumbnailer
  Mplayer thumbnailer;
#else
  thumbnailer::thumbnailer  thumbnailer;
  thumbnailer.init((conf->p_debug_level() > 2));
  thumbnailer.deepval = 
	  (movie_conf->p_thumbnail_accur() >= 0 && movie_conf->p_thumbnail_accur() < 10) ? movie_conf->p_thumbnail_accur() : 3;
  thumbnailer.skipblanks = movie_conf->p_thumbnail_noblanks();
  if (thumbnailer.skipblanks > 0)
     print_warning("Internal thumbnailing: skip blank frames mode is active");
  if (thumbnailer.skipblanks > 2)
    print_warning(string_format::str_printf("Skip blank frames mode is set to %d,\n thumbnailing will be much slower\n",
	  thumbnailer.skipblanks));
#endif
  int info_width = conf->p_v_res() / 3 - 15, info_height = info_width/4*3;

  string normal_font = "Vera/" + conv::itos(resolution_dependant_font_size(13, conf->p_v_res()));
  std::pair<int, int> element_size = string_format::calculate_string_size("abcltuwHPMjJg", normal_font);
  int normal_font_height = element_size.second-5-(conf->p_v_res()/405);

  string header_font = "Vera/" + conv::itos(resolution_dependant_font_size(28, conf->p_v_res()));
  std::pair<int, int> header_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
  int header_box_size = static_cast<int>(header_size.second * 0.75);

  // resolution dependant stuff

  int search_box_height = 40;
  if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
    search_box_height = 60;

  int rows = 3;
  int rows_search = 2;

  if (conf->p_v_res() <= 600) {
    int search_box_height = 35;
    if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
      search_box_height = 50;

    if (conf->p_v_res() <= 500)
      rows = 2;
  }

  int top_height = 10 * 2 + header_size.second;

  string button_playback_font = "Vera/" + conv::itos(resolution_dependant_font_size(17, conf->p_v_res()));
  element_size = string_format::calculate_string_size("abcltuwHPMjJg", button_playback_font);
  int button_playback_height = element_size.second - 5;

  int bottom_height = static_cast<int>(2 * button_playback_height * 0.85);

  // resolution independant stuff

  AspectRatio ar = aspect_ratio(conf->p_h_res(), conf->p_v_res());

  int images_per_row = 5; // 4:3

  if (ar == SIXTEEN_NINE)
    images_per_row = 6;
  else if (ar == SIXTEEN_TEN)
    images_per_row = 6;

  int image_height_all = (conf->p_v_res() - top_height - bottom_height)/rows;
  int image_height = image_height_all - normal_font_height*2 + 3;

  image_height = static_cast<int>(image_height/1.35); // zoom

  int image_width_all = (conf->p_h_res()-2*25)/images_per_row;
  int image_width = image_width_all - 20;

  image_width = static_cast<int>(image_width/1.35); // zoom

  if (rows == rows_search) {
    image_height = image_height_all - normal_font_height*2;
    image_height = static_cast<int>(image_height/1.35); // zoom
  }

  std::stack<string> queued_dirs;

  std::cout << "starting to generate thumbnails for movie files" << std::endl;

  list<string> movie_folders;
  list<string> movie_dirs;
  if (directories.size()){
    movie_dirs = directories;
  }
  else
    movie_dirs = movie_conf->p_movie_dirs(); // make compiler happy

  // make sure movie folders are not malformed
  foreach (string& dir, movie_dirs) {
    if (dir[dir.size()-1] != '/') {
      movie_folders.push_back(dir + '/');
    } else
      movie_folders.push_back(dir);
  }
  list<string> movie_dirs_no_watch;
  if (directories.empty())
    movie_dirs_no_watch = movie_conf->p_movie_dirs_no_watch(); // make compiler happy
  // make sure movie folders are not malformed
  foreach (string& dir, movie_dirs_no_watch) {
    if (dir[dir.size()-1] != '/') {
      movie_folders.push_back(dir + '/');
    } else
      movie_folders.push_back(dir);
  }

  foreach (string& dir, movie_folders)
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
	if (check_type(cur_file, movie_conf->p_filetypes_m()) == emptyMyPair)
	  continue;

	insert_file_into_db(cur_file, cur_dir, db);

	string thumbnail = "";

	string::size_type dot_pos = cur_file.rfind(".");
	if (dot_pos != string::npos) {

	  int full_width = static_cast<int>(image_width * 1.35);
	  int full_height = static_cast<int>(image_height * 1.35);

	  // list view
	  thumbnail = S_Render::get_instance()->image_get_fullpath(cur_file, full_width, full_height);

	  // try thumbnail
	  if (!file_exists(thumbnail))
	    create_thumbnail(cur_file, thumbnail, full_width, full_height, thumbnailer);

	  // list view
	  thumbnail = conf->p_temp_image_loc() + cur_file.substr(0, dot_pos) + "_" + conv::itos(info_width) + "_" + conv::itos(info_height) + "_thumb.jpg";

          if (!file_exists(thumbnail))
	    create_thumbnail(cur_file, thumbnail, info_width, info_height, thumbnailer);
	}
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
      print.add_line(gettext("My Media System movie library creation tool"));
      print.add_line("");
      print.add_line(gettext("Usage: ./mms-movie-library"));
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

  S_MovieConfig::get_instance()->parse_configuration_file(config->p_homedir());

  if (config->p_temp_image_loc() == "") {
    string cache_dir;
    recurse_mkdir(config->p_var_data_dir(), "cache/", &cache_dir);
    config->s_temp_image_loc(cache_dir);
  }

  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
  //screen_updater->start();
  screen_updater->activate();

  BackgroundUpdater *background_updater = S_BackgroundUpdater::get_instance();

  //background_updater->start();
  background_updater->activate();

  SQLDatabase db((config->p_var_data_dir() + "moviehd.db").c_str());

  std::cout << "storing information in: " << (config->p_var_data_dir() + "moviehd.db") << std::endl;

  check_db(db);

  check_path(db, directories);
}
