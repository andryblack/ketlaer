#include "libfspp.hpp"

#include "pic-library.hpp"

#include "renderer.hpp"
#include "picture_config.hpp"
#include "config.hpp"

#include "cpp_sqlitepp.hpp"

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

    string name = good_parent;
    if (name[name.size()-1] == '/')
      name = name.substr(0, name.size()-1);

    char *tmp = sqlite3_mprintf("INSERT INTO Folders VALUES(NULL, '0', '%q', '%q', '%q', '1')", good_parent.c_str(), name.substr(name.rfind('/') + 1).c_str() , name.substr(name.rfind('/') + 1).c_str() );

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

  char *tmp_insert = sqlite3_mprintf("INSERT INTO Folders VALUES(NULL, '%q', '%q', '%q', '%q', '%q')",
				     parent_id.c_str(), good_filename.c_str(), name.c_str(),
				     string_format::lowercase(name).c_str(), conv::itos(is_dir).c_str());
  db.execute(tmp_insert);
  sqlite3_free(tmp_insert);
}

// stupid workaround
void Mms::clean_up()
{}

void create_indexes(SQLDatabase& db)
{
  db.execute("CREATE INDEX idx_folders_name ON Folders (name)");
  db.execute("CREATE INDEX idx_folders_lname ON Folders (lname)");
  db.execute("CREATE INDEX idx_folders_parent ON Folders (parent)");
  db.execute("CREATE INDEX idx_folders_filename ON Folders (filename)");

  db.execute("CREATE INDEX idx_pictures_filename ON Pictures (filename)");
}

