// used for #ifdef
#include "config.h"

#include "libfspp.hpp"

#include "game.hpp"

//Input
#include "input.hpp"

#include "common.hpp"
#include "gettext.hpp"
#include "graphics.hpp"

#include "common-feature.hpp"
#include "startmenu.hpp"
#include "extra_menu.hpp"
#include "updater.hpp"
#include "busy_indicator.hpp"
#include "shutdown.hpp"
#include "search.hpp"
#include "touch.hpp"
#include "resolution.hpp"

#include "boost.hpp"

#ifdef use_inotify
#include "notify.hpp"
#endif

// ostringstream
#include <sstream>

#include <iostream>

#include <cassert>

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

using std::list;
using std::vector;
using std::string;

Game::GameOpts::GameOpts()
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-game", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-game", nl_langinfo(CODESET));
#endif

  // header
  header = "Game Options";
  translated_header = dgettext("mms-game", "Game Options");
  save_name = "GameOptions";

  // values

  reload_p = NULL;

  GameConfig *game_conf = S_GameConfig::get_instance();

#ifdef use_inotify
  if (game_conf->p_game_dirs_no_watch().size() > 0) {
#else
  if (game_conf->p_game_dirs_no_watch().size() > 0 || game_conf->p_game_dirs().size() > 0) {
#endif
    // reload
    vector<string> none;
    reload_p = new Option(false, dgettext("mms-game", "reload"), "reload", 0, none, none);
    val.push_back(reload_p);
  }

  // sorting order
  dir_order_p = go->dir_order();
  val.push_back(dir_order_p);
}

Game::GameOpts::~GameOpts()
{
  save();
  if (reload_p)
    delete reload_p;
}

Game::Game()
 : db((conf->p_var_data_dir() + "games.db").c_str()),
   exit_loop(false), reload_dirs(false)
{
  game_conf = S_GameConfig::get_instance();

  check_db();

  set_folders();

  check_db_consistency();

  reset();

  res_dependant_calc();

  S_ResolutionManagement::get_instance()->register_callback(boost::bind(&Game::res_dependant_calc, this));
}

void Game::res_dependant_calc()
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
  list_font = graphics::resolution_dependant_font_wrapper(16, conf);

  normal_font_height = graphics::calculate_font_height(normal_font, conf);
  list_font_height = graphics::calculate_font_height(list_font, conf);

  header_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
  header_box_size = static_cast<int>(header_size.second * 0.75);

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
}

Game::~Game()
{
  db_mutex.enterMutex();
  db.close();
  db_mutex.leaveMutex();
}

void Game::set_folders()
{
  list<string> game_dirs = game_conf->p_game_dirs(); // make compiler happy
  // make sure game folders are not malformed
  foreach (string& dir, game_dirs)
    if (dir[dir.size()-1] != '/') {
      game_folders.push_back(dir + '/');
    } else
      game_folders.push_back(dir);

#ifdef use_inotify
  S_Notify::get_instance()->register_plugin("game", game_folders,
					    boost::bind(&Game::fs_change, this, _1, _2));
#endif

  list<string> game_dirs_no_watch = game_conf->p_game_dirs_no_watch(); // make compiler happy
  // make sure game folders are not malformed
  foreach (string& dir, game_dirs_no_watch)
    if (dir[dir.size()-1] != '/') {
      game_folders.push_back(dir + '/');
    } else
      game_folders.push_back(dir);
}

void Game::startup_updater()
{
  S_Search::get_instance()->register_module(SearchModule(dgettext("mms-game", "Games"), boost::bind(&Game::gen_search_list, this, _1),
							 boost::bind(&Game::reenter, this, _1)));
}

void Game::create_db()
{
  vector<string> fields;
  fields.push_back("version INTEGER NOT NULL");
  db.createTable("Schema", fields);

  db.execute("INSERT INTO Schema VALUES('2')");

  fields.clear();
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("parent INTEGER");
  fields.push_back("filename TEXT");
  fields.push_back("name TEXT");
  fields.push_back("lname TEXT");
  fields.push_back("is_folder INTEGER"); // 1 = true
  db.createTable("Folders", fields);

  create_indexes();
}

void Game::create_indexes()
{
  db.execute("CREATE INDEX idx_folders_name ON Folders (lname)");
  db.execute("CREATE INDEX idx_folders_parent ON Folders (parent)");
  db.execute("CREATE INDEX idx_folders_filename ON Folders (filename)");
}

