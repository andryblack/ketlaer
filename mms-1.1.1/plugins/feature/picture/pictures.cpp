// used for #ifdef
#include "config.h"

#include "libfspp.hpp"

#include "pictures.hpp"

//Input
#include "input.hpp"

#include "config.hpp"
#include "common.hpp"
#include "graphics.hpp"
#include "updater.hpp"
#include "common-feature.hpp"
#include "gettext.hpp"
#include "rand.hpp"

#include "search.hpp"

#include "startmenu.hpp"
#include "busy_indicator.hpp"
#include "shutdown.hpp"
#include "extra_menu.hpp"
#include "touch.hpp"
#include "SSaverobj.h"
#include "resolution.hpp"

// ostringstream
#include <sstream>

#include <iostream>

#include <unistd.h>

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

using std::list;
using std::vector;
using std::string;

Pictures::Pictures()
  : db((conf->p_var_data_dir() + "pictures.db").c_str()),
    exit_loop(false), in_fullscreen(false), orientation(0),
    zoom_level(1), zoom_position(1), reload_dirs(false),
    thumbnailed_all_files(false), pause(false), navigating_media(false),
    exit_startmenu(false)
{
  pic_conf = S_PictureConfig::get_instance();
  ext_mask = file_tools::create_ext_mask(conf->p_filetypes_p());
  visible = false;

  set_folders();

  check_db_consistency();

  reset();

  res_dependant_calc();

  S_ResolutionManagement::get_instance()->register_callback(boost::bind(&Pictures::res_dependant_calc, this));
}

void Pictures::res_dependant_calc()
{
  // resolution dependant stuff

  int search_box_height = 40;
  if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
    search_box_height = 60;

  rows = 3;
  rows_search = 2;

  if (conf->p_v_res() <= 600) {
    int search_box_height = 35;
    if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
      search_box_height = 50;

    if (conf->p_v_res() <= 500)
      rows = 2;
  }

  // resolution independant stuff

  header_font = graphics::resolution_dependant_font_wrapper(28, conf);
  search_font = graphics::resolution_dependant_font_wrapper(22, conf);
  search_select_font = graphics::resolution_dependant_font_wrapper(17, conf);
  normal_font = graphics::resolution_dependant_font_wrapper(13, conf);
  position_font = graphics::resolution_dependant_font_wrapper(18, conf);

  fullscreen_font = graphics::resolution_dependant_font_wrapper(16, conf);

  std::pair<int, int> header_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
  header_box_size = static_cast<int>(header_size.second * 0.75);

  normal_font_height = graphics::calculate_font_height(normal_font, conf);

  int top_height = 10 * 2 + header_size.second;

  string button_playback_font = "Vera/" + conv::itos(resolution_dependant_font_size(17, conf->p_v_res()));
  std::pair<int, int> element_size = string_format::calculate_string_size("abcltuwHPMjJg", button_playback_font);
  int button_playback_height = element_size.second - 5;

  int bottom_height = static_cast<int>(2 * button_playback_height * 0.85);

  AspectRatio ar = aspect_ratio(conf->p_h_res(), conf->p_v_res());

  images_per_row = 5; // 4:3

  if (ar == SIXTEEN_NINE)
    images_per_row = 6;
  else if (ar == SIXTEEN_TEN)
    images_per_row = 6;

  image_height_all = (conf->p_v_res() - top_height - bottom_height)/rows;
  image_height = image_height_all - normal_font_height*2 + 3;

  image_height = static_cast<int>(image_height/1.35); // zoom
  image_height_all_eks_text = image_height + 6;

  image_width_all = (conf->p_h_res()-2*25)/images_per_row;
  image_width = image_width_all - 20;

  image_width = static_cast<int>(image_width/1.35); // zoom

  if (rows != rows_search)
    image_height_all_search = image_height_all;
  else {
    image_height_all_search = (conf->p_v_res() - top_height - bottom_height - search_box_height)/rows_search;

    image_height = image_height_all - normal_font_height*2;

    image_height = static_cast<int>(image_height/1.35); // zoom
    image_height_all_eks_text = image_height + 6;
  }

  y_start = top_height;

  check_db();
}

Pictures::~Pictures()
{
  db_mutex.enterMutex();
  db.close();
  db_mutex.leaveMutex();
}

void Pictures::reset()
{
  for (int i = 0, size = folders.size(); i < size; ++i)
    folders.pop();

  folders.push(std::make_pair(picture_folders, 0));
}

void Pictures::set_folders()
{
  list<string> picture_dirs = pic_conf->p_picture_dirs(); // make compiler happy
  // make sure picture folders are not malformed
  foreach (string& dir, picture_dirs)
    if (dir[dir.size()-1] != '/')
      picture_folders.push_back(dir + '/');
    else
      picture_folders.push_back(dir);

#ifdef use_inotify
  S_Notify::get_instance()->register_plugin("pictures", picture_folders,
					    boost::bind(&Pictures::fs_change, this, _1, _2));
#endif

  list<string> picture_dirs_no_watch = pic_conf->p_picture_dirs_no_watch(); // make compiler happy
  // make sure picture folders are malformed
  foreach (string& dir, picture_dirs_no_watch)
    if (dir[dir.size()-1] != '/')
      picture_folders.push_back(dir + '/');
    else
      picture_folders.push_back(dir);
}

void Pictures::check_db_consistency()
{
  db_mutex.enterMutex();

  if (!db.hasTable("Folders")) {
    db_mutex.leaveMutex();
    return;
  }

  //Need to call rdir for each folder listed in picture_folders, so that the "parent" db row is updated. Probably there's a better way of doing it
  foreach (const string& dir, picture_folders ) {
    vector<Picture> tempfiles = rdir(dir);
  }

  SQLQuery *q = db.query("Folders", "SELECT id, parent, filename FROM %t WHERE parent==0");

  if (q->numberOfTuples() >  picture_folders.size()) { //if there's more "root" dirs in DB than in config file, db should be updated
    for (int i = 0; i < q->numberOfTuples(); ++i) {
      string filename = (*q)[i]["filename"].c_str();
      bool found = false;
      foreach (string& dir, picture_folders) {
	if ( dir == filename )
	  found=true;
      }
      if (!found) {
	foreach (string& dir, picture_folders)
	  if ( filename.find(dir, 0) != string::npos ) {
	    SQLQuery *q = db.query("Folders", ( "SELECT id FROM %t WHERE filename='"+dir + "'").c_str());
	    string parent_id =  (*q)[0]["id"].c_str();
	    db.execute(("UPDATE Folders SET parent = '" + parent_id + "' WHERE filename = '" + filename + "'").c_str());
	  }
      }
    }
  }
  db_mutex.leaveMutex();
}

void Pictures::startup_updater()
{
  BackgroundUpdater *background_updater = S_BackgroundUpdater::get_instance();
  background_updater->timer.add(TimeElement("extra_thumbnails", boost::bind(&Pictures::check_thumbnail, this),
					    boost::bind(&Pictures::background_thumbnails, this)));

  S_Search::get_instance()->register_module(SearchModule(dgettext("mms-pictures", "Pictures"), boost::bind(&Pictures::gen_search_list, this, _1),
							 boost::bind(&Pictures::reenter, this, _1)));
}

Pictures::PicOpts::PicOpts()
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-pictures", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-pictures", nl_langinfo(CODESET));
#endif

  // header
  header = "Picture Options";
  translated_header = dgettext("mms-pictures", "Picture Options");
  save_name = "PictureOptions";

  // values

  PictureConfig *pic_conf = S_PictureConfig::get_instance();

#ifdef use_inotify
  if (pic_conf->p_picture_dirs_no_watch().size() > 0) {
#else
  if (pic_conf->p_picture_dirs_no_watch().size() > 0 || pic_conf->p_picture_dirs().size() > 0) {
#endif
    // reload
    vector<string> none;
    reload_p = new Option(false, dgettext("mms-pictures", "reload"), "reload", 0, none, none);
    val.push_back(reload_p);
  }

  // sorting order
  dir_order_p = go->dir_order();
  val.push_back(dir_order_p);

  if (conf->p_cdpath().size() > 1)
    val.push_back(go->cd_device);

  vector<string> slideshow_values;

  for (int i = 0; i < 120; ++i)
    slideshow_values.push_back(conv::itos(i));

  slideshow_p = new Option(false, dgettext("mms-pictures", "slideshow delay"), "slideshow delay", 10,
			   slideshow_values, slideshow_values);
  val.push_back(slideshow_p);

  vector<string> zoom_values;

  zoom_values.push_back(dgettext("mms-pictures", "no"));
  zoom_values.push_back(dgettext("mms-pictures", "yes"));

  vector<string> english_zoom_values;

  english_zoom_values.push_back("no");
  english_zoom_values.push_back("yes");

  zoom_p = new Option(false, dgettext("mms-pictures", "zoom to fullscreen"), "zoom to fullscreen", 0,
		      zoom_values, english_zoom_values);
  val.push_back(zoom_p);

  vector<string> preview_values;

  preview_values.push_back(dgettext("mms-pictures", "no"));
  preview_values.push_back(dgettext("mms-pictures", "yes"));

  vector<string> english_preview_values;

  english_preview_values.push_back("no");
  english_preview_values.push_back("yes");

  preview_p = new Option(false, dgettext("mms-pictures", "show previews"), "show previews", 1,
			 preview_values, english_preview_values);
  val.push_back(preview_p);

  vector<string> info_values;
  info_values.push_back(dgettext("mms-pictures", "None"));
  info_values.push_back(dgettext("mms-pictures","Audio"));
  info_values.push_back(dgettext("mms-pictures","Title"));
  info_values.push_back(dgettext("mms-pictures","Full"));
  vector<string> info_english_values;
  info_english_values.push_back("None");
  info_english_values.push_back("Audio");
  info_english_values.push_back("Title");
  info_english_values.push_back("Full");
  info_p = new Option(false, dgettext("mms-pictures", "show info"), "show info", 3, info_values, info_english_values );
  val.push_back(info_p);

  vector<string> recurse_values;

  recurse_values.push_back(dgettext("mms-pictures", "no"));
  recurse_values.push_back(dgettext("mms-pictures", "yes"));

  vector<string> english_recurse_values;

  english_recurse_values.push_back("no");
  english_recurse_values.push_back("yes");

  recurse_p = new Option(false, dgettext("mms-pictures", "recurse dirs in fullscreen"), "recurse dirs in fullscreen", 0,
			 recurse_values, english_recurse_values);
  val.push_back(recurse_p);

  if (conf->p_idle_time() != 0) {
    vector<string> thumbnail_extract_values;

    thumbnail_extract_values.push_back(dgettext("mms-pictures", "no"));
    thumbnail_extract_values.push_back(dgettext("mms-pictures", "yes"));

    vector<string> english_thumbnail_extract_values;

    english_thumbnail_extract_values.push_back("no");
    english_thumbnail_extract_values.push_back("yes");

    thumbnail_extract_p = new Option(false, dgettext("mms-pictures", "bg thumbnail generation"), "bg thumbnail generation", 0,
				     thumbnail_extract_values, english_thumbnail_extract_values);
    val.push_back(thumbnail_extract_p);
  }

  vector<string> random_values;

  random_values.push_back(dgettext("mms-pictures", "no"));
  random_values.push_back(dgettext("mms-pictures", "yes"));

  vector<string> english_random_values;

  english_random_values.push_back("no");
  english_random_values.push_back("yes");

  random_p = new Option(false, dgettext("mms-pictures", "random order slideshow"), "random order slideshow", 0,
			 random_values, english_random_values);
  val.push_back(random_p);
}