void create_db(SQLDatabase& db)
{
  vector<string> fields;
  fields.push_back("version INTEGER NOT NULL");
  db.createTable("Schema", fields);

  db.execute("INSERT INTO Schema VALUES('3')");

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
  db.createTable("Group", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("pid INTEGER NOT NULL");
  fields.push_back("gid INTEGER NOT NULL");
  db.createTable("GPicture", fields);

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("filename TEXT");
  fields.push_back("title TEXT");
  fields.push_back("comment TEXT");
  fields.push_back("date INTEGER");
  fields.push_back("gid INTEGERL");
  fields.push_back("rating INTEGER");
  fields.push_back("rotation INTEGER");
  db.createTable("Pictures", fields);

  create_indexes(db);
}

void check_db(SQLDatabase& db)
{
  if (db.hasTable("Schema")) {
    // check for correct version
    SQLQuery *q = db.query("Schema", "SELECT version FROM %t");
    if (q && q->numberOfTuples() > 0) {
      SQLRow &row = (*q)[0];
      if (row["version"] != "3") {

	Print print(gettext("Upgrading pictures database schema"), Print::INFO);

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

static bool thumbnail_fullscreen = false;

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
  PictureConfig *pic_conf = S_PictureConfig::get_instance();
  Render *render = S_Render::get_instance();

  std::stack<string> queued_dirs;

  std::cout << "starting to thumbnail" << std::endl;

  string header_font = "Vera/" + conv::itos(resolution_dependant_font_size(28, conf->p_v_res()));

  std::pair<int, int> header_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
  int header_box_size = static_cast<int>(header_size.second * 0.75);

  string normal_font = "Vera/" + conv::itos(resolution_dependant_font_size(13, conf->p_v_res()));

  std::pair<int, int> element_size = string_format::calculate_string_size("abcltuwHPMjJg", normal_font);
  int normal_font_height = element_size.second - 5 - (conf->p_v_res()/405);

  int top_height = 10 * 2 + header_size.second;

  string button_playback_font = "Vera/" + conv::itos(resolution_dependant_font_size(17, conf->p_v_res()));
  element_size = string_format::calculate_string_size("abcltuwHPMjJg", button_playback_font);
  int button_playback_height = element_size.second - 5;

  int bottom_height = static_cast<int>(2 * button_playback_height * 0.85);

  int rows = 3;
  int rows_search = 2;

  if (conf->p_v_res() <= 600) {
    int search_box_height = 35;
    if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
      search_box_height = 50;

    if (conf->p_v_res() <= 500)
      rows = 2;
  }

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

  list<string> pic_folders;
  list<string> pic_dirs;
  if (directories.size())
    pic_dirs = directories;
  else
    pic_dirs = pic_conf->p_picture_dirs(); // make compiler happy

  // make sure pic folders are not malformed
  foreach (string& dir, pic_dirs) {
    if (dir[dir.size()-1] != '/') {
      pic_folders.push_back(dir + '/');
    } else
      pic_folders.push_back(dir);
  }

  list<string> pic_dirs_no_watch;
  if (directories.empty())
   pic_dirs_no_watch = pic_conf->p_picture_dirs_no_watch(); // make compiler happy
  // make sure pic folders are not malformed
  foreach (string& dir, pic_dirs_no_watch) {
    if (dir[dir.size()-1] != '/') {
      pic_folders.push_back(dir + '/');
    } else
      pic_folders.push_back(dir);
  }

  foreach (string& dir, pic_folders)
    queued_dirs.push(dir);

  while (!queued_dirs.empty()) {
    string cur_dir = queued_dirs.top();
    queued_dirs.pop();

    for (file_iterator<file_t, default_order> i (cur_dir); i != i.end (); i.advance(false)) {

      string cur_file = string_format::remove_doubles(i->getName());

      // exclude folders ?
      list<string> excluded = pic_conf->p_excluded_picture_dirs();
      if (excluded.size() > 0) {
	bool stop = false;

	foreach (string& excluded_dir, excluded)
	  if (cur_file.substr(cur_file.rfind("/")+1) == excluded_dir) {
	    stop = true;
	    break;
	  }

	if (stop)
	  continue;
      }

      if (isDirectory(cur_file)) {
	insert_file_into_db(cur_file, cur_dir, db);
	queued_dirs.push(cur_file);
      } else {

	if (check_type(cur_file, conf->p_filetypes_p()) == emptyMyPair)
	  continue;

	insert_file_into_db(cur_file, cur_dir, db);

	int full_width = static_cast<int>(image_width*1.35);
	int full_height = static_cast<int>(image_height*1.35);

	if (!file_exists(render->image_get_fullpath(cur_file, full_width, full_height))) {
	  std::cout << "creating marked thumbnail for image: " << cur_file  << std::endl;
	  render->create_scaled_image_wrapper(cur_file, full_width, full_height);
	}

	if (thumbnail_fullscreen && 
	    !file_exists(render->image_get_fullpath(cur_file, conf->p_h_res(), conf->p_v_res()))) {
	  std::cout << "creating fullscreen thumbnail for image: " << cur_file  << std::endl;
	  render->create_scaled_image_wrapper(cur_file, conf->p_h_res(), conf->p_v_res());
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

  bool fullscreen_thumbnails = false;

  while ((c = getopt_long(argc, argv, "c:p:d:h:f",
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
    case 'f':
      thumbnail_fullscreen = true;
      break;
    case 'h':
      Print print (Print::INFO);
      print.add_line(gettext("My Media System picture library creation tool"));
      print.add_line("");
      print.add_line(gettext("Usage: ./mms-pic-library"));
      print.add_line("");
      print.add_line(gettext("Options:"));
      print.add_line(gettext(" -c <absolute> : use alternate config directory"));
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

  S_PictureConfig::get_instance()->parse_configuration_file(config->p_homedir());

  if (config->p_temp_image_loc() == "") {
    string cache_dir;
    recurse_mkdir(config->p_var_data_dir(), "cache/", &cache_dir);
    config->s_temp_image_loc(cache_dir);
  }

  SQLDatabase db((config->p_var_data_dir() + "pictures.db").c_str());

  std::cout << "storing information in: " << (config->p_var_data_dir() + "pictures.db") << std::endl;

  check_db(db);

  check_path(db, directories);
}