void Game::check_db()
{
  if (db.hasTable("Schema")) {
    // check for correct version
    SQLQuery *q = db.query("Schema", "SELECT version FROM %t");
    if (q && q->numberOfTuples() > 0) {
      SQLRow &row = (*q)[0];
      if (row["version"] != "2") {

	Print print(gettext("Upgrading game database schema"), Print::INFO);

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

void Game::check_db_consistency()
{
  db_mutex.enterMutex();

  if (!db.hasTable("Folders")) {
    db_mutex.leaveMutex();
    return;
  }

  //Need to call rdir for each folder listed in game_folders, so that the "parent" db row is updated. Probably there's a better way of doing it
  foreach (const string& dir, game_folders ) {
    vector<GameEntry> tempfiles = rdir(dir);
  }

  SQLQuery *q = db.query("Folders", "SELECT id, parent, filename FROM %t WHERE parent==0");

  if (q->numberOfTuples() >  game_folders.size()) { //if there's more "root" dirs in DB than in config file, db should be updated
    for (int i = 0; i < q->numberOfTuples(); ++i) {
      string filename = (*q)[i]["filename"].c_str();
      bool found = false;
      foreach (string& dir, game_folders) {
	if ( dir == filename )
	  found=true;
      }
      if (!found) {
	foreach (string& dir, game_folders)
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

void Game::reset()
{
  for (int i = 0, size = folders.size(); i < size; ++i)
    folders.pop();

  folders.push(std::make_pair(game_folders, 0));
}

bool Game::file_sort::operator()(const GameEntry& lhs, const GameEntry& rhs)
{
  return sort_order(lhs.lowercase_name, rhs.lowercase_name,
		    lhs.path.empty() ? lhs.filenames.front() : lhs.path,
		    rhs.path.empty() ? rhs.filenames.front() : rhs.path,
		    lhs.type != "file", rhs.type != "file",
		    get_class<Game>(dgettext("mms-game", "Game"))->get_opts()->dir_order());
}

void Game::find_element_and_do_action(const GameEntry& game)
{
  int p = 0;
  foreach (GameEntry& f, files) {
    if (f == game) {
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

void Game::action()
{
  if (vector_lookup(files, folders.top().second).type != "file")
    enter_dir();
  else
    action_play();
}

// commands
void Game::action_play()
{
  GameEntry e = vector_lookup(files, folders.top().second);
  int nfiles = e.filenames.size();
  if (nfiles > 0)
    playgame(e);
}

void Game::go_back()
{
  if (!(folders.size() == 1)) {
    folders.pop();
    load_current_dirs();
  }
}

void Game::one_up()
{
  if (search_mode)
    return;

  if (folders.size() > 1)
    go_back();
  else if (folders.size() == 1)
    exit();
}

void Game::enter_dir()
{
  if (vector_lookup(files, folders.top().second).type != "file") {
    string temp_cur_dir = vector_lookup(files ,folders.top().second).path;
    vector<GameEntry> templist = rdir(temp_cur_dir);

    if (templist.size() != 0) {
      list<string> templs; templs.push_back(temp_cur_dir + "/");
      folders.push(std::make_pair(templs, 0));
      files = templist;
      std::sort(files.begin(), files.end(), file_sort());

      S_BackgroundUpdater::get_instance()->run_once(boost::bind(&Game::check_for_changes, this));
    } else
      DialogWaitPrint pdialog(dgettext("mms-game", "Folder is empty"), 1000);
  }
}

#ifdef use_inotify
// notify
void Game::fs_change(NotifyUpdate::notify_update_type type, const string& path)
{
  string dir = path;
  if (dir[dir.size()-1] != '/')
    dir = dir.substr(0, dir.rfind('/')+1);
  
  if (type == NotifyUpdate::CREATE_DIR || type == NotifyUpdate::MOVE || type == NotifyUpdate::DELETE_DIR)
    reload_dir(dir);

  bool reparsed_current_dir = false;

  foreach (string& p, folders.top().first)
    if (dir == p) {
      reparse_current_dir();
      reparsed_current_dir = true;
      break;
    }

  if (type == NotifyUpdate::DELETE_DIR || type == NotifyUpdate::DELETE_FILE) {
    bool current_dir_ok = false;

    while (!current_dir_ok) {
      load_current_dirs();
      if (files.size() == 0) {
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
  if (folders.top().second > files.size()-1)
    folders.top().second = files.size()-1;

  if (!global->playback_in_fullscreen && visible && reparsed_current_dir)
    graphical_print(files);
}
#endif

void Game::reparse_current_dir()
{
  reload_current_dirs();

  // sanity check
  if (folders.top().second > files.size()-1)
    folders.top().second = 0;
}

void Game::load_current_dirs()
{
  files = parse_dir(folders.top().first);

  if (folders.top().first.size() >= 1 && folders.size() == 1)
    std::sort(files.begin(), files.end(), file_sort());
}

bool Game::reload_dir(const string& path)
{
  vector<GameEntry> hdfiles = rdir_hd(path, false);

  vector<GameEntry> dbfiles = rdir(path);

  // find what is new and what is removed
  vector<string> new_list;
  foreach (GameEntry& entry, hdfiles)
    new_list.push_back(entry.path);

  vector<string> old_list;
  foreach (GameEntry& entry, dbfiles)
    old_list.push_back(entry.path);

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

    db_mutex.leaveMutex();
  }

  if (removed_files.size() > 0 || new_files.size() > 0)
    return true;
  else
    return false;
}

void Game::check_for_changes()
{
  bool reload = false;

  list<string> top_folders = folders.top().first;

  foreach (string& dir, top_folders)
    if (reload_dir(dir))
      reload = true; // no break since we want to update the db for all folders

  reload_dirs = reload;
}

void Game::reload_current_dirs()
{
  check_for_changes();
  load_current_dirs();

  if (files.size() == 0)
    go_back();
}

void Game::update_db_current_dirs()
{
  foreach (string& dir, folders.top().first)
    reload_dir(dir);
}

void Game::options()
{
  if (opts.values().size() == 0)
    DialogWaitPrint pdialog(dgettext("mms-game", "No options available for games"), 2000);
  else {
    if (opts.mainloop()) {

      S_BusyIndicator::get_instance()->busy();

      update_db_current_dirs();

      reload_current_dirs();

      if (folders.top().second > files.size()-1)
	folders.top().second = 0;

      S_BusyIndicator::get_instance()->idle();
    }
  }
}

void Game::reenter(int db_id)
{
  if (change_dir_to_id(db_id))
    mainloop();
}

int Game::find_position_in_folder_list(const list<string>& files,
				       const string& filename)
{
  int pos = 0;
  bool pos_found = false;
  foreach (const string& file, files) {
    vector<GameEntry> templist = rdir(file);
    std::sort(templist.begin(), templist.end(), file_sort());
    pos = 0;
    foreach (GameEntry& entry, templist) {
      if (filename == entry.path ||
	  (entry.filenames.size() > 0 && filename == entry.filenames.front())) {
	pos_found = true;
	break;
      } else
	++pos;
    }
    if (pos_found)
      break;
  }
  return pos;
}

vector<std::pair<string, int> > Game::gen_search_list(const string& search_word)
{
  string parent = "";

  if (input_master->current_saved_map() != "search" && search_depth == dgettext("mms-game", "current folder")) {
    string parent_sql_str = "SELECT id FROM %t WHERE";

    list<string> current_folders = folders.top().first;
    bool first = true;
    foreach (string& folder, current_folders) {
      if (first) {
	parent_sql_str += " filename='" + folder + "'";
	first = false;
      } else
	parent_sql_str += " OR filename='" + folder + "'";
    }

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

  SQLQuery *q = db.query("Folders", ("SELECT id, parent, filename, name, is_folder FROM %t WHERE " + parent  + " fuzzycmp('" + search_word + "', lname, 1)").c_str());

  if (q) {
    for (int i = 0; i < q->numberOfTuples(); ++i) {

      SQLRow &row = (*q)[i];

      bool result_ok = false;

      if (input_master->current_saved_map() != "search" && search_depth == dgettext("mms-game", "current folder")) {
	list<string> current_folders = folders.top().first;
	foreach (string& folder, current_folders) {
	  if (row["filename"].find(folder) != string::npos) {
	    result_ok = true;
	    break;
	  }
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

bool Game::change_dir_to_id(int db_id)
{
  reset();

  db_mutex.enterMutex();

  SQLQuery *q = db.query("Folders", ("SELECT * FROM %t WHERE id='" + conv::itos(db_id) + "'").c_str());

  if (q && q->numberOfTuples() > 0) {

    string filename = (*q)[0]["filename"];
    string cur_dir_path;

    delete q;
    db_mutex.leaveMutex();

    if (filename[filename.size() - 1] == '/')
      filename = filename.substr(0, filename.size() - 1);

    string original_filename = filename;

    int pos;
    foreach (string& dir, game_folders)
      if ((pos = filename.rfind(dir)) != string::npos) {
	cur_dir_path = filename.substr(0, dir.size()+1);
	filename = filename.substr(dir.size()+1);
      }

    if (!filename.empty()) {
      while ((pos = filename.find("/")) != string::npos) {
	folders.top().second = find_position_in_folder_list(folders.top().first, cur_dir_path + filename.substr(0, pos));
	list<string> templs; templs.push_back(cur_dir_path + filename.substr(0, pos));
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

string Game::id_to_filename(int db_id)
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

bool Game::search_is_graphical_view(const std::vector<GameEntry>& cur_files)
{
  return true;
}

void Game::search_func()
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

  screen_updater->trigger.add(TriggerElement("search marker", empty,  boost::bind(&Game::print_marked_search_letter, this),
					     boost::bind(&Game::clean_up_search_print, this)));
  screen_updater->timer.add(TimeElement("search marker", boost::bind(&Game::check_search_letter, this),
					boost::bind(&Game::print_marked_search_letter, this)));

  int pos = input_master->graphical_search<GameEntry>(files,
						      boost::bind(&Game::graphical_print, this, _1),
						      boost::bind(&Game::gen_search_list, this, _1),
						      boost::bind(&Game::id_to_filename, this, _1),
						      boost::bind(&Game::search_is_graphical_view, this, _1),
						      images_per_row, search_mode, search_str, lowercase_search_str,
						      search_depth, offset, search_help_offset,
						      S_BusyIndicator::get_instance());

  screen_updater->trigger.del("search marker");
  screen_updater->timer.del("search marker");

  force_search_print_cleanup();

  if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
    search_top_size -= search_size;

  if (pos != -1 && search_depth == dgettext("mms-game", "current folder"))
    folders.top().second = pos;
  else if (pos != -1)
    change_dir_to_id(pos);
}

void Game::exit()
{
  exit_loop = true;
}

string Game::mainloop()
{
  visible = true;

  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();
  Shutdown *sd = S_Shutdown::get_instance();

  search_top_size = 30;

  Input input;

  bool update_needed = true;

  render->device->animation_section_begin(true);
  int curlayer = render->device->get_current_layer();
  render->device->switch_to_layer(curlayer + 1);
  render->device->reset_layout_attribs_nowait();
  render->device->set_layout_alpha(0.0, curlayer + 1);
  render->device->animation_zoom(0,0,1,1,20, curlayer + 1);
  render->device->animation_spin(360,0,40, curlayer + 1);
  render->device->animation_fade(1,0,80, curlayer);
  render->device->animation_fade(0,1,40, curlayer + 1);
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
	graphical_print(files);

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
	  if (files.size() > images_per_row) {
	    for (int i = 0; i < images_per_row; ++i) {
	      if (folders.top().second == 0) {
		folders.top().second = files.size()-1;
		i += images_per_row-files.size()%images_per_row;
	      }
	      else
		folders.top().second -= 1;
	    }
	  }
 	}
      else if (input.command == "next")
 	{
	  if (files.size() > images_per_row) {
	    for (int i = 0; i < images_per_row; ++i) {
	      if (folders.top().second == files.size()-1 && i == 0) {
		folders.top().second = 0;
		break;
	      } else if (folders.top().second == files.size()-1) {
		break;
	      } else {
		folders.top().second += 1;
	      }
	    }
	  }
 	}
      else if (input.command == "left")
	{
	  if (folders.top().second != 0)
	    folders.top().second -= 1;
	  else
	    folders.top().second = files.size() - 1;
	}
      else if (input.command == "right")
 	{
	  folders.top().second = (folders.top().second+1)%files.size();
 	}
      else if (input.command == "back")
	{
	  one_up();
	}
      else if (input.command == "play_game")
	{
	  action_play();
	}
      else if (input.command == "action")
	{
	  action();
	}
      else if (input.command == "second_action")
	{
	  ExtraMenu em;

	  if (vector_lookup(files, folders.top().second).type != "file")
	    em.add_item(ExtraMenuItem(dgettext("mms-game", "Enter directory"), input_master->find_shortcut("action"),
				      boost::bind(&Game::enter_dir, this)));
	  else
	    em.add_item(ExtraMenuItem(dgettext("mms-game", "Play game"), input_master->find_shortcut("action"),
				      boost::bind(&Game::action_play, this)));

	  em.add_item(ExtraMenuItem(dgettext("mms-game", "Search"), input_master->find_shortcut("search"),
				    boost::bind(&Game::search_func, this)));
	  em.add_item(ExtraMenuItem(dgettext("mms-game", "Options"), input_master->find_shortcut("options"),
				    boost::bind(&Game::options, this)));

	  if (folders.size() > 1) {
	    em.add_item(ExtraMenuItem(dgettext("mms-game", "Go up one directory"), input_master->find_shortcut("back"),
				      boost::bind(&Game::go_back, this)));
	    em.add_item(ExtraMenuItem(dgettext("mms-game", "Return to startmenu"), input_master->find_shortcut("startmenu"),
				      boost::bind(&Game::exit, this)));
	  } else
	    em.add_item(ExtraMenuItem(dgettext("mms-game", "Return to startmenu"), input_master->find_shortcut("back"),
				      boost::bind(&Game::exit, this)));

	  foreach (ExtraMenuItem& item, global->menu_items)
	    em.add_persistent_item(item);

	  em.mainloop();
	}
      else if (input.command == "page_up")
 	{
	  int jump = conf->p_jump();

	  if (search_mode)
	    jump = images_per_row*rows_search;
	  else
	    jump = images_per_row*rows;

	  if (files.size() > jump) {
	    int diff = folders.top().second-jump;
	    if (folders.top().second == 0)
	      folders.top().second = (files.size()-1)+diff;
	    else if (diff < 0)
	      folders.top().second = 0;
	    else
	      folders.top().second = diff;
	  }
 	}
      else if (input.command == "page_down")
 	{
	  int jump = conf->p_jump();

	  if (search_mode)
	    jump = images_per_row*rows_search;
	  else
	    jump = images_per_row*rows;

	  if (files.size() > jump) {
	    if (folders.top().second > (files.size() - jump) && folders.top().second != files.size()-1)
	      folders.top().second = files.size()-1;
	    else
	      folders.top().second = (folders.top().second+jump)%files.size();
	  }
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

      update_needed = !global->check_commands(input);
    }

  exit_loop = false;

  visible = false;

  render->device->animation_section_begin();
  render->device->layer_active_no_wait(false);
  render->device->switch_to_layer(curlayer);
  render->device->animation_fade(0,1,80,curlayer);
  render->device->animation_section_end();

  return "";
}

string Game::get_name_from_file(const GameEntry& e)
{
  return e.name;
}

bool Game::search_compare(const GameEntry& s)
{
  return (s.lowercase_name.substr(0, search_str.size()) == lowercase_search_str);
}

// print a game element
void Game::print_game_element(const GameEntry& r, const GameEntry& position, int y)
{
  string name = r.name;

  if (r.type != "file")
    name += "/";

  string_format::format_to_size(name, list_font, conf->p_h_res()-75-155, true);

  if (r == position)
    render->current.add(new PObj(themes->general_marked_large, 70, y,
				 conf->p_h_res()-(2*70), list_font_height, 2, NOSCALING));

  std::ostringstream out; out << r.filenames.size();

  render->current.add(new TObj(name, list_font, 75, y,
			       themes->game_font1, themes->game_font2,
			       themes->game_font3, 3));
  render->current.add(new TObj(r.filetype, list_font, conf->p_h_res()-145, y,
			       themes->game_font1, themes->game_font2,
			       themes->game_font3, 3));
  render->current.add(new TObj(out.str(), list_font, conf->p_h_res()-85, y,
			       themes->game_font1, themes->game_font2,
			       themes->game_font3, 3));
}

string Game::get_cover(const std::string& s)
{
  string path = "";
  string test = "";
  string::size_type pos;

  if ((pos = s.rfind(".")) != string::npos) {
    path = s.substr(0, pos);
    test = (path + ".png").c_str();
    if (file_exists(test))
      return test;
    test = (path + ".jpg").c_str();
    if (file_exists(test))
      return test;
  }
  return "";
}

void Game::graphical_print(const vector<GameEntry>& cur_files)
{
  render->prepare_new_image();

  render->current.add(new PObj(themes->game_background, 0, 0, 0, SCALE_FULL));

  PObj *back = new PFObj(themes->startmenu_game_dir, 25, 10, header_box_size, header_box_size, 2, true);

  render->current.add(back);

  S_Touch::get_instance()->register_area(TouchArea(rect(back->x, back->y, back->w, back->h), back->layer, boost::bind(&Game::one_up, this)));

  if (themes->show_header) {
    string header = dgettext("mms-game", "Games");

    if (folders.size() > 1) {
      string top_folder = folders.top().first.front();

      if (top_folder[top_folder.size()-1] == '/')
	top_folder = top_folder.substr(0, top_folder.size()-1);

      assert(top_folder.rfind('/') != string::npos);
      header += " - " + top_folder.substr(top_folder.rfind('/')+1);
      string_format::format_to_size(header, header_font, conf->p_h_res()-220, false);
    }

    TObj *t = new TObj(header, header_font, 25 + header_box_size + 10, 10 + (header_box_size - header_size.second)/2,
		       themes->game_header_font1, themes->game_header_font2,
		       themes->game_header_font3, 2);

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

    string s = dgettext("mms-game", "Search: ");
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

  int nr_in_last_row = cur_files.size()%images_per_row;
  if (nr_in_last_row == 0)
    nr_in_last_row = images_per_row;

  if (cur_files.size() > cur_rows*images_per_row) {
    if (pos_in_list < std::ceil(double(cur_rows)/2.0)*images_per_row) {
      pos = pos_in_list;
      start = 0;
    } else if (pos_in_list >= cur_files.size() - (images_per_row+nr_in_last_row)) { // last two rows
      start = cur_files.size()-((cur_rows-1)*images_per_row+nr_in_last_row);
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

  for (int j = 0; j < cur_rows*images_per_row && j < cur_files.size() && start < cur_files.size(); ++j) {

    GameEntry s = vector_lookup(cur_files, start);
    string name = s.name;

    int y_pos_offset = 0;

    if (s.type != "file") {

      int pic_width = static_cast<int>(image_width/1.15);
      int pic_height = static_cast<int>(image_height/1.15);
      int pic_width_all = image_width_all;
      int pic_height_all_eks_text = image_height_all_eks_text;

      int max_pic_width = static_cast<int>(pic_width * 1.4);
      int max_pic_height = static_cast<int>(pic_height * 1.4);

      string picture_path = render->default_path + themes->general_normal_dir;

      if (pos == j) {
	pic_width = max_pic_width;
	pic_height = max_pic_width;
	pic_width_all += pic_width - image_width;
	pic_height_all_eks_text += pic_height - image_height;
	x -= (pic_width - image_width)/2;
	y_pos_offset = (pic_height - image_height)/2;
	y -= y_pos_offset;
	picture_path = render->default_path + themes->game_dir_marked;
      }

      render->create_scaled_image_wrapper(picture_path, max_pic_width, max_pic_height);
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

      S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&Game::find_element_and_do_action, this, s)));

    } else {
      string cover_name = "";
      list<string>::const_iterator iter = (s.filenames).begin();
      cover_name = get_cover(*iter);

      int pic_width = static_cast<int>(image_width/1.15);
      int pic_height = static_cast<int>(image_height/1.15);
      int pic_width_all = image_width_all;
      int pic_height_all_eks_text = image_height_all_eks_text;

      int max_pic_width = static_cast<int>(pic_width * 1.40);
      int max_pic_height = static_cast<int>(pic_height * 1.40);

      if (pos == j) {
	pic_width = max_pic_width;
	pic_height = max_pic_width;
	pic_width_all += pic_width - image_width;
	pic_height_all_eks_text += pic_height - image_height;
	x -= (pic_width - image_width)/2;
	y_pos_offset = (pic_height - image_height)/2;
	y -= y_pos_offset;
      }

      if (!file_exists(cover_name)) {

	string picture_path = render->default_path + themes->game_file;

	render->create_scaled_image_wrapper(picture_path, max_pic_width, max_pic_height);
	// use pic with max_width and max_height and scale it in software
	PFObj *p = new PFObj(picture_path, x, y, max_pic_width, max_pic_height, pic_width, pic_height, 4);

	p->x = p->real_x = x + (pic_width_all - p->real_w)/2;
	p->y = p->real_y = y + (pic_height_all_eks_text - p->real_h)/2;

	p->real_x -= 1;
	p->real_y -= 1;

	if (pos == j) {
	  x += (pic_width - image_width)/2;
	  y += y_pos_offset * 2;
	}

	render->current.add(p);

	S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&Game::find_element_and_do_action, this, s)));

      } else {

	render->create_scaled_image_wrapper_upscaled(cover_name, max_pic_width, max_pic_height);
	// use pic with max_width and max_height and scale it in software
	PFObj *p = new PFObj(cover_name, x, y, max_pic_width, max_pic_height, pic_width, pic_height, 3);

	p->x = p->real_x = x + (pic_width_all - p->real_w)/2;
	p->y = p->real_y = y + (pic_height_all_eks_text - p->real_h)/2;

	p->real_x -= 1;
	p->real_y -= 1;

	if (pos == j) {
	  x += (pic_width - image_width)/2;
	  y += y_pos_offset * 2;
	}

	render->current.add(p);

	S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&Game::find_element_and_do_action, this, s)));
      }
    }

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
				   themes->game_marked_font1, themes->game_marked_font2,
				   themes->game_marked_font3, 3));
      render->current.add(new TObj(name_second_part, normal_font, x + (image_width_all-x_size2)/2,
				   y+image_height_all_eks_text+normal_font_height,
				   themes->game_marked_font1, themes->game_marked_font2,
				   themes->game_marked_font3, 3));
      y -= y_pos_offset;
    } else {
      render->current.add(new TObj(name, normal_font, x + (image_width_all-x_size)/2, y+image_height_all_eks_text,
				   themes->game_font1, themes->game_font2,
				   themes->game_font3, 3));
      render->current.add(new TObj(name_second_part, normal_font, x + (image_width_all-x_size2)/2,
				   y+image_height_all_eks_text+normal_font_height,
				   themes->game_font1, themes->game_font2,
				   themes->game_font3, 3));
    }

    x += image_width_all;

    if (start == cur_files.size()-1) {
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
    if (cur_files.size() > 0)
      position << pos_in_list + 1 << "/" << cur_files.size();
    else
      position << "";
  else
    position << folders.top().second+1 << "/" << cur_files.size();

  int x_size = string_format::calculate_string_width(position.str(), position_font);

  render->current.add(new TObj(position.str(), position_font, conf->p_h_res()-(60+x_size), 20,
			       themes->game_font1, themes->game_font2,
			       themes->game_font3, 3));

  render->draw("game-graphical");

  render->image_mut.leaveMutex();
}

void Game::playgame(const GameEntry file)
{
  DialogWaitPrint pdialog(dgettext("mms-game", "Starting game..."), 1000);

  string cur_files = "";

  foreach (const string& filename, file.filenames)
    cur_files += " \"" + filename + "\"";

  string cur_dir = file.filenames.front().substr(0,file.filenames.front().rfind("/")+1);

  // support playlist files
  if (file.filetype == "mame" || file_exists(cur_dir + "mame_roms")) {
    run::exclusive_external_program(game_conf->p_mame_path() + " " + game_conf->p_mame_opts() + ' ' + cur_files);
  } else if (file.filetype == "snes" || file_exists(cur_dir + "snes_roms")) {
    run::exclusive_external_program(game_conf->p_snes_path() + " " + game_conf->p_snes_opts() + ' ' + cur_files);
  } else if (file.filetype == "nes" || file_exists(cur_dir + "nes_roms")) {
    run::exclusive_external_program(game_conf->p_nes_path() + " " + game_conf->p_nes_opts() + ' ' + cur_files);
  } else if (file.filetype == "gens" || file_exists(cur_dir + "gens_roms")) {
    run::exclusive_external_program(game_conf->p_gens_path() + " " + game_conf->p_gens_opts() + ' ' + cur_files);
  } else if (file.filetype == "sh" || file_exists(cur_dir + "sh_scripts")) {
    run::exclusive_external_program(cur_files);
  }
}

// play everything on a cd
void Game::showcd(int count)
{
  vector<GameEntry> backup_files;

  backup_files = files;

  std::stack<std::pair<list<string>, int > > backup_folders = folders;

  list<string> dirs;
  dirs.push_back(cd->get_mount_point());

  files = parse_dir(dirs);

  while (!folders.empty())
    folders.pop();

  folders.push(std::make_pair(dirs, 0));

  mainloop();

  files = backup_files;
  folders = backup_folders;

  //umount the disc
  run::external_program("umount '" + cd->get_mount_point() + "'");
}

void Game::playpsx()
{
  string device = cd->get_device();

  //play disc
  run::exclusive_external_program((game_conf->p_psxemu_path() + ' ' + game_conf->p_psxemu_opts()).c_str());
}


// ====================================================================
//
// dir
//
// ====================================================================

// Add a file to the list
GameEntry Game::addfile(const string& name, const MyPair& filetype)
{
  GameEntry r;
  r.id = ++id;
  r.filenames.push_back(name);
  r.path = name;

  string tempname = name.substr(0, name.size()-((filetype.first).size()+1));
  string::size_type pos = tempname.rfind("/");
  if (pos != string::npos)
    r.name = tempname.substr(pos+1);
  else
    r.name = tempname;
  if (conf->p_convert())
    r.name = string_format::convert(r.name);
  r.lowercase_name = string_format::lowercase(r.name);
  r.type = "file";
  r.filetype = filetype.second;

  return r;
}

// Add a game dir as "one file" to the game list
GameEntry Game::add_dir(const string& f)
{
  GameEntry output;

  string filename = (f[f.size()-1] == '/') ? f.substr(0, f.size()-1) : f;

  string type = "";

  vector<string> known_filetypes;

  libfs_set_folders_first(opts.dir_order());

  for (file_iterator<file_t, default_order> i (filename); i != i.end (); i.advance(false))
    {
      string name = i->getName();

      if (check_type(name, game_conf->p_filetypes_g()) != emptyMyPair) {
	known_filetypes.push_back(name);
	type = "rom";
      }
    }

  if (!known_filetypes.empty()) {
    output.id = ++id;
    string::size_type pos = filename.rfind("/");
    output.name = filename.substr(pos+1);
    output.path = filename;

    output.type = "dir-single";
    if (known_filetypes.size() > 1) {
      string first = known_filetypes.front();
      vector<string>::iterator i = known_filetypes.begin();
      ++i;
      for (vector<string>::iterator end = known_filetypes.end();
	   i != end; ++i)
	if (string_format::difference(first, *i) > 1) {
	  output.type = "dir-multi";
	  break;
	}
    }

    if (conf->p_convert())
      output.name = string_format::convert(output.name);

    const MyPair filetype = check_type(known_filetypes.front(), game_conf->p_filetypes_g());
    output.filetype = filetype.second;
    foreach (string& filetype, known_filetypes)
      output.filenames.push_back(filetype);

  } else {

    output.id = ++id;
    string::size_type pos = filename.rfind("/");
    output.name = filename.substr(pos+1);
    output.type = "dir";
    output.path = filename;
    output.filetype = "dir";

  }

  output.lowercase_name = string_format::lowercase(output.name);

  return output;
}

void Game::insert_file_into_db(const string& filename, const string& parent)
{
  bool is_dir = isDirectory(filename);

  string good_filename;
  string cut_filename;

  if (is_dir) {
    good_filename = string_format::unique_folder_name(filename);
    cut_filename = good_filename.substr(0, good_filename.size()-1);
  } else {
    good_filename = filename;
    cut_filename = filename;
  }

  string::size_type i;
  if ((i = cut_filename.rfind('/')) == string::npos)
    i = 0;
  string name = cut_filename.substr((i != 0) ? i+1 : i);

  if (!is_dir && (i = name.rfind('.')) != string::npos)
    name = name.substr(0, i);

  if (name == "neogeo") // Ignoring rom not playable, only necesary for others roms
    return;

  db_mutex.enterMutex();

  // check if already exists
  SQLQuery *q = db.query("Folders", ("SELECT id FROM %t WHERE filename='" + string_format::escape_db_string(good_filename) + "'").c_str());
  if (q && q->numberOfTuples() > 0) {
    delete q;
    db_mutex.leaveMutex();
    return;
  } else
    delete q;

  string parent_id = get_parent_id(parent, db, game_folders);

  char *tmp_insert = sqlite3_mprintf("INSERT INTO Folders VALUES(NULL, '%q', '%q', '%q', '%q', '%q')",
				     parent_id.c_str(), good_filename.c_str(), name.c_str(),
				     string_format::lowercase(name).c_str(), conv::itos(is_dir).c_str());
  db.execute(tmp_insert);
  sqlite3_free(tmp_insert);
  db_mutex.leaveMutex();
}

int Game::db_id(const string& file, bool is_dir)
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

vector<string> Game::check_db_for_folders(const string& parent)
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

bool Game::rdir_internal(const string& filename, const string& argv,
			 vector<GameEntry> &cur_files, bool file_in_db)
{
  if (global->check_stop_bit()) {
    cur_files.clear();
    return false;
  }

  if (isDirectory (filename)) { // dir
    GameEntry dir = add_dir(filename);
    if ((dir.filenames).size() != 0 || dir.filetype == "dir") {
      if (file_in_db)
	dir.db_id = db_id(dir.path, true);
      cur_files.push_back(dir);
    }
  } else { // file
    MyPair filetype = check_type(filename, game_conf->p_filetypes_g());
    if (filetype != emptyMyPair) {
      GameEntry file(addfile(filename, filetype));
      if (file_in_db)
	file.db_id = db_id(filename, false);
      cur_files.push_back(file);
    }
  }

  return true;
}

vector<GameEntry> Game::rdir_hd(const string& argv, bool insert_into_db)
{
  libfs_set_folders_first(opts.dir_order());

  if (insert_into_db) {

    bool inserted = false;

    for (file_iterator<file_t, default_order> i (argv); i != i.end (); i.advance(false)) {
      string filename = filesystem::FExpand(i->getName());
      if (isDirectory(filename) || check_type(filename, game_conf->p_filetypes_g()) != emptyMyPair) {
	insert_file_into_db(filename, argv);
	inserted = true;
      }
    }

    if (inserted)
      return rdir(argv);
    else {
      vector<GameEntry> cur_files;
      return cur_files;
    }

  } else {

    vector<GameEntry> cur_files;

    for (file_iterator<file_t, default_order> i (argv); i != i.end (); i.advance(false)) {
      string filename = filesystem::FExpand(i->getName());
      if (!rdir_internal(filename, argv, cur_files, false))
	break;
    }

    return cur_files;
  }
}

// read a dir and spit out a list of files and dirs. For games.
vector<GameEntry> Game::rdir(const string& argv)
{
  vector<GameEntry> cur_files;

  vector<string> db_files = check_db_for_folders(argv);

  if (db_files.size() > 0) {
    foreach (string& file, db_files)
      if (!rdir_internal(file, argv, cur_files, true))
	break;
  } else {
    cur_files = rdir_hd(argv, true);
  }

  return cur_files;
}

vector<GameEntry> Game::parse_dir(const list<string>& dirs)
{
  vector<GameEntry> vec_files;

  foreach (const string& dir, dirs) {
    vector<GameEntry> tempfiles = rdir(dir);

    vec_files.insert(vec_files.end(), tempfiles.begin(), tempfiles.end());
  }

  return vec_files;
}

void Game::read_dirs()
{
  // process game dir
  files = parse_dir(game_folders);

  if (game_folders.size() >= 1)
    std::sort(files.begin(), files.end(), file_sort());

  S_BackgroundUpdater::get_instance()->run_once(boost::bind(&Game::check_for_changes, this));
}

void Game::determine_media()
{
 bool data_disc = cd->data_disc();
  cd->close();

  if (data_disc) {
    //mount the disc
    run::external_program("mount '" + cd->get_mount_point() + "'");

    int count;
    string mediatype = testdir(&count);

    if (mediatype == "empty" || (mediatype == "psx" && !game_conf->p_psxemu_enable()))
      Print pdialog(dgettext("mms-game", "The disc does not contain any recognizable files"),
		    Print::SCREEN);
    else if (count < 2)
      DialogWaitPrint pdialog(dgettext("mms-game", "Starting game..."), 1000);

    if (mediatype != "roms")
      //umount the disc
      run::external_program("umount '" + cd->get_mount_point() + "'");

    if (mediatype == "roms")
      showcd(count);
    else if (mediatype == "psx" && game_conf->p_psxemu_enable())
      playpsx();
  } else {
    Print pdialog(dgettext("mms-game", "The disc does not contain any recognizable files"),
		  Print::SCREEN);
  }
}

// help function for determine_media
string Game::testdir(int *count)
{
  *count = 0;

  libfs_set_folders_first(opts.dir_order());

  for (file_iterator<file_t, default_order> i (cd->get_mount_point()); i != i.end (); ++i)
    {
      string filename = filesystem::FExpand(i->getName());
      string cur_dir = string_format::lowercase(filename.substr(filename.rfind("/")+1));

      if (isDirectory (filename)) {
		//++(*count);
      } else if (cur_dir == "system.cnf") {
      	return "psx";
      } else {
	MyPair filetype = check_type(filename, game_conf->p_filetypes_g());
	if (filetype != emptyMyPair)
	  ++(*count);
      }
    }

  if (*count == 0)
    return "empty";
  else
    // none of the above found, it must be a divx
    return "roms";
}