Pictures::PicOpts::~PicOpts()
{
  save();

  delete dir_order_p;
  delete slideshow_p;
  delete zoom_p;
  delete preview_p;
  delete info_p;
  delete recurse_p;
  delete random_p;
  if (conf->p_idle_time() != 0) {
    delete thumbnail_extract_p;
  }
}

void Pictures::create_db()
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

  create_indexes();
}

void Pictures::create_indexes()
{
  db.execute("CREATE INDEX idx_folders_name ON Folders (name)");
  db.execute("CREATE INDEX idx_folders_lname ON Folders (lname)");
  db.execute("CREATE INDEX idx_folders_parent ON Folders (parent)");
  db.execute("CREATE INDEX idx_folders_filename ON Folders (filename)");

  db.execute("CREATE INDEX idx_pictures_filename ON Pictures (filename)");
}

void Pictures::check_db()
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

	create_db();
      }
    }
    delete q;
  } else {
    // create the whole db
    create_db();
  }
}

int Pictures::get_db_orientation(int db_id)
{
  int db_orientation = 0;

  db_mutex.enterMutex();

  SQLQuery *q = db.query("Folders", ("SELECT filename from %t WHERE id='" + conv::itos(db_id) + "'").c_str());

  if (q && q->numberOfTuples() > 0) {
    SQLQuery *q2 = db.query("Pictures", ("SELECT * FROM %t WHERE filename = '" + (*q)[0]["filename"] + "'").c_str());
    if (q2 && q2->numberOfTuples() > 0)
      db_orientation = conv::atoi((*q2)[0]["rotation"]);
    delete q2;
  }

  delete q;

  db_mutex.leaveMutex();

  return db_orientation;
}

void Pictures::set_db_orientation(int db_id)
{
  db_mutex.enterMutex();

  SQLQuery *q = db.query("Folders", ("SELECT filename from %t WHERE id='" + conv::itos(db_id) + "'").c_str());
  if (q && q->numberOfTuples() > 0) {
    db.execute(("UPDATE Pictures SET rotation = '" + conv::itos(orientation) + "' WHERE filename = '" + (*q)[0]["filename"] + "'").c_str());
  }
  delete q;

  db_mutex.leaveMutex();
}

void Pictures::find_element_and_do_action(const Picture& picture)
{
  int p = 0;
  foreach (Picture& f, pic_list) {
    if (f == picture) {
      folders.top().second = p;
      if (search_mode)
	input_master->got_touch_action();
      else
	action();
      break;
    } else
      ++p;
  }
}

int Pictures::find_position_in_folder_list(const list<string>& files, const string& filename)
{
  int pos = 0;
  bool pos_found = false;
  foreach (const string& file, files) {
    vector<Picture> templist = rdir(file);
    std::list<Picture> t(templist.begin(), templist.end());
    t.sort(file_sort());
    templist = vector<Picture>(t.begin(), t.end());
    pos = 0;
    foreach (Picture& picture, templist) {
      if (filename == picture.path) {
	pos_found = true;
	break;
      } else
	++pos;
    }
    if (pos_found)
      break;
  }

  if (!pos_found)
    return 0; // buggy
  else
    return pos;
}

bool Pictures::change_dir_to_id(int db_id)
{
  reset();

  db_mutex.enterMutex();

  SQLQuery *q = db.query("Folders", ("SELECT * FROM %t WHERE id='" + conv::itos(db_id) + "'").c_str());
  if (q && q->numberOfTuples() > 0) {

    string filename = (*q)[0]["filename"];
    if (filename[filename.size() - 1] == '/')
      filename = filename.substr(0, filename.size()-1);
    string original_filename = filename;

    delete q;
    db_mutex.leaveMutex();

    string cur_dir_path;

    int pos;
    foreach (string& dir, picture_folders)
      if ((pos = filename.rfind(dir)) != string::npos) {
	cur_dir_path = filename.substr(0, dir.size()+1);
	filename = filename.substr(dir.size()+1);
      }

    if (!filename.empty()) {
      while ((pos = filename.find("/")) != string::npos) {
	folders.top().second = find_position_in_folder_list(folders.top().first, cur_dir_path + filename.substr(0, pos));

	string dir = cur_dir_path + filename.substr(0, pos+1);

	list<string> templs; templs.push_back(dir);
	assert(dir[dir.size()-1] == '/');
	folders.push(std::make_pair(templs, 0));
	cur_dir_path += filename.substr(0, pos+1);
	filename = filename.substr(pos+1);
      }
    }

    // we should now be in the correct folder, find the correct file
    folders.top().second = find_position_in_folder_list(folders.top().first, original_filename);

    load_current_dirs();

    return true;
  } else {
    delete q;
    db_mutex.leaveMutex();
    return false;
  }
}

void Pictures::reenter(int db_id)
{
  if (change_dir_to_id(db_id))
    mainloop();
}

vector<std::pair<string, int> > Pictures::gen_search_list(const string& search_word)
{
  string parent = "";

  if (input_master->current_saved_map() == "pictures" && search_depth == dgettext("mms-pictures", "current folder")) {
    string parent_sql_str = "SELECT id FROM %t WHERE";

    list<string> current_folders = folders.top().first;
    bool first = true;
    foreach (string& folder, current_folders)
      if (first) {
	parent_sql_str += " filename='" + folder + "'";
	first = false;
      } else
	parent_sql_str += " OR filename='" + folder + "'";

    string parent_ids = "";

    db_mutex.enterMutex();

    SQLQuery *q0 = db.query("Folders", parent_sql_str.c_str());

    if (q0) {
      for (int i = 0; i < q0->numberOfTuples(); ++i) {
	SQLRow &row = (*q0)[i];
	if (i > 0)
	  parent_ids += ", " + row["id"];
	else
	  parent_ids += row["id"];
      }
    }
    delete q0;

    db_mutex.leaveMutex();

    parent = "parent in (" + parent_ids + ") AND";
  }

  std::vector<std::pair<string, int> > result;

  db_mutex.enterMutex();

  SQLQuery *q = db.query("Folders", ("SELECT id, parent, filename, name, is_folder FROM %t WHERE " + parent + " fuzzycmp('" + search_word + "', lname, 1)").c_str());

  if (q) {
    for (int i = 0; i < q->numberOfTuples(); ++i) {

      SQLRow &row = (*q)[i];

      bool result_ok = false;

      if (input_master->current_saved_map() == "pictures" && search_depth == dgettext("mms-pictures", "current folder")) {
	foreach (string& folder, folders.top().first)
	  if (row["filename"].find(folder) != string::npos) {
	    result_ok = true;
	    break;
	  }
      } else
	result_ok = true;

      if (result_ok && row["parent"] != "0") {
	string out = row["name"];
	if (row["is_folder"] == "1")
	  out += "/";
	result.push_back(std::make_pair(string_format::convert(out), conv::atoi(row["id"])));
      }
    }
  }
  delete q;

  db_mutex.leaveMutex();

  return result;
}

bool Pictures::file_sort::operator()(const Simplefile& lhs, const Simplefile& rhs)
{
  return sort_order(lhs.lowercase_name, rhs.lowercase_name,
		    lhs.path, rhs.path, lhs.type == "dir", rhs.type == "dir",
		    get_class<Pictures>(dgettext("mms-pictures", "Pictures"))->get_opts()->dir_order());
}

void Pictures::action_no_find_recursion()
{
  if (vector_lookup(pic_list, folders.top().second).type != "dir")
    pictures_fullscreen();
  else
    enter_dir();
}

// commands
void Pictures::action()
{
  if (vector_lookup(pic_list, folders.top().second).type != "dir" &&
      conv::stob(opts.recurse_in_fullscreen())) {

    recurse_files = parse_dirs_recursion();

    if (find_recursion_pos())
      pictures_fullscreen();

  } else
    action_no_find_recursion();
}

bool Pictures::enter_dir()
{
  if(vector_lookup(pic_list, folders.top().second).type != "dir") {
    input_master->set_map("pictures_fullscreen");
    pictures_fullscreen();
    input_master->set_map("pictures");
  } else {

    // FIXME: remove

    // go into the directory.
    string temp_cur_dir = vector_lookup(pic_list, folders.top().second).path;

    // get the total number of files:
    int nr_files = 0;
    for (file_iterator<file_t, default_order> i (temp_cur_dir); i != i.end (); i.advance(false))
      if (!isDirectory(i->getName()))
	++nr_files;

    ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

    // disable screensaver if it's running
    bool audio_fullscreen_running = true;

    if (!screen_updater->timer.status("audio_fullscreen"))
      audio_fullscreen_running = false;
    else
      screen_updater->timer.deactivate("audio_fullscreen");

    screen_updater->status_progressbar = 0;
    screen_updater->total_progressbar = nr_files;
    screen_updater->header = dgettext("mms-pictures", "Generating thumbnails");

    screen_updater->timer.add(TimeElement("progressbar", &check_progressbar, &print_progressbar));

    if (conf->p_priority_change())
      nice(19); //Lower priority to keep audio from skipping

    // process picture dir
    vector<Picture> templist = rdir(temp_cur_dir);

    if (conf->p_priority_change())
      nice(-19);

    screen_updater->timer.del("progressbar");
    cleanup_progressbar();

    // reenable screensaver
    if (audio_fullscreen_running)
      screen_updater->timer.activate("audio_fullscreen");

    if (templist.size() != 0) {
      list<string> templs;
      if (temp_cur_dir[temp_cur_dir.size()-1] != '/')
	templs.push_back(temp_cur_dir + '/');
      else
	templs.push_back(temp_cur_dir);
      folders.push(std::make_pair(templs, 0));

      std::list<Picture> t(templist.begin(), templist.end());
      t.sort(file_sort());
      pic_list = vector<Picture>(t.begin(), t.end());

      return true;
    } else {
      DialogWaitPrint pdialog(dgettext("mms-pictures", "Folder is empty"), 1000);
      return false;
    }

    S_BackgroundUpdater::get_instance()->run_once(boost::bind(&Pictures::check_for_changes, this));
  }
  return true;
}

void Pictures::go_back()
{
  if (search_mode)
    return;

  if (folders.size() > 1) {
    folders.pop();

    load_current_dirs();

  } else
    exit();
}

void Pictures::load_current_dirs()
{
  pic_list = parse_dir(folders.top().first);

  if (folders.top().first.size() >= 1 && folders.size() == 1) {
    std::list<Picture> t(pic_list.begin(), pic_list.end());
    t.sort(file_sort());
    pic_list = vector<Picture>(t.begin(), t.end());
  }
}

bool Pictures::find_recursion_pos()
{
  bool found_match = false;
  list<string> dirs = folders.top().first;
  pos_recursion = 0;

  // find the first image compared to the current position
  string cur_file = vector_lookup(pic_list, folders.top().second).path;
  foreach (Picture& picture, recurse_files)
    if (picture.path.substr(0, cur_file.size()) == cur_file) {
      found_match = true;
      break;
    } else
      ++pos_recursion;

  return found_match;
}

void Pictures::recurse_dirs()
{
  in_fullscreen = true;

  // create list, find the next image compared to where we stand currently
  recurse_files = parse_dirs_recursion();

  bool found_match = find_recursion_pos();

  if (found_match) {

    if (recurse_files.size() > 1) {
      if (pos_recursion == 0)
	pos_recursion = recurse_files.size() -1;
      else
	pos_recursion -= 1;
    }

    next_no_skip_folders();

    pictures_fullscreen();
  }
}

bool Pictures::reload_dir(const string& path)
{
  vector<Picture> hdfiles = rdir_hd(path, false);

  vector<Picture> dbfiles = rdir(path);

  // find what is new and what is removed
  vector<string> new_list;
  foreach (Picture& picture, hdfiles)
    new_list.push_back(picture.path);

  vector<string> old_list;
  foreach (Picture& picture, dbfiles)
    old_list.push_back(picture.path);

  vector<string> new_files;
  vector<string> removed_files;

  folder_difference(new_list, old_list, new_files, removed_files);

  // insert new into db
  foreach (string& path, new_files) {
    string original_path = path;
    if (path[path.size() - 1] == '/')
      path = path.substr(0, path.size()-1);

    string::size_type pos_backslash = path.rfind('/');
    if (pos_backslash != string::npos) {
      path = path.substr(0, pos_backslash+1);
    }

    insert_file_into_db(original_path, path);
  }

  // remove old from db
  foreach (string& path, removed_files) {
    db_mutex.enterMutex();

    char *tmp = sqlite3_mprintf("DELETE FROM Folders WHERE filename LIKE '%q%%'", path.c_str());
    db.execute(tmp);
    sqlite3_free(tmp);

    tmp = sqlite3_mprintf("DELETE FROM Pictures WHERE filename LIKE '%q%%'", path.c_str());
    db.execute(tmp);
    sqlite3_free(tmp);

    db_mutex.leaveMutex();
  }

  return (removed_files.size() > 0 || new_files.size() > 0);
}

void Pictures::check_for_changes()
{
  bool reload = false;

  list<string> top_folders = folders.top().first;

  foreach (string& dir, top_folders)
    if (reload_dir(dir))
      reload = true; // no break since we want to update the db for all folders

  reload_dirs = reload;
}

void Pictures::reload_current_dirs()
{
  check_for_changes();
  load_current_dirs();

  if (pic_list.size() == 0)
    go_back();
}

void Pictures::update_db_current_dirs()
{
  foreach (string& dir, folders.top().first){
    file_tools::remove_full_dir_from_cache(dir);
    reload_dir(dir);
  }
}

void Pictures::options()
{
  visible = false;

  S_BusyIndicator::get_instance()->idle();

  if (opts.mainloop()) {

    S_BusyIndicator::get_instance()->busy();

    update_db_current_dirs();

    reload_current_dirs();

    if (folders.top().second > pic_list.size()-1)
      folders.top().second = 0;

    thumbnailed_all_files = false;

    S_BusyIndicator::get_instance()->idle();
  }

  opts.save();

  visible = true;
}

string Pictures::id_to_filename(int db_id)
{
  string path;

  db_mutex.enterMutex();

  SQLQuery *q = db.query("Folders", ("SELECT filename FROM %t WHERE id='" + conv::itos(db_id) + "'").c_str());
  if (q && q->numberOfTuples() > 0)
    path = (*q)[0]["filename"];
  else
    assert(false);

  db_mutex.leaveMutex();

  return path;
}

bool Pictures::search_is_graphical_view(const vector<Picture>& cur_files)
{
  return true;
}

void Pictures::search_func()
{
  S_BusyIndicator::get_instance()->idle();

  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

  search_mode = true;
  search_help_offset = -1;

  std::pair<int, int> search_text_size = string_format::calculate_string_size("abcltuwHPMjJg", search_font);
  int search_size = static_cast<int>(search_text_size.second * 0.75);

  if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
    search_top_size += search_size;

  print_marked_search_letter();

  list<string> empty;
  empty.push_back("");

  screen_updater->trigger.add(TriggerElement("search marker", empty, boost::bind(&Pictures::print_marked_search_letter, this),
					     boost::bind(&Pictures::clean_up_search_print, this)));
  screen_updater->timer.add(TimeElement("search marker", boost::bind(&Pictures::check_search_letter, this),
					boost::bind(&Pictures::print_marked_search_letter, this)));

  int pos = input_master->graphical_search<Picture>(pic_list,
						    boost::bind(&Pictures::print, this, _1),
						    boost::bind(&Pictures::gen_search_list, this, _1),
						    boost::bind(&Pictures::id_to_filename, this, _1),
						    boost::bind(&Pictures::search_is_graphical_view, this, _1),
						    images_per_row, search_mode, search_str, lowercase_search_str,
						    search_depth, offset, search_help_offset,
						    S_BusyIndicator::get_instance());

  screen_updater->trigger.del("search marker");
  screen_updater->timer.del("search marker");

  force_search_print_cleanup();

  if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
    search_top_size -= search_size;

  if (pos != -1 && search_depth == dgettext("mms-pictures", "current folder"))
    folders.top().second = pos;
  else if (pos != -1)
    change_dir_to_id(pos);
}

void Pictures::exit()
{
  render->show_audio_track = true;
  exit_loop = true;
}

void Pictures::exit_fullscreen()
{
  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
  screen_updater->timer.del("pictures");

  exit();

  in_fullscreen = false;

  if (conv::stob(opts.slideshow_random())) {
    Picture cur_random_pic = vector_lookup(random_files, pos_random);

    if (conv::stob(opts.recurse_in_fullscreen()))
      find_recursion_file(true);

    folders.top().second = 0;
    foreach (Picture& pic, pic_list) {
      if (pic.path == cur_random_pic.path)
	return;
      else
	++folders.top().second;
    }

    // file not found
    folders.top().second = 0; 

 } else if (conv::stob(opts.recurse_in_fullscreen())) {
    if (folders.size() > 1) {
      folders.pop();
      load_current_dirs();
      enter_dir();
    }

    string recursion_file = vector_lookup(recurse_files, pos_recursion).path;

    int pos = 0;
    foreach (Picture& picture, pic_list) {
      if (!isDirectory(picture.path)) {
	if (picture.path == recursion_file) {
	  folders.top().second = pos;
	  return;
	}
      }
      ++pos;
    }

    // file not found
    folders.top().second = 0;
  }
}

void Pictures::find_recursion_file(bool random)
{
  fullscreen_mutex.enterMutex();

  string next_file;
  if (random)
    next_file = vector_lookup(random_files, pos_random).path;
  else
    next_file = vector_lookup(recurse_files, pos_recursion).path;
  string next_path = next_file;
  string::size_type pos;
  if ((pos = next_path.rfind('/')) != string::npos)
    next_path = next_path.substr(0, pos);

  vector<string> tested_dirs;

  while (true) {

    // quick check

    // next
    if (folders.top().second + 1 <= pic_list.size() - 1) {
      if (vector_lookup(pic_list, folders.top().second + 1).path == next_file) {
	folders.top().second += 1;
	fullscreen_mutex.leaveMutex();
	return;
      }
    }

    // prev
    if (folders.top().second - 1 >= 0 && pic_list.size() >= folders.top().second) {
      if (vector_lookup(pic_list, folders.top().second - 1).path == next_file) {
	folders.top().second -= 1;
	fullscreen_mutex.leaveMutex();
	return;
      }
    }

    // check if on current level
    list<string> dirs = folders.top().first;

    int file_pos = 0;
    foreach (Picture& picture, pic_list) {
      if (!isDirectory(picture.path)) {
	if (picture.path == next_file) {
	  folders.top().second = file_pos;
	  fullscreen_mutex.leaveMutex();
	  return;
	}
      }
      ++file_pos;
    }

    // try to find the correct subdir in current dir
    bool found_dir = false;
    file_pos = 0;
    foreach (Picture& picture, pic_list) {
      string cur_file = picture.path;
      if (cur_file.size() <= next_path.size()) {
	if (next_path.substr(0, cur_file.size()) == cur_file &&
	    !already_tested(tested_dirs, cur_file)) {
	  if (isDirectory(cur_file)) {
	    tested_dirs.push_back(cur_file);
	    folders.top().second = file_pos;
	    enter_dir();
	    found_dir = true;
	    break;
	  }
	}
      }
      ++file_pos;
    }

    if (!found_dir) {
      // it must be on a level below
      go_back();
    }
  }

  fullscreen_mutex.leaveMutex();
}

bool Pictures::already_tested(const vector<string>& tested_dirs, string dir)
{
  foreach (const string& tested_dir, tested_dirs)
    if (tested_dir == dir)
      return true;
  return false;
}

// helper functions, to minimize code copying, functions used in mainloops.
void Pictures::prev()
{
  if (folders.top().second == 0)
    folders.top().second = pic_list.size()-1;
  else
    --(folders.top().second);
}

void Pictures::next()
{
  folders.top().second = (folders.top().second+1)%pic_list.size();
}

void Pictures::prev_random()
{
  do {
    if (pos_random != 0)
      --pos_random;
    else
      pos_random = random_files.size() - 1;
  } while (vector_lookup(random_files, pos_random).type == "dir");
}

void Pictures::next_random()
{
  do {
    pos_random = (pos_random+1)%random_files.size();
  } while (vector_lookup(random_files, pos_random).type == "dir");
}

void Pictures::prev_no_skip_folders()
{
  if (pos_recursion != 0)
    --pos_recursion;
  else
    pos_recursion = recurse_files.size() - 1;

  find_recursion_file();
}

void Pictures::prev_skip_folders()
{
  do {
    prev();
  } while (vector_lookup(pic_list, folders.top().second).type == "dir");
}

void Pictures::next_no_skip_folders()
{
  if (pos_recursion != recurse_files.size() - 1)
    ++pos_recursion;
  else
    pos_recursion = 0;

  find_recursion_file();
}

void Pictures::next_skip_folders()
{
  do {
    next();
  } while (vector_lookup(pic_list, folders.top().second).type == "dir");
}

string Pictures::mainloop()
{
  visible = true;

  input_master->set_map("pictures");

  search_top_size = 30;

  Input input;

  bool update_needed = true;

  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();
  Shutdown *sd = S_Shutdown::get_instance();

  Render *render = S_Render::get_instance();
  int curlayer = render->device->get_current_layer();
  render->device->switch_to_layer(curlayer + 1);
  render->device->animation_section_begin(true);
  render->device->reset_layout_attribs_nowait();
  render->device->set_layout_alpha(0.0, curlayer + 1);
  render->device->animation_zoom(0,0,1,1,20,curlayer + 1);
  render->device->animation_fade(0,1,40,curlayer + 1);
  render->device->animation_fade(1,0,40,curlayer);
  render->device->animation_section_end();

  while (!exit_loop)
    {
      if (reload_dirs) {
	reload_current_dirs();
	update_needed = true;
	reload_dirs = false;
	if (exit_loop)
	  break;
      }

      if (update_needed)
	print(pic_list);

      input = input_master->get_input_busy_wrapped(busy_indicator);

      if (sd->is_enabled()) {
      	sd->cancel();
      	continue;
      }

      update_needed = true;

      if (input.key == "touch_input") {
	S_Touch::get_instance()->run_callback();
	continue;
      }

      if (input.command == "prev")
 	{
	  if (pic_list.size() > images_per_row) {
	    for (int i = 0; i < images_per_row; ++i) {
	      if (folders.top().second == 0) {
		folders.top().second = pic_list.size()-1;
		i += images_per_row-pic_list.size()%images_per_row;
	      }
	      else
		--(folders.top().second);
	    }
	  }
 	}
      else if (input.command == "next")
 	{
	  if (pic_list.size() > images_per_row) {
	    for (int i = 0; i < images_per_row; ++i) {
	      if (folders.top().second == pic_list.size()-1 && i == 0) {
		folders.top().second = 0;
		break;
	      } else if (folders.top().second == pic_list.size()-1) {
		break;
	      } else {
		++(folders.top().second);
	      }
	    }
	  }
 	}
      else if (input.command == "left")
 	{
	  prev();
 	}
      else if (input.command == "right")
 	{
	  next();
 	}
      else if (input.command == "page_up")
 	{
	  int jump = images_per_row*rows;

	  if (search_mode)
	    jump = images_per_row*rows_search;

	  if (pic_list.size() > jump) {
	    int diff = folders.top().second-jump;
	    if (folders.top().second == 0)
	      folders.top().second = (pic_list.size()-1)+diff;
	    else if (diff < 0)
	      folders.top().second = 0;
	    else
	      folders.top().second = diff;
	  }
 	}
      else if (input.command == "page_down")
 	{
	  int jump = images_per_row*rows;

	  if (search_mode)
	    jump = images_per_row*rows_search;

	  if (pic_list.size() > jump) {
	    if (folders.top().second >= (pic_list.size() - jump) && folders.top().second != (pic_list.size()-1))
	      folders.top().second = pic_list.size()-1;
	    else
	      folders.top().second = (folders.top().second+jump)%pic_list.size();
	  }
 	}
      else if (input.command == "recurse_dirs" && conv::stob(opts.recurse_in_fullscreen()))
	{
	  recurse_dirs();
	}
      else if (input.command == "action")
	{
	  action();
	}
      else if (input.command == "back")
	{
	  go_back();
	}
      else if (input.command == "second_action")
	{
	  ExtraMenu em;

	  string t = dgettext("mms-pictures", "Show picture in fullscreen");

	  if (vector_lookup(pic_list, folders.top().second).type == "dir")
	    t = dgettext("mms-pictures", "Enter directory");

	  em.add_item(ExtraMenuItem(t, input_master->find_shortcut("action"),
				    boost::bind(&Pictures::action, this)));

	  if (conv::stob(opts.recurse_in_fullscreen())) {
	    if (vector_lookup(pic_list, folders.top().second).type == "dir")
	      em.add_item(ExtraMenuItem(dgettext("mms-pictures", "Recurse all folders on current level"),
					input_master->find_shortcut("recurse_dirs"),
					boost::bind(&Pictures::recurse_dirs, this)));
	  }

	  em.add_item(ExtraMenuItem(dgettext("mms-pictures", "Search"), input_master->find_shortcut("search"),
				    boost::bind(&Pictures::search_func, this)));
	  em.add_item(ExtraMenuItem(dgettext("mms-pictures", "Options"), input_master->find_shortcut("options"),
				    boost::bind(&Pictures::options, this)));

	  if (folders.size() > 1) {
	    em.add_item(ExtraMenuItem(dgettext("mms-pictures", "Go up one directory"), input_master->find_shortcut("back"),
				      boost::bind(&Pictures::go_back, this)));
	    em.add_item(ExtraMenuItem(dgettext("mms-pictures", "Return to startmenu"), input_master->find_shortcut("startmenu"),
				      boost::bind(&Pictures::exit, this)));
	  } else
	    em.add_item(ExtraMenuItem(dgettext("mms-pictures", "Return to startmenu"), input_master->find_shortcut("back"),
				      boost::bind(&Pictures::exit, this)));

	  foreach (ExtraMenuItem& item, global->menu_items)
	    em.add_persistent_item(item);

	  em.mainloop();
	}
      else if (input.command == "startmenu")
 	{
	  exit();
	}
      else if (input.command == "options")
	{
	  options();
	}
      else if (input.command == "search")
	{
	  search_func();
	}
      else
	update_needed = !global->check_commands(input);
    }

  exit_loop = false;

  visible = false;

  render->device->layer_active(false);
  render->device->switch_to_layer(curlayer);
  render->device->animation_section_begin();
  render->device->reset_layout_attribs_nowait();
  render->device->animation_fade(0,1,80,curlayer);
  render->device->animation_section_end();

  return "";
}

vector<string> Pictures::images_in_dir(const list<string>& dirs)
{
  std::stack<string> queued_dirs;
  vector<string> image_files;

  libfs_set_folders_first(opts.dir_order());

  foreach (const string& dir, dirs) {
    queued_dirs.push(dir);
    while (!queued_dirs.empty()) {
      string cur_dir = queued_dirs.top();
      queued_dirs.pop();
      for (file_iterator<file_t, default_order> i (cur_dir); i != i.end (); i.advance(false)) {

	string cur_file = filesystem::FExpand(i->getName());

	// exclude folders ?
	if (pic_conf->p_excluded_picture_dirs().size() > 0) {
	  list<string> excluded = pic_conf->p_excluded_picture_dirs();
	  bool stop = false;

	  for (list<string>::const_iterator epd = excluded.begin(), epd_end = excluded.end();
	       epd != epd_end; ++epd)
	    if (cur_file.substr(cur_file.rfind("/")+1) == *epd) {
	      stop = true;
	      break;
	    }

	  if (stop)
	    continue;
	}

	if (isDirectory(cur_file))
	  queued_dirs.push(cur_file);
	else
	  image_files.push_back(cur_file);
      }
    }
  }

  return image_files;
}


void Pictures::background_thumbnails()
{
  if (!thumbnailed_all_files && (conf->p_idle_time() > 0) &&
      (opts.thumbnail_extract() == dgettext("mms-pictures", "yes"))) {

    int nr_extracts = 3;

    // check if we are idle
    int idle_time = time(0)-conf->p_last_key();
    if (idle_time < 3)
      return;
    else if (idle_time > 60 * conf->p_idle_time())
      nr_extracts = 4;

    if (all_image_files.empty()) {
      all_image_files = images_in_dir(picture_folders);
      // optimization, we don't want to both make the list and start extracting
      return;
    }

    if (conf->p_priority_change())
      nice(19); //Lower priority to keep audio from skipping

    while (!all_image_files.empty()) {

      bool extracted = false;

      // thumbnails

      if (!file_exists(render->image_get_fullpath(all_image_files.back(), image_width, image_height))) {
	extracted = true;
	render->image_mut.enterMutex();
	render->device->wait_for_not_drawing();
	render->create_scaled_image_wrapper_upscaled(all_image_files.back(), image_width, image_height);
	render->image_mut.leaveMutex();
      }

      int pic_width = static_cast<int>(image_width*1.35);
      int pic_height = static_cast<int>(image_height*1.35);

      if (!file_exists(render->image_get_fullpath(all_image_files.back(), pic_width, pic_height))) {
	extracted = true;
	render->image_mut.enterMutex();
	render->device->wait_for_not_drawing();
	render->create_scaled_image_wrapper_upscaled(all_image_files.back(), pic_width, pic_height);
	render->image_mut.leaveMutex();
      }


      // fullscreen
      if (!file_exists(render->image_get_fullpath(all_image_files.back(), conf->p_h_res(), conf->p_v_res()))) {
	extracted = true;
	render->image_mut.enterMutex();
	render->device->wait_for_not_drawing();
	render->create_scaled_image_wrapper(all_image_files.back(), conf->p_h_res(), conf->p_v_res());
	render->image_mut.leaveMutex();
      }

      if (nr_extracts == 0) {
	if (conf->p_priority_change())
	  nice(-19);
	return;
      } else if (extracted)
	--nr_extracts;

      all_image_files.erase(all_image_files.end());
    }

    if (conf->p_priority_change())
      nice(-19);

    thumbnailed_all_files = true;
  }
}

int Pictures::check_thumbnail()
{
  return 3000; // ms
}

string Pictures::get_name_from_file(const Simplefile& s)
{
  return s.name;
}

bool Pictures::search_compare(const Simplefile& s)
{
  return (s.lowercase_name.substr(0, search_str.size()) == lowercase_search_str);
}

void Pictures::prepare_and_show_fullscreen()
{
  string left_path, right_path;
  Picture current;

  if (conv::stob(opts.slideshow_random())) {

    int left_pos;
    if (pos_random != 0)
      left_pos = pos_random - 1;
    else
      left_pos = random_files.size() - 1;

    left_path = vector_lookup(random_files, left_pos).path;
    right_path = vector_lookup(random_files, (pos_random + 1) % random_files.size()).path;

    current = vector_lookup(random_files, pos_random);

  } else if (conv::stob(opts.recurse_in_fullscreen())) {

    int left_pos_recursion;
    if (pos_recursion != 0)
      left_pos_recursion = pos_recursion - 1;
    else
      left_pos_recursion = recurse_files.size() - 1;

    left_path = vector_lookup(recurse_files, left_pos_recursion).path;
    right_path = vector_lookup(recurse_files, (pos_recursion + 1) % recurse_files.size()).path;

    current = vector_lookup(recurse_files, pos_recursion);

  } else {

    int left = folders.top().second;
    do {
      left -= 1;
      if (left == -1)
	left=pic_list.size()-1;
    } while (vector_lookup(pic_list, left).type == "dir");

    int right = folders.top().second;
    do {
      right += 1;
      if (right == pic_list.size())
	right=0;
    } while (vector_lookup(pic_list, right).type == "dir");

    left_path = pic_list[left].path;
    right_path = pic_list[right].path;

    current = vector_lookup(pic_list, folders.top().second);
  }

  last_press = time(0);

  if (conf->p_priority_change())
    nice(19); //Lower priority to keep audio from skipping

  print_fullscreen(current, left_path, right_path);

  if (conf->p_priority_change())
    nice(-19);
}

void Pictures::zoom()
{
  if (zoom_level == 1) {

    // calculate zoom position
    render->image_mut.enterMutex();
    PFSObj p = PFSObj(vector_lookup(pic_list, folders.top().second).path,
		      false, orientation, zoom_level, zoom_position, 1, 1, 1);
    render->image_mut.leaveMutex();

    // don't zoom small images
    if (p.real_w < conf->p_h_res() && p.real_h < conf->p_v_res())
      return;

    zoom_level = 2;

    int pic_res_w = p.real_w*zoom_level;
    max_pos_v = 3; // default
    if (pic_res_w <= conf->p_h_res())
      max_pos_v = 1;
    else if (pic_res_w <= int(1.5*conf->p_h_res()))
      max_pos_v = 2;

    int pic_res_h = p.real_h*zoom_level;
    max_pos_h = 3; // default
    if (pic_res_h <= conf->p_v_res())
      max_pos_h = 1;
    else if (pic_res_h <= int(1.5*conf->p_v_res()))
      max_pos_h = 2;

    zoom_pos_h = int(max_pos_h/2)+1;
    zoom_pos_v = int(max_pos_v/2)+1;

    zoom_position = (zoom_pos_h-1)*max_pos_v+zoom_pos_v-1;

  } else {
    zoom_level = 1;
    zoom_position = 1;
  }
}

void Pictures::suspend()
{
  pause = !pause;
}

void Pictures::rotate_right()
{
  orientation = (orientation+1)%4;
  set_db_orientation_lookup();
}

void Pictures::rotate_left()
{
  orientation = (orientation == 0) ? 3 : orientation - 1;
  set_db_orientation_lookup();
}

void Pictures::options_fullscreen()
{
  bool recursion_before = conv::stob(opts.recurse_in_fullscreen());

  options();

  if (!recursion_before && conv::stob(opts.recurse_in_fullscreen())) {
    recurse_files = parse_dirs_recursion();
    find_recursion_pos();
  }

  if (conv::stob(opts.slideshow_random()))
    set_random_files();
}

void Pictures::deactivate_updaters()
{
  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
  screen_updater->timer.deactivate("pictures");
}

void Pictures::activate_updaters()
{
  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
  if (opts.slideshow() != "0")
    screen_updater->timer.activate("pictures");
}

int Pictures::get_db_orientation_lookup()
{
  if (conv::stob(opts.slideshow_random()))
    return get_db_orientation(vector_lookup(random_files, pos_random).db_id);
  else if (conv::stob(opts.recurse_in_fullscreen()))
    return get_db_orientation(vector_lookup(recurse_files, pos_recursion).db_id);
  else
    return get_db_orientation(vector_lookup(pic_list, folders.top().second).db_id);
}

void Pictures::set_db_orientation_lookup()
{
  if (conv::stob(opts.slideshow_random()))
    return set_db_orientation(vector_lookup(random_files, pos_random).db_id);
  else if (conv::stob(opts.recurse_in_fullscreen()))
    return set_db_orientation(vector_lookup(recurse_files, pos_recursion).db_id);
  else
    return set_db_orientation(vector_lookup(pic_list, folders.top().second).db_id);
}

void Pictures::fullscreen_left()
{
  if (conv::stob(opts.slideshow_random())) {
    if (pos_random == 0)
      pos_random = random_files.size()-1; 
    else
      --pos_random;
  } else if (conv::stob(opts.recurse_in_fullscreen()))
    prev_no_skip_folders();
  else
    prev_skip_folders();

  orientation = get_db_orientation_lookup();
}

void Pictures::fullscreen_right()
{
  if (conv::stob(opts.slideshow_random()))
    pos_random = (pos_random+1)%random_files.size();
  else if (conv::stob(opts.recurse_in_fullscreen()))
    next_no_skip_folders();
  else
    next_skip_folders();

  orientation = get_db_orientation_lookup();
}

void Pictures::set_random_files()
{
  Picture tmp_pic;

  if (conv::stob(opts.recurse_in_fullscreen())) {
    tmp_pic = vector_lookup(recurse_files, pos_recursion);
    random_files = recurse_files;
  } else  {
    tmp_pic = vector_lookup(pic_list, folders.top().second);
    random_files = pic_list;
  }

  std::random_shuffle(random_files.begin(), random_files.end());

  pos_random = 0;
  foreach (Picture& pic, random_files) {
    if (pic == tmp_pic)
      break;
    else
      ++pos_random;
  }
}

void Pictures::pictures_fullscreen()
{
  if (conv::stob(opts.slideshow_random()))
    set_random_files();

  in_fullscreen = true;

  Input input;

  orientation = get_db_orientation_lookup();

  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

  screen_updater->timer.add(TimeElement("pictures", boost::bind(&Pictures::check_picture_time, this),
					boost::bind(&Pictures::pictures_check, this)));

  if (opts.slideshow() == "0")
    screen_updater->timer.deactivate("pictures");

  // disable screensaver if it's running
  bool audio_fullscreen_running = true;

  if (!screen_updater->timer.status("audio_fullscreen"))
    audio_fullscreen_running = false;
  else
    screen_updater->timer.deactivate("audio_fullscreen");

  bool update_needed = true;

  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();

#ifdef use_notify_area
  nArea->disable();
#endif

  while (!exit_loop)
    {
      global->set_playback_offset(image_width + 10);

      prepare_and_show_fullscreen();

      input = input_master->get_input_busy_wrapped(busy_indicator);

      update_needed = true;

      global->set_playback_offset(0);

      if (input.key == "touch_input") {
	S_Touch::get_instance()->run_callback();
	continue;
      }

      if (zoom_level != 1) {

	if (input.command == "prev") {
	  if (zoom_pos_h != 1)
	    --zoom_pos_h;
	} else if (input.command == "next") {
	  if (zoom_pos_h < max_pos_h)
	    ++zoom_pos_h;
	} else if (input.command == "left") {
	  if (zoom_pos_v != 1)
	    --zoom_pos_v;
	} else if (input.command == "right") {
	  if (zoom_pos_v < max_pos_v)
	    ++zoom_pos_v;
	} else if (input.command == "action" || (input.command == "back" && input.mode == "general")) {
	  zoom_level = 1;
	  zoom_position = 1;
	  activate_updaters();
	  continue;
	} else
	  global->check_commands(input);

	// calculate zoom_position
	zoom_position = (zoom_pos_h-1)*max_pos_v + zoom_pos_v - 1;

	continue;
      }

      if (input.command == "back" && input.mode == "general")
	{
	  exit_fullscreen();
	}
      else if (input.command == "startmenu")
 	{
	  exit_fullscreen_to_smenu();
	}
      else if (input.command == "left")
	{
	  // we don't want to change left or right since they are ok for touch
	  // screen even in random mode
	  fullscreen_left();
	}
      else if (input.command == "right")
	{
	  fullscreen_right();
	}
      else if (input.command == "page_up")
 	{
	  int jump = images_per_row*rows;

	  if (search_mode)
	    jump = images_per_row*rows_search;

	  int list_size = pic_list.size();
	  if (conv::stob(opts.slideshow_random()))
	    list_size = random_files.size();
	  else if (conv::stob(opts.recurse_in_fullscreen()))
	    list_size = recurse_files.size();

	  if (list_size > jump) {
	    for (int i = 0; i < jump; ++i) {

	      if (conv::stob(opts.slideshow_random()))
		prev_random();
	      else if (conv::stob(opts.recurse_in_fullscreen()))
		prev_no_skip_folders();
	      else
		prev_skip_folders();
	    }
	    
	    orientation = get_db_orientation_lookup();
	  }
 	}
      else if (input.command == "page_down")
 	{
	  int jump = images_per_row*rows;

	  if (search_mode)
	    jump = images_per_row*rows_search;

	  int list_size = pic_list.size();
	  if (conv::stob(opts.slideshow_random()))
	    list_size = random_files.size();
	  else if (conv::stob(opts.recurse_in_fullscreen()))
	    list_size = recurse_files.size();

	  if (list_size > jump) {
	    for (int i = 0; i < jump; ++i) {
	      if (conv::stob(opts.slideshow_random()))
		next_random();
	      if (conv::stob(opts.recurse_in_fullscreen()))
		next_no_skip_folders();
	      else
		next_skip_folders();
	    }

	    orientation = get_db_orientation_lookup();
	  }
 	}
      else if (input.command == "next")
	{
	  rotate_right();
	}
      else if (input.command == "prev")
	{
	  rotate_left();
	}
      else if (input.command == "action")
	{
	  zoom();

	  if (zoom_level != 1)
	    deactivate_updaters();
	  else
	    activate_updaters();
	}
      else if (input.command == "suspend")
	{
	  deactivate_updaters();
	  suspend();
	  activate_updaters();
	}
      else if (input.command == "second_action")
	{
	  ExtraMenu em;

	  em.add_item(ExtraMenuItem(dgettext("mms-pictures", "Zoom"), input_master->find_shortcut("action"),
				    boost::bind(&Pictures::zoom, this), true));

	  string slideshow_status = dgettext("mms-pictures", "Suspend slideshow");

	  if (pause)
	    slideshow_status = dgettext("mms-pictures", "Resume slideshow");

	  em.add_item(ExtraMenuItem(slideshow_status, input_master->find_shortcut("suspend"),
				    boost::bind(&Pictures::suspend, this)));

	  em.add_item(ExtraMenuItem(dgettext("mms-pictures", "Rotate left"), input_master->find_shortcut("prev"),
				    boost::bind(&Pictures::rotate_left, this)));

	  em.add_item(ExtraMenuItem(dgettext("mms-pictures", "Rotate right"), input_master->find_shortcut("next"),
				    boost::bind(&Pictures::rotate_right, this)));

	  em.add_item(ExtraMenuItem(dgettext("mms-pictures", "Options"), input_master->find_shortcut("options"),
				    boost::bind(&Pictures::options_fullscreen, this)));

	  em.add_item(ExtraMenuItem(dgettext("mms-pictures", "Leave fullscreen"),
				    input_master->find_shortcut("back"),
				    boost::bind(&Pictures::exit_fullscreen, this)));

	  em.add_item(ExtraMenuItem(dgettext("mms-pictures", "Return to startmenu"),
				    input_master->find_shortcut("startmenu"),
				    boost::bind(&Pictures::exit_fullscreen_to_smenu, this)));

	  foreach (ExtraMenuItem& item, global->menu_items)
	    em.add_persistent_item(item);

	  deactivate_updaters();

	  global->set_playback_offset(image_width + 10);

	  em.mainloop();

	  if (zoom_level == 1)
	    activate_updaters();
	}
      else if (input.command == "options")
	{
	  deactivate_updaters();
	  options_fullscreen();
	  activate_updaters();
	}
      else
	update_needed = !global->check_commands(input);
    }

  if (zoom_level != 1) {
    zoom_level = 1;
    zoom_position = 1;
  }

  if (!exit_startmenu)
    exit_loop = false;
  else
    exit_startmenu = false;

  global->set_playback_offset(0);
  S_SSaverobj::get_instance()->ResetCounter();

  if (audio_fullscreen_running)
    screen_updater->timer.activate("audio_fullscreen");

#ifdef use_notify_area
  nArea->enable();
#endif
}

void Pictures::exit_fullscreen_to_smenu()
{
  exit_fullscreen();
  exit_startmenu = true;
}

void Pictures::print(const vector<Picture>& cur_list)
{
  render->prepare_new_image();

  render->current.add(new PObj(themes->pictures_background, 0, 0, 0, SCALE_FULL));

  PObj *back = new PFObj(themes->startmenu_pictures_dir, 25, 10, header_box_size, header_box_size, 2, true);

  render->current.add(back);

  S_Touch::get_instance()->register_area(TouchArea(rect(back->x, back->y, back->w, back->h), back->layer,
						   boost::bind(&Pictures::go_back, this)));

  if (themes->show_header) {
    string header = dgettext("mms-pictures", "Pictures");

    if (folders.size() > 1) {
      string top_folder = folders.top().first.front();

      if (top_folder[top_folder.size()-1] == '/')
	top_folder = top_folder.substr(0, top_folder.size()-1);

      assert(top_folder.rfind('/') != string::npos);
      header += " - " + top_folder.substr(top_folder.rfind('/')+1);
      string_format::format_to_size(header, header_font, conf->p_h_res()-220, false);
    }

    std::pair<int, int> header_size = string_format::calculate_string_size(header, header_font);

    TObj *t = new TObj(header, header_font, 25 + header_box_size + 10, 10 + (header_box_size - header_size.second)/2,
		       themes->pictures_header_font1, themes->pictures_header_font2,
		       themes->pictures_header_font3, 2);

    render->current.add(t);

    S_Touch::get_instance()->register_area(TouchArea(rect(t->x, t->y, t->w, t->h), t->layer, global->toggle_playlist));
  }

  if (search_mode) {
    int y = 10 + header_box_size + 10;

    std::pair<int, int> search_text_size = string_format::calculate_string_size("abcltuwHPMjJg", search_font);
    int search_size = static_cast<int>(search_text_size.second * 0.75);

    if (offset == -1)
      render->current.add(new RObj(0, y, conf->p_h_res(), search_size + 5, 0, 0, 0, 215, 2));
    else
      render->current.add(new RObj(0, y, conf->p_h_res(), search_size + 5,
				   themes->general_search_rectangle_color1,
				   themes->general_search_rectangle_color2,
				   themes->general_search_rectangle_color3,
				   themes->general_search_rectangle_color4, 2));

    render->current.add(new PFObj(themes->general_search, 47, y, search_size, search_size, 3, true));

    std::pair<int, int> search_select_sizes = string_format::calculate_string_size(search_depth, search_select_font);

    int max_x = conf->p_h_res()-search_select_sizes.first - 25;

    string sw = search_str;

    string s = dgettext("mms-pictures", "Search: ");
    int x_size = string_format::calculate_string_width(s, search_font);
    string_format::format_to_size(sw, search_font, max_x-x_size, true);

    int search_text_height = string_format::calculate_string_size(s + sw, search_font).second;

    render->current.add(new TObj(s + sw, search_font, 47 + search_size + 10,
				 y + (search_size + 5 - search_text_height)/2,
				 themes->search_font1, themes->search_font2, themes->search_font3, 3));

    int search_select_height = string_format::calculate_string_size(search_depth, search_select_font).second;

    render->current.add(new TObj(search_depth, search_select_font, max_x,
				 y + (search_size + 5 - search_select_height)/2,
				 themes->search_font1, themes->search_font2, themes->search_font3, 3));
  }

  // print the range
  int pos, start, pos_in_list, cur_rows, cur_image_height_all;

  if (search_mode) {
    if (offset == -1)
      pos_in_list = 0;
    else
      pos_in_list = offset;
    cur_rows = rows_search;
    cur_image_height_all = image_height_all_search;
  } else {
    pos_in_list = folders.top().second;
    cur_rows = rows;
    cur_image_height_all = image_height_all;
  }

  int nr_in_last_row = cur_list.size()%images_per_row;
  if (nr_in_last_row == 0)
    nr_in_last_row = images_per_row;

  if (cur_list.size() > cur_rows*images_per_row) {
    if (pos_in_list < std::ceil(double(cur_rows)/2.0)*images_per_row) { // first two rows
      pos = pos_in_list;
      start = 0;
    } else if (pos_in_list >= cur_list.size() - (images_per_row+nr_in_last_row)) { // last two rows
      start = cur_list.size()-((cur_rows-1)*images_per_row+nr_in_last_row);
      pos = pos_in_list-start;
    } else {
      pos = pos_in_list%images_per_row+images_per_row;
      if (pos == images_per_row-1)
	pos = 2*images_per_row-1;
      start = pos_in_list-pos;
    }
  } else {
    pos = pos_in_list;
    start = 0;
  }

  int x = (conf->p_h_res()-images_per_row*image_width_all)/2;
  int x_start = x;
  int y = y_start;

  if (search_mode) {
    std::pair<int, int> search_text_size = string_format::calculate_string_size("abcltuwHPMjJg", search_font);
    int search_size = search_text_size.second;

    y += search_size;

    if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
      y += 49;
  }

  for (int j = 0; j < cur_rows*images_per_row && j < cur_list.size(); ++j) {

    Simplefile s = vector_lookup(cur_list, start);

    int y_pos_offset = 0;

    if (s.type == "dir") {

      int pic_width = static_cast<int>(image_width/1.15);
      int pic_height = static_cast<int>(image_height/1.15);
      int pic_width_all = image_width_all;
      int pic_height_all_eks_text = image_height_all_eks_text;

      int max_pic_width = static_cast<int>(pic_width * 1.4);
      int max_pic_height = static_cast<int>(pic_height * 1.4);

      string picture_path = render->default_path + themes->general_normal_dir;

      if (pos == j) {
	pic_width = max_pic_width;
	pic_height = max_pic_height;
	pic_width_all += pic_width - image_width;
	pic_height_all_eks_text += pic_height - image_height;
	x -= (pic_width - image_width)/2;
	y_pos_offset = (pic_height - image_height)/2;
	y -= y_pos_offset;
	picture_path = render->default_path + themes->pictures_dir_marked;
      }

      render->create_scaled_image_wrapper_upscaled(picture_path, max_pic_width, max_pic_height);
      // use pic with max_width and max_height and scale it in software
      PFObj *p = new PFObj(picture_path, x, y, max_pic_width, max_pic_height, pic_width, pic_height, 3);

      p->x = p->real_x = x + (pic_width_all - p->real_w)/2;
      p->y = p->real_y = y + (pic_height_all_eks_text - p->real_h)/2;

      p->real_x -= 1;
      p->real_y -= 1;

      if (pos == j) {
	x += (pic_width - image_width)/2;
	y += y_pos_offset * 2; // shift text also
      }

      render->current.add(p);

      S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&Pictures::find_element_and_do_action, this, s)));

    } else { // file

      bool border = false;

      int pic_width = image_width;
      int pic_height = image_height;
      int pic_width_all = image_width_all;
      int pic_height_all_eks_text = image_height_all_eks_text;

      int max_pic_width = static_cast<int>(pic_width * 1.35);
      int max_pic_height = static_cast<int>(pic_height * 1.35);

      if (pos == j) {
	pic_width = max_pic_width;
	pic_height = max_pic_height;
	pic_width_all += pic_width - image_width;
	pic_height_all_eks_text += pic_height - image_height;
	x -= (pic_width - image_width)/2;
	y_pos_offset = (pic_height - image_height)/2;
	y -= y_pos_offset;
      }

      render->create_scaled_image_wrapper_upscaled(s.path, max_pic_width, max_pic_height);
      // use pic with max_width and max_height and scale it in software
      PFObj *p = new PFObj(s.path, x, y, max_pic_width, max_pic_height, pic_width, pic_height, 3);

      p->x = p->real_x = x + (pic_width_all - (p->w + 10))/2;
      p->y = p->real_y = y + (pic_height_all_eks_text - (p->h + 10))/2;

      p->real_x -= 1;
      p->real_y -= 1;

      x += (pic_width - image_width)/2;
      y += y_pos_offset * 2; // shift text also

      render->current.add(p);

      S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&Pictures::find_element_and_do_action, this, s)));
    }

    string name = s.name;
    string old_name = name;

    string_format::format_to_size(name, normal_font, image_width_all-10, false);

    string name_second_part = old_name.substr(name.size());

    if (name_second_part.size() > 0 && name_second_part[0] != ' ') {
      // splitting words
      string::size_type last_whole_word = name.rfind(' ');
      if (last_whole_word != string::npos) {
	name = name.substr(0, last_whole_word);
	name_second_part = old_name.substr(name.size());
      }
    }

    int x_size = string_format::calculate_string_width(name, normal_font);

    string_format::format_to_size(name_second_part, normal_font, image_width_all-10, false);

    int x_size2 = string_format::calculate_string_width(name_second_part, normal_font);

    if (pos == j) {
      render->current.add(new TObj(name, normal_font, x + (image_width_all-x_size)/2, y+image_height_all_eks_text,
				   themes->pictures_marked_font1, themes->pictures_marked_font2,
				   themes->pictures_marked_font3, 3));
      render->current.add(new TObj(name_second_part, normal_font, x + (image_width_all-x_size2)/2,
				   y+image_height_all_eks_text+normal_font_height,
				   themes->pictures_marked_font1, themes->pictures_marked_font2,
				   themes->pictures_marked_font3, 3));
      y -= y_pos_offset;
    } else {
      render->current.add(new TObj(name, normal_font, x + (image_width_all-x_size)/2, y+image_height_all_eks_text,
				   themes->pictures_font1, themes->pictures_font2,
				   themes->pictures_font3, 3));
      render->current.add(new TObj(name_second_part, normal_font, x + (image_width_all-x_size2)/2,
				   y+image_height_all_eks_text+normal_font_height,
				   themes->pictures_font1, themes->pictures_font2,
				   themes->pictures_font3, 3));
    }

    x += image_width_all;

    if (start == cur_list.size()-1) {
      if ((j+1)%images_per_row != 0)
	j += (images_per_row-(j+1)%images_per_row);

      x = x_start;
      y += cur_image_height_all;

      start = 0;
      continue;

    } else
      ++start;

    if ((j+1)%images_per_row == 0) {
      y += cur_image_height_all;
      x = x_start;
    }
  }

  std::ostringstream position;

  if (search_mode)
    if (cur_list.size() > 0)
      position << pos_in_list + 1 << "/" << cur_list.size();
    else
      position << "";
  else
    position << folders.top().second+1 << "/" << cur_list.size();

  int x_size = string_format::calculate_string_width(position.str(), position_font);

  render->current.add(new TObj(position.str(), position_font, conf->p_h_res()-(60+x_size), 20,
			       themes->pictures_font1, themes->pictures_font2,
			       themes->pictures_font3, 3));

  render->draw_and_release("pictures");
}

void Pictures::print_fullscreen(const Simplefile& e, const string& left_path, const string& right_path)
{
  // sanity
  if (e.type == "dir")
    return;

  if (conf->p_priority_change())
    nice(19);

  render->prepare_new_image();

  render->current.add(new RObj(0, 0, conf->p_h_res(), conf->p_v_res(), 0, 0, 0, 255, 0));

  // FIXME: create thumbnail for at least the db stored rotation

  if (zoom_level != 1 || orientation != 0) {
    PFSObj *p = new PFSObj(e.path, conv::stob(opts.zoom()), orientation,
			   zoom_level, zoom_position, max_pos_v, max_pos_h, 1);

    S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer,
						     boost::bind(&Pictures::exit_fullscreen, this)));

    render->current.add(p);
  } else {
    // use thumbnailed for performance
    render->create_scaled_image_wrapper(e.path, conf->p_h_res(), conf->p_v_res());

    PFSObj *p = new PFSObj(render->image_get_fullpath(e.path, conf->p_h_res(), conf->p_v_res()),
			   conv::stob(opts.zoom()), orientation, zoom_level, zoom_position,
			   max_pos_v, max_pos_h, 1);

    S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer,
						     boost::bind(&Pictures::exit_fullscreen, this)));

    render->current.add(p);
  }

  if (conv::stob(opts.show_preview()) && zoom_level == 1) {

    // make sure the thumbnails are there
    render->create_scaled_image_wrapper(left_path, image_width, image_height);
    render->create_scaled_image_wrapper(right_path, image_width, image_height);

    bool is_transparent;

    // left
    std::pair<int, int> dimensions = render->image_dimensions(render->image_get_fullpath(left_path, image_width, image_height), &is_transparent);

    PFObj *left = new PFObj(left_path, 5, conf->p_v_res()-dimensions.first-5, image_width, image_height, false, 2);
    render->current.add(left);

    S_Touch::get_instance()->register_area(TouchArea(rect(left->x, left->y, left->w, left->h), left->layer, boost::bind(&Pictures::fullscreen_left, this)));

    // right
    dimensions = render->image_dimensions(render->image_get_fullpath(right_path, image_width, image_height), &is_transparent);

    PFObj *right = new PFObj(right_path, conf->p_h_res()-dimensions.second-5, conf->p_v_res()-dimensions.first-5, image_width, image_height, false, 2);
    render->current.add(right);

    S_Touch::get_instance()->register_area(TouchArea(rect(right->x, right->y, right->w, right->h), right->layer, boost::bind(&Pictures::fullscreen_right, this)));
  }

  std::ostringstream position;
  if (conv::stob(opts.slideshow_random()))
    position << pos_random+1 << "/" << random_files.size();
  else if (conv::stob(opts.recurse_in_fullscreen()))
    position << pos_recursion+1 << "/" << recurse_files.size();
  else
    position << folders.top().second+1 << "/" << pic_list.size();

  int x_size = string_format::calculate_string_width(position.str(), position_font);

  int x_pos = 20, y_pos = 20;

  if (conf->p_busy_indicator()) {
    x_pos = 35;
    y_pos = 10;
  }

  if (opts.show_info() == dgettext("mms-pictures","Title") || opts.show_info() == dgettext("mms-pictures","None"))
    render->show_audio_track = false;
  else
    render->show_audio_track = true;

  if (opts.show_info() == dgettext("mms-pictures","Full") || opts.show_info() == dgettext("mms-pictures","Title"))
    render->current.add(new TObj(position.str(), position_font,
				 conf->p_h_res()-(x_pos+x_size), y_pos,
				 themes->pictures_font1, themes->pictures_font2,
				 themes->pictures_font3, 2));

  string name = e.name;

  if (pause && conv::stob(opts.slideshow()))
    name += dgettext("mms-pictures", " ( paused )");

  string_format::format_to_size(name, fullscreen_font, conf->p_h_res()-(20+x_size+30), false);

  if ((opts.show_info() == dgettext("mms-pictures","Title")) || (opts.show_info() == dgettext("mms-pictures","Full")))
    render->current.add(new TObj(name, fullscreen_font, 20, 20,
				 themes->pictures_font1, themes->pictures_font2,
				 themes->pictures_font3, 2));

  render->draw_and_release("pictures full");

  if (conf->p_priority_change())
    nice(-19);
}

void Pictures::play_pictures_cd()
{
  navigating_media = true;

  top_media_folders.clear();
  top_media_folders.push_back(cd->get_mount_point());

  if (cd->data_disc()) {
    // backup ;)
    vector<Picture> old_list = pic_list;
    std::stack<std::pair<list<string>, int > > temp_folders = folders;
    while (!folders.empty())
      folders.pop();
    list<string> templs; templs.push_back(cd->get_mount_point());
    folders.push(std::make_pair(templs, 0));

    //mount the disc
    run::external_program("mount '" + cd->get_mount_point() + "'");

    bool valid_files = testdir(cd->get_mount_point());

    pic_list = rdir(cd->get_mount_point());

    if (pic_list.size() == 0 || !valid_files)
      Print pdialog(dgettext("mms-pictures", "The disc does not contain any recognizable files"),
		    Print::SCREEN);
    else
      mainloop();

    //umount the disc
    run::external_program("umount '" + cd->get_mount_point() + "'");

    pic_list = old_list;
    folders = temp_folders;
  } else
    Print pdialog(dgettext("mms-pictures", "The disc does not contain any recognizable files"),
		  Print::SCREEN);

  navigating_media = false;
}

// ====================================================================
//
// dir
//
// ====================================================================

// EXIF Picture Orientation - Read one byte, testing for EOF
int read_1_byte(FILE * myfile)
{
  return getc(myfile);
}

// EXIF Picture Orientation - Read 2 bytes, convert to unsigned int
// All 2-byte quantities in JPEG markers are MSB first
static unsigned int read_2_bytes(FILE * myfile)
{
  int c1, c2;

  c1 = getc(myfile);
  if (c1 == EOF)
    return EOF;

  c2 = getc(myfile);
  if (c2 == EOF)
    return EOF;

  return (((unsigned int) c1) << 8) + ((unsigned int) c2);
}

// EXIF Picture Orientation - Read the JPEG EXIF Picture Orientation
// The Exif orientation value gives the orientation of the camera
// relative to the scene when the image was captured.
int read_exif_orientation (string filepath)
{
  int n_flag, set_flag;
  unsigned int length, i;
  int is_motorola; /* Flag for byte order */
  unsigned int offset, number_of_tags, tagnum;
  static unsigned char exif_data[65536L];


  n_flag = 0; set_flag = 0;

  FILE * myfile = fopen(filepath.c_str(), "rb");
  if (myfile == NULL)
    return 0;

  /* Read File head, check for JPEG SOI + Exif APP1 */
  for (i = 0; i < 4; i++)
    exif_data[i] = (unsigned char) read_1_byte(myfile);
  if (exif_data[0] != 0xFF ||
      exif_data[1] != 0xD8 ||
      exif_data[2] != 0xFF ||
      exif_data[3] != 0xE1)
    goto Exit0;

  /* Get the marker parameter length count */
  length = read_2_bytes(myfile);
  /* Length includes itself, so must be at least 2 */
  /* Following Exif data length must be at least 6 */
  if (length < 8)
    goto Exit0;
  length -= 8;
  /* Read Exif head, check for "Exif" */
  for (i = 0; i < 6; i++)
    exif_data[i] = (unsigned char) read_1_byte(myfile);
  if (exif_data[0] != 0x45 ||
      exif_data[1] != 0x78 ||
      exif_data[2] != 0x69 ||
      exif_data[3] != 0x66 ||
      exif_data[4] != 0 ||
      exif_data[5] != 0)
    goto Exit0;
  /* Read Exif body */
  for (i = 0; i < length; i++)
    exif_data[i] = (unsigned char) read_1_byte(myfile);

  if (length < 12) goto Exit0; /* Length of an IFD entry */

  /* Discover byte order */
  if (exif_data[0] == 0x49 && exif_data[1] == 0x49)
    is_motorola = 0;
  else if (exif_data[0] == 0x4D && exif_data[1] == 0x4D)
    is_motorola = 1;
  else
    goto Exit0;

  /* Check Tag Mark */
  if (is_motorola) {
    if (exif_data[2] != 0) goto Exit0;
    if (exif_data[3] != 0x2A) goto Exit0;
  } else {
    if (exif_data[3] != 0) goto Exit0;
    if (exif_data[2] != 0x2A) goto Exit0;
  }

  /* Get first IFD offset (offset to IFD0) */
  if (is_motorola) {
    if (exif_data[4] != 0) goto Exit0;
    if (exif_data[5] != 0) goto Exit0;
    offset = exif_data[6];
    offset <<= 8;
    offset += exif_data[7];
  } else {
    if (exif_data[7] != 0) goto Exit0;
    if (exif_data[6] != 0) goto Exit0;
    offset = exif_data[5];
    offset <<= 8;
    offset += exif_data[4];
  }
  if (offset > length - 2) goto Exit0; /* check end of data segment */

  /* Get the number of directory entries contained in this IFD */
  if (is_motorola) {
    number_of_tags = exif_data[offset];
    number_of_tags <<= 8;
    number_of_tags += exif_data[offset+1];
  } else {
    number_of_tags = exif_data[offset+1];
    number_of_tags <<= 8;
    number_of_tags += exif_data[offset];
  }
  if (number_of_tags == 0) goto Exit0;
  offset += 2;

  /* Search for Orientation Tag in IFD0 */
  for (;;) {
    if (offset > length - 12) goto Exit0; /* check end of data segment */
    /* Get Tag number */
    if (is_motorola) {
      tagnum = exif_data[offset];
      tagnum <<= 8;
      tagnum += exif_data[offset+1];
    } else {
      tagnum = exif_data[offset+1];
      tagnum <<= 8;
      tagnum += exif_data[offset];
    }
    if (tagnum == 0x0112) break; /* found Orientation Tag */
    if (--number_of_tags == 0) goto Exit0;
    offset += 12;
  }
  /* Get the Orientation value */
  if (is_motorola) {
    if (exif_data[offset+8] != 0) goto Exit0;
    set_flag = exif_data[offset+9];
  }
  else {
    if (exif_data[offset+9] != 0) goto Exit0;
    set_flag = exif_data[offset+8];
  }
  fclose(myfile);
  if (set_flag == 3) return 2;
  if (set_flag == 6) return 1;
  if (set_flag == 8) return 3;
  /* return 0 if there is no rotation */
  return 0;

Exit0:
  fclose(myfile);
  return 0;
}

bool Pictures::testdir(const string& dir)
{
  libfs_set_folders_first(opts.dir_order());

  for (file_iterator<file_t, default_order> i (dir); i != i.end (); ++i) {
    if (global->check_stop_bit())
      break;

    if (check_type((*i).getName(), conf->p_filetypes_p()) != emptyMyPair)
      return true;
  }

  return false;
}

void Pictures::insert_picture_into_db(const string& filename, const string& name)
{
  db_mutex.enterMutex();

// Read the JPEG EXIF Picture Orientation
  int pic_orientation = read_exif_orientation (filename);

  SQLQuery *q = db.query("Pictures", ("SELECT * FROM %t WHERE filename='" + string_format::escape_db_string(filename) + "'").c_str());
  if (q && q->numberOfTuples() > 0) {
    delete q;
    db_mutex.leaveMutex();
    return; // file already exists, don't insert
  } else
    delete q;

  struct stat64 s;
  stat64(filename.c_str(), &s);

  // FIXME: group id (value 6)
  char *tmp = sqlite3_mprintf("INSERT INTO Pictures VALUES(NULL, '%q', '%q', '%q', '%i', 0, 0, '%q')",
			      filename.c_str(), name.c_str(), "", s.st_mtime, conv::itos(pic_orientation).c_str());
  db.execute(tmp);
  sqlite3_free(tmp);

  db_mutex.leaveMutex();
}

// Add an picture to the list
Picture Pictures::addfile(const string& name, const MyPair& filetype)
{
  Picture p = addsimplefile(name, filetype);

  insert_picture_into_db(name, p.name);

  return p;
}

void Pictures::insert_file_into_db(const string& filename, const string& parent)
{
  bool is_dir = isDirectory(filename);

  string good_filename;

  if (is_dir)
    good_filename = string_format::unique_folder_name(filename);
  else
    good_filename = filename;

  db_mutex.enterMutex();

  // check if already exists
  SQLQuery *q = db.query("Folders", ("SELECT id FROM %t WHERE filename='" + string_format::escape_db_string(good_filename) + "'").c_str());
  if (q && q->numberOfTuples() > 0) {
    delete q;
    db_mutex.leaveMutex();
    return;
  } else
    delete q;

  string parent_id = get_parent_id(parent, db, navigating_media ? top_media_folders : picture_folders);

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

  db_mutex.leaveMutex();
}

int Pictures::db_id(const string& file, bool is_dir)
{
  int i = -1;

  string good_file;

  if (is_dir)
    good_file = string_format::unique_folder_name(file);
  else
    good_file = file;

  db_mutex.enterMutex();

  SQLQuery *q = db.query("Folders", ("SELECT id FROM %t WHERE filename='" + string_format::escape_db_string(good_file) + "'").c_str());
  if (q && q->numberOfTuples() > 0) {
    SQLRow &row = (*q)[0];
    i = conv::atoi(row["id"]);
  }
  delete q;

  db_mutex.leaveMutex();

  return i;
}

bool Pictures::rdir_internal(const string& original_name, const string& argv,
			     vector<Picture> &cur_files, bool file_in_db)
{
  if (global->check_stop_bit()) {
    cur_files.clear();
    return false;
  }

  string cur_file = original_name;
  if (cur_file[cur_file.size()-1] == '/')
    cur_file = cur_file.substr(0, cur_file.size()-1);

  string name;
  if (argv[argv.size()-1] != '/')
    name = cur_file.substr(argv.size()+1);
  else
    name = cur_file.substr(argv.size());

  if (isDirectory (cur_file)) {
    // exclude folders ?

    if (pic_conf->p_excluded_picture_dirs().size() > 0) {
      list<string> excluded = pic_conf->p_excluded_picture_dirs();
      bool stop = false;

      foreach (string& excluded_dir, excluded)
	if (cur_file.substr(cur_file.rfind("/")+1) == excluded_dir) {
	  stop = true;
	  break;
	}

      if (stop)
	return true;
    }
   if(!file_tools::dir_contains_known_files(cur_file, ext_mask, 20))
      return true;

    // dir
    Picture r;
    r.id = ++id;
    r.name = name;
    if (conf->p_convert())
      r.name = string_format::convert(r.name);
    r.lowercase_name = string_format::lowercase(r.name);
    r.path = cur_file;
    r.type="dir";
    if (file_in_db)
      r.db_id = db_id(r.path, true);

    cur_files.push_back(r);

  } else {
    // file
    const MyPair filetype = check_type(cur_file, conf->p_filetypes_p());
    if (filetype != emptyMyPair)
      cur_files.push_back(get_file_from_path(cur_file, filetype, file_in_db));

    ++S_ScreenUpdater::get_instance()->status_progressbar;
  }

  return true;
}

Picture Pictures::get_file_from_path(const string& path, MyPair filetype, bool file_in_db)
{
  Picture p;
  if (in_fullscreen)
    p = addsimplefile(path, filetype);
  else
    p = addfile(path, filetype);
  if (file_in_db)
    p.db_id = db_id(p.path, false);
  return p;
}

vector<string> Pictures::check_db_for_folders(const string& parent)
{
  vector<string> names;

  string good_parent = string_format::unique_folder_name(parent);

  db_mutex.enterMutex();

  SQLQuery *q = db.query("Folders", ("SELECT id FROM %t WHERE filename='" + string_format::escape_db_string(good_parent) + "'").c_str());
  if (q && q->numberOfTuples() > 0) {
    assert(q->numberOfTuples() == 1);
    SQLRow &row = (*q)[0];
    string parent_id = row["id"];

    SQLQuery *q2;

    if (opts.dir_order() == "directories first")
      q2 = db.query("Folders", ("SELECT filename, upper(name) as key FROM %t WHERE parent='" + parent_id + "' ORDER BY is_folder desc, key").c_str());
    else
      q2 = db.query("Folders", ("SELECT filename, upper(name) as key FROM %t WHERE parent='" + parent_id + "' ORDER BY key").c_str());

    if (q2) {
      for (int i = 0; i < q2->numberOfTuples(); ++i) {
	SQLRow &row = (*q2)[i];
	names.push_back(row["filename"]);
      }
    }
    delete q2;
  }
  delete q;

  db_mutex.leaveMutex();

  return names;
}

vector<Picture> Pictures::rdir_hd(const string& argv, bool insert_into_db)
{
  libfs_set_folders_first(opts.dir_order());

  if (insert_into_db) {

    bool inserted = false;

    for (file_iterator<file_t, default_order> i (argv); i != i.end (); i.advance(false)) {
      string filename = filesystem::FExpand(i->getName());
      if (isDirectory(filename) || check_type(filename, conf->p_filetypes_p()) != emptyMyPair) {
	insert_file_into_db(filename, argv);
	inserted = true;
      }
    }

    if (inserted)
      return rdir(argv);
    else {
      vector<Picture> cur_files;
      return cur_files;
    }

  } else {

    vector<Picture> cur_files;

    for (file_iterator<file_t, default_order> i (argv); i != i.end (); i.advance(false)) {
      string filename = filesystem::FExpand(i->getName());
      if (!rdir_internal(filename, argv, cur_files, false))
	break;
    }

    return cur_files;
  }
}

// read a dir and spit out a list of files and dirs
vector<Picture> Pictures::rdir(const string& argv)
{
  vector<Picture> files;

  vector<string> db_files = check_db_for_folders(argv);

  if (db_files.size() > 0) {
    foreach (string& dir, db_files)
      if (!rdir_internal(dir, argv, files, true))
	break;
  } else {
    files = rdir_hd(argv, true);
  }

  return files;
}

// The über function for parsing dirs.
vector<Picture> Pictures::parse_dir(const list<string>& dirs)
{
  vector<Picture> files;

  foreach (const string& dir, dirs) {
    vector<Picture> tempfiles = rdir(dir);
    files.insert(files.end(), tempfiles.begin(), tempfiles.end());
  }

  return files;
}

// functions for creating lists for recursion

vector<Picture> Pictures::parse_dirs_recursion()
{
  vector<string> image_files = images_in_dir(folders.top().first);

  // we need them in the correct order for recursion
  sort(image_files.begin(), image_files.end());

  vector<Picture> files;

  foreach (string& image, image_files)
    {
      const MyPair filetype = check_type(image, conf->p_filetypes_p());
      if (filetype != emptyMyPair)
	files.push_back(get_file_from_path(image, filetype));
    }

  return files;
}

void Pictures::read_dirs()
{
  // process picture dir
  pic_list = parse_dir(picture_folders);

  if (picture_folders.size() >= 1) {
    std::list<Picture> t(pic_list.begin(), pic_list.end());
    t.sort(file_sort());
    pic_list = vector<Picture>(t.begin(), t.end());
  }

  if (pic_list.size() != 0)
    folders.top().second = 0;

  S_BackgroundUpdater::get_instance()->run_once(boost::bind(&Pictures::check_for_changes, this));
}

#ifdef use_inotify
// notify
void Pictures::fs_change(NotifyUpdate::notify_update_type type, const string& path)
{
  std::string dir = filesystem::FExpand(path);
  if (dir.empty())
    dir = "/";
  else if (!file_tools::is_directory(dir))
    dir = dir.substr(0, dir.rfind('/')+1);
  else
    dir +="/";

  if (type == NotifyUpdate::CREATE_DIR || type == NotifyUpdate::MOVE || type == NotifyUpdate::DELETE_DIR)
    reload_dir(dir);

  bool reparsed_current_dir = false;
  do{
    foreach (std::string& p, folders.top().first)
      if (dir == p) {
	reparse_current_dir();
	reparsed_current_dir = true;
	break;
      }
    if (reparsed_current_dir || dir == "/" || dir.size() < 2)
      break;

    dir = dir.substr(0, dir.rfind('/', dir.size()-2)+1); /* go 1 directory up */
  } while(true);

  if (type == NotifyUpdate::DELETE_DIR || type == NotifyUpdate::DELETE_FILE) {
    bool current_dir_ok = false;

    while (!current_dir_ok) {
      load_current_dirs();
      if (pic_list.size() == 0) {
	if (folders.size() == 1) {
	  exit();
	  input_master->add_input(Input(), ""); // exit mainloop
	  return;
	} else {
	  folders.pop();
	  reparsed_current_dir = true;
	}
      } else
	current_dir_ok = true;
    }
  }

  // sanity check
  if (folders.top().second > pic_list.size()-1)
    folders.top().second = pic_list.size()-1;

  if (!global->playback_in_fullscreen && !in_fullscreen && visible && reparsed_current_dir)
    print(pic_list);
}
#endif

void Pictures::reparse_current_dir()
{
  // process picture dir
  reload_current_dirs();

  // sanity check
  if (folders.top().second > pic_list.size()-1)
    folders.top().second = 0;
}

void Pictures::pictures_check()
{
  if (opts.slideshow() != "0" && !pause && last_press <= time(0) - conv::atoi(opts.slideshow())) {

    if (conv::stob(opts.slideshow_random()))
      next_random();
    else if (conv::stob(opts.recurse_in_fullscreen()))
      next_no_skip_folders();
    else
      next_skip_folders();

    orientation = get_db_orientation_lookup();

    prepare_and_show_fullscreen();
  }
}

int Pictures::check_picture_time()
{
  return 1000;
}
