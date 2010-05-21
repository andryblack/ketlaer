//#define DLOG

#include "graphical_movie.hpp"

#include "libfspp.hpp"

#include "search.hpp"
#include "busy_indicator.hpp"
#include "shutdown.hpp"
#include "touch.hpp"

#include "Movie_info.h"

#include "boost.hpp"
// ostringstream
#include <sstream>
#include <fstream>

#include <assert.h>

using std::string;
using std::vector;
using std::list;

GraphicalMovie::GraphicalMovie()
  : MovieDB("moviehd.db", true), reload_dirs(false)
{
  if (!recurse_mkdir(conf->p_var_data_dir(),"movies", &imdb_dir))
    print_critical(dgettext("mms-movie", "Could not create directory ") + imdb_dir, "MOVIE");

  imdb_message_not_displayed = true;
  ext_mask = file_tools::create_ext_mask(movie_conf->p_filetypes_m());
  visible = false;
}

void GraphicalMovie::startup_updater()
{
#ifndef use_ffmpeg_thumbnailer
  Plugins *plugins = S_Plugins::get_instance();
  thumbnailer = plugins->find(plugins->movie_players, "MPlayer");

  if (thumbnailer == 0)
    std::cerr << "Warning, thumbnailer not found, please install mplayer plugin" << std::endl;
#endif

  S_Search::get_instance()->register_module(SearchModule(dgettext("mms-movie", "Videos"),
							 boost::bind(&GraphicalMovie::gen_search_list, this, _1),
							 boost::bind(&GraphicalMovie::reenter, this, _1)));
  Movie::startup_updater();
}

int GraphicalMovie::find_position_in_folder_list(const list<string>& files,
						 const string& filename)
{
  int pos = 0;
  bool pos_found = false;
  foreach (const string& file, files) {
    vector<CIMDBMovie> templist = rdir(file);
    std::sort(templist.begin(), templist.end(), file_sort());
    pos = 0;
    foreach (CIMDBMovie& movie, templist) {
      if ((filename[filename.size()-1] == '/' && filename == movie.path) ||
	  filename == movie.filenames.front()) {
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

void GraphicalMovie::action()
{
  if (folders.top().second >= files.size()) /* sanity check */
    return;                                 /* TODO: is there a race condition here??? */
  else if (vector_lookup(files, folders.top().second).type != "file")
    enter_dir();
  else
    action_play();
}

string GraphicalMovie::path_of_imdb_movie(const CIMDBMovie& c)
{
  if (!c.path.empty())
    return c.path;
  else
    return c.filenames.front();
}

bool GraphicalMovie::reload_dir(const string& path)
{
  bool db_load_succes = false;

  vector<CIMDBMovie> hdfiles = rdir_hd(path, db_load_succes, false);

  vector<CIMDBMovie> dbfiles = rdir(path);

  // find what is new and what is removed
  vector<string> new_list;
  foreach (CIMDBMovie& movie, hdfiles)
    new_list.push_back(path_of_imdb_movie(movie));

  vector<string> old_list;
  foreach (CIMDBMovie& movie, dbfiles)
    old_list.push_back(path_of_imdb_movie(movie));

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

  db_mutex.enterMutex();

  // remove old from db
  foreach (string& path, removed_files) {

    char *tmp = sqlite3_mprintf("DELETE FROM Folders WHERE filename LIKE '%q%%'", path.c_str());
    db.execute(tmp);
    sqlite3_free(tmp);

    tmp = sqlite3_mprintf("DELETE FROM HDMovie WHERE path LIKE '%q%%'", path.c_str());
    db.execute(tmp);
    sqlite3_free(tmp);

  }

  db_mutex.leaveMutex();

  return (removed_files.size() > 0 || new_files.size() > 0);
}

void GraphicalMovie::reload_current_dirs()
{
  check_for_changes();
  load_current_dirs();

  if (files.size() == 0)
    one_up();
}

bool GraphicalMovie::change_dir_to_id(int db_id)
{
  reset();

  db_mutex.enterMutex();

  SQLQuery *q = db.query("Folders", ("SELECT * FROM %t WHERE id='" + conv::itos(db_id) + "'").c_str());

  if (q && q->numberOfTuples() > 0) {

    string filename = (*q)[0]["filename"];
    string original_filename = (*q)[0]["filename"];

    delete q;
    db_mutex.leaveMutex();

    string cur_dir_path;

    if (filename[filename.size() - 1] == '/')
      filename = filename.substr(0, filename.size() - 1);

    list<string> dirs = movie_conf->p_movie_dirs();

    int pos;
    foreach (string& dir, dirs)
      if ((pos = filename.rfind(dir)) != string::npos) {
	cur_dir_path = filename.substr(0, dir.size()+1);
	filename = filename.substr(dir.size()+1);
      }

    if (!filename.empty()) {
      while ((pos = filename.find("/")) != string::npos) {
	folders.top().second = find_position_in_folder_list(folders.top().first, cur_dir_path + filename.substr(0, pos + 1));

	list<string> templs; templs.push_back(cur_dir_path + filename.substr(0, pos + 1));
	folders.push(std::make_pair(templs, 0));
	cur_dir_path += filename.substr(0, pos+1);
	filename = filename.substr(pos+1);
      }
    }

    // we should now be in the correct folder, find the correct file
    folders.top().second = find_position_in_folder_list(folders.top().first, original_filename);

    reload_current_dirs();

    S_BackgroundUpdater::get_instance()->run_once(boost::bind(&GraphicalMovie::check_for_changes, this));

    return true;
  } else {
    delete q;
    db_mutex.leaveMutex();
    return false;
  }
}

void GraphicalMovie::reenter(int db_id)
{
  if (change_dir_to_id(db_id))
    mainloop();
}

std::vector<std::pair<std::string, int> > GraphicalMovie::gen_search_list(const std::string& search_word)
{
  string parent = "", movie_parent = "";

  if (input_master->current_saved_map() == "movie" && search_depth == dgettext("mms-movie", "current folder")) {
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
    movie_parent = "path = (SELECT filename FROM Folders WHERE parent in (" + parent_ids + ")) AND";
  }

  std::vector<std::pair<std::string, int> > result;
  vector<int> ids_already_found;

  db_mutex.enterMutex();

  // folders
  SQLQuery *q = db.query("Folders", ("SELECT id, parent, filename, name, is_folder FROM %t WHERE " + parent + " fuzzycmp('" + search_word + "', lname, 1)").c_str());

  if (q) {

    for (int i = 0; i < q->numberOfTuples(); ++i) {

      SQLRow &row = (*q)[i];

      string out;

      bool result_ok = false;

      if (input_master->current_saved_map() == "movie" && search_depth == dgettext("mms-movie", "current folder")) {
	foreach (string& folder, folders.top().first)
	  if (row["filename"].find(folder) != string::npos) {
	    result_ok = true;
	    break;
	  }
      } else
	result_ok = true;

      if (result_ok && row["parent"] != "0") {
	if (row["is_folder"] == "1")
	  out = row["name"] + "/";
	else
	  out = row["name"];

	int id = conv::atoi(row["id"]);
	result.push_back(std::make_pair(string_format::convert(out), id));
	ids_already_found.push_back(id);
      }
    }
  }
  delete q;

  q = db.query("HDMovie", ("SELECT path, title, Folders.id as id FROM %t, Folders WHERE " + movie_parent + " fuzzycmp('" + search_word + "', ltitle, 1) AND Folders.filename = HDMovie.path").c_str());
  if (q) {
    for (int i = 0; i < q->numberOfTuples(); ++i) {

      SQLRow &row = (*q)[i];

      int id = conv::atoi(row["id"]);

      bool found_match = false;
      foreach (int id_already_found, ids_already_found)
	if (id == id_already_found) {
	  found_match = true;
	  break;
	}

      if (!found_match)
	result.push_back(std::make_pair(row["title"], id));
    }
  }
  delete q;

  db_mutex.leaveMutex();

  return result;
}

// commands
void GraphicalMovie::secondary_menu()
{
  ExtraMenu em;

  if (vector_lookup(files, folders.top().second).type != "file") {
    em.add_item(ExtraMenuItem(dgettext("mms-movie", "Enter directory"), input_master->find_shortcut("action"),
			      boost::bind(&GraphicalMovie::enter_dir, this)));

    em.add_item(ExtraMenuItem(dgettext("mms-movie", "Play all files in dir"), input_master->find_shortcut("play_movie"),
			      boost::bind(&GraphicalMovie::action_play, this)));
  } else
    em.add_item(ExtraMenuItem(dgettext("mms-movie", "Play video"), input_master->find_shortcut("action"),
			      boost::bind(&GraphicalMovie::action_play, this)));

  if (!vector_lookup(files, folders.top().second).m_strTitle.empty()) {
    em.add_item(ExtraMenuItem(dgettext("mms-movie", "Update imdb information"), 
			      input_master->find_shortcut("reget"),
			      boost::bind(&GraphicalMovie::reget_movie_information, this)));

    em.add_item(ExtraMenuItem(dgettext("mms-movie", "Delete imdb information"),
			      input_master->find_shortcut("delete_file_info"),
			      boost::bind(&GraphicalMovie::remove_from_db, this,
					  folders.top().second)));
  } else {
    em.add_item(ExtraMenuItem(dgettext("mms-movie", "Get information on imdb"), 
			      input_master->find_shortcut("reget"),
			      boost::bind(&GraphicalMovie::reget_movie_information, this)));
  }

  if (vector_lookup(files, folders.top().second).type == "file")
    em.add_item(ExtraMenuItem(dgettext("mms-movie", "Print file information"), input_master->find_shortcut("print_file_info"),
			      boost::bind(&GraphicalMovie::print_movie_info, this)));

  em.add_item(ExtraMenuItem(dgettext("mms-movie", "Print imdb information"), input_master->find_shortcut("info"),
			    boost::bind(&GraphicalMovie::print_info, this)));

  if (folders.size() > 1) {
    em.add_item(ExtraMenuItem(dgettext("mms-movie", "Go up one directory"), input_master->find_shortcut("back"),
			      boost::bind(&GraphicalMovie::go_back, this)));
    em.add_item(ExtraMenuItem(dgettext("mms-movie", "Return to startmenu"), input_master->find_shortcut("startmenu"),
			      boost::bind(&GraphicalMovie::exit, this)));
  } else
    em.add_item(ExtraMenuItem(dgettext("mms-movie", "Return to startmenu"), input_master->find_shortcut("back"),
			      boost::bind(&GraphicalMovie::exit, this)));

  add_standard(em);

  foreach (ExtraMenuItem& item, global->menu_items)
    em.add_persistent_item(item);

  conf->s_sam(true);

  em.mainloop();
}

void GraphicalMovie::print_movie_info()
{
  Multifile mf = vector_lookup(files, folders.top().second);

  if (mf.type == "file" && mf.filenames.size() > 0) {

    Movie_info *mv = new Movie_info;

    int x = conf->p_v_res()/3*2;

    foreach (const string &filename, mf.filenames) {
      if ((mv->probe_movie(filename))){
	vector<string> vec;
	MOVIE_RESPONSE_INFO a;
	mv->Fetch_Movie_data(&a);
	Print pdialog(Print::SCREEN);
	bool got_lines = (!a.general_info.empty() || !a.video_info.empty() || !a.audio_info.empty());

	if (!got_lines){
	  pdialog.add_line(dgettext("mms-movie", "No available information about this file"));
	  pdialog.print();
	} else {
	  //general information
	  if (string_format::wrap_strings(a.general_info, extra_information_font, x, vec) > 0){
	    pdialog.add_line(dgettext("mms-movie", "General information:"));
	    foreach (const string &str, vec)
	      pdialog.add_line(str);
	    pdialog.add_line("--");
	  }

	  if (string_format::wrap_strings(a.video_info, extra_information_font, x, vec) > 0){
	    pdialog.add_line(dgettext("mms-movie", "Video details:"));
	    foreach (const string &str, vec)
	      pdialog.add_line(str);
	    pdialog.add_line("--");
	  }

	  if (string_format::wrap_strings(a.audio_info, extra_information_font, x, vec) > 0){
	    pdialog.add_line(dgettext("mms-movie", "Audio details:"));
	    foreach (const string &str, vec)
	      pdialog.add_line(str);
	    pdialog.add_line("--");
	  }
	  pdialog.print();
	}
      }
    }

    delete mv;
  }
}

void GraphicalMovie::enter_dir()
{
  if (vector_lookup(files, folders.top().second).type != "file") {
    string temp_cur_dir = vector_lookup(files ,folders.top().second).path;
    vector<CIMDBMovie> templist = rdir(temp_cur_dir);

    if (templist.size())
      for (size_t t = 0; t < templist.size(); ++t){
	dprintf("%s\n", templist[t].name.c_str());
	if (string_format::lowercase(templist[t].name) == "video_ts"){
	  action_play();
	  return;
	}
      }

    if ((templist.size() == 0 && string_format::lowercase(temp_cur_dir).rfind("video_ts") != string::npos ) ) {
      action_play();
      return;
    }

    if (templist.size() != 0) {
      list<string> templs; templs.push_back(temp_cur_dir);
      folders.push(std::make_pair(templs, 0));
      files = templist;
      std::sort(files.begin(), files.end(), file_sort());

      S_BackgroundUpdater::get_instance()->run_once(boost::bind(&GraphicalMovie::check_for_changes, this));
    } else{
      dprintf("Graphicalmovie says Folder is empty\n");

      DialogWaitPrint pdialog(dgettext("mms-movie", "Folder is empty"), 1000);
    }
  }
}

void GraphicalMovie::check_for_changes()
{
  bool reload = false;

  list<string> top_folders = folders.top().first;

  foreach (string& dir, top_folders)
    if (reload_dir(dir))
      reload = true; // no break since we want to update the db for all folders

  reload_dirs = reload;
}

string GraphicalMovie::mainloop()
{
  visible = true;

  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();
  Shutdown *sd = S_Shutdown::get_instance();

  input_master->set_map("movie");

  Input input;

  bool update_needed = true;

  render->device->animation_section_begin(true);
  int curlayer = render->device->get_current_layer();
  render->device->switch_to_layer(curlayer+1);
  render->device->reset_layout_attribs_nowait();
  render->device->set_layout_alpha(0.0, curlayer+1);
  render->device->animation_zoom(3,3,1,1,40,curlayer+1);
  render->device->animation_fade(0,1,50,curlayer+1);
  render->device->animation_fade(1,0,50,curlayer);
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

      if (update_needed) {
	print_modifying(files);
	print_lcd_menu();
      }

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

      if (fullscreen_check(input))
	continue;

      if (input.command == "prev")
 	{
	  if (opts.display_mode() == dgettext("mms-movie", "list view")) {
	      if (folders.top().second != 0)
		folders.top().second -= 1;
	      else
		folders.top().second = files.size() - 1;
	    } else {
	      prev();
	    }
 	}
      else if (input.command == "next")
 	{
	  if (opts.display_mode() == dgettext("mms-movie", "list view"))
	    folders.top().second = (folders.top().second+1)%files.size();
	  else
	    next();
 	}
      else if (input.command == "left")
	{
	  if (opts.display_mode() == dgettext("mms-movie", "icon view"))
	    left();
	}
      else if (input.command == "right")
	{
	  if (opts.display_mode() == dgettext("mms-movie", "icon view"))
	    right();
	}
      else if (input.command == "back")
	{
	  one_up();
	}
      else if (input.command == "play_movie")
	{
	  action_play();
	}
      else if (input.command == "reget")
	{
	  reget_movie_information();
	}
      else if (input.command == "search_imdb")
	{
	  search_imdb();
	}
      else if (input.command == "info" && input.mode == "graphical" &&
	       vector_lookup(files, folders.top().second).type != "multi-dir")
	{
	  print_info();
	}
      else if (input.command == "print_file_info")
	{
	  print_movie_info();
	}
      else if (input.command == "action")
	{
	  action();
	}
      else if (input.command == "search")
	{
	  search_func();
        }
      else if (input.command == "second_action")
	{
	  secondary_menu();
	}
      else
	MovieTemplate<CIMDBMovie>::movie_mainloop_common(input);

      update_needed = !global->check_commands(input);
    }

  render->device->animation_section_begin();
  render->device->switch_to_layer(curlayer);
  render->device->reset_layout_attribs_nowait();
  render->device->animation_fade(0,1,80,curlayer);
  render->device->animation_zoom(1,1,4,4,40,curlayer+1);
  render->device->animation_fade(1,0,40,curlayer+1);
  render->device->animation_section_end();

  exit_loop = false;

  visible = false;

  return "";
}

vector<CIMDBMovie> GraphicalMovie::parse_dir(const list<string>& dirs)
{
  vector<CIMDBMovie> vec_files;

  foreach (const string& dir, dirs) {
    vector<CIMDBMovie> tempfiles = rdir(dir);
    vec_files.insert(vec_files.end(), tempfiles.begin(), tempfiles.end());
  }

  return vec_files;
}

vector<string> GraphicalMovie::check_db_for_folders(const string& parent)
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

    if (opts.dir_order() == dgettext("mms-movie", "directories first"))
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

bool GraphicalMovie::rdir_internal(const string& original_name, const string& argv,
				   vector<CIMDBMovie> &cur_files, bool& db_load_succes,
				   bool file_in_db)
{
  if (global->check_stop_bit()) {
    cur_files.clear();
    return false;
  }

  string filename = original_name;

  bool inside_movie_directory = false;
  list<string> filenames;

  // if the directory where the files reside is known to be a movie, use it
  // instead of trying to fetch information. But use the names of the files
  // to differentiate files like CD1.avi CD2.avi etc.

  string small_name = "";

  if (filename[filename.size()-1] == '/')
    filename = filename.substr(0, filename.size()-1);

  string::size_type pos_parent = filename.rfind("/");
  if (pos_parent != string::npos) {
    small_name = filename.substr(pos_parent+1);
    filename = filename.substr(0, pos_parent);
  }

  filename = original_name;

  if (filename[filename.size()-1] == '/')
    filename = filename.substr(0, filename.size()-1);

  if (isDirectory (filename)) { // dir

    if(!file_tools::dir_contains_known_files(filename, ext_mask, 10))
      return true;
    dprintf("%s contains know files\n", filename.c_str());

    Multifile dir = add_dir(filename);
    dir.path += "/";
    if (dir.filenames.size() != 0 || dir.filetype == "dir") {
      CIMDBMovie m = CIMDBMovie(dir);
      if (file_in_db) {
	std::pair<int, bool> info = db_info(m.path, true);
	m.db_id = info.first;
	m.thumbnailed_before_failed = info.second;
      }
      db_mutex.enterMutex();
      if (m.Load(physical, db))
	db_load_succes = true;
      db_mutex.leaveMutex();
      if (inside_movie_directory) {
	m.real_name = small_name;
	if (conf->p_convert())
	  m.real_name = string_format::convert(m.real_name);
	m.type = "file"; // we don't want to enter anything inside a
	// movie directory
	m.filenames = filenames;
      }
      cur_files.push_back(m);
    }
  } else { // file
    MyPair filetype = check_type(filename, movie_conf->p_filetypes_m());
    if (filetype != emptyMyPair) {
      Multifile e = addfile(filename, filetype);
      CIMDBMovie m = CIMDBMovie(e);
      if (file_in_db) {
	std::pair<int, bool> info = db_info(filename, false);
	m.db_id = info.first;
	m.thumbnailed_before_failed = info.second;
      }
      db_mutex.enterMutex();
      if (m.Load(physical, db))
	db_load_succes = true;
      db_mutex.leaveMutex();
      cur_files.push_back(m);
    }
  }

  return true;
}

void GraphicalMovie::insert_file_into_db(const string& filename, const string& parent)
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

  db_mutex.enterMutex();

  // check if already exists
  SQLQuery *q = db.query("Folders", ("SELECT id FROM %t WHERE filename='" + string_format::escape_db_string(good_filename) + "'").c_str());
  if (q && q->numberOfTuples() > 0) {
    delete q;
    db_mutex.leaveMutex();
    return;
  } else
    delete q;

  string parent_id = get_parent_id_movie(parent, db, navigating_media ? top_media_folders : movie_folders);

  string::size_type i;
  if ((i = cut_filename.rfind('/')) == string::npos)
    i = 0;
  string name = cut_filename.substr((i != 0) ? i+1 : i);

  if (!is_dir && (i = name.rfind('.')) != string::npos)
    name = name.substr(0, i);

  char *tmp_insert = sqlite3_mprintf("INSERT INTO Folders VALUES(NULL, '%q', '%q', '%q', '%q', '0', '%q')",
				     parent_id.c_str(), good_filename.c_str(), name.c_str(),
				     string_format::lowercase(name).c_str(), conv::itos(is_dir).c_str());
  db.execute(tmp_insert);
  sqlite3_free(tmp_insert);

  db_mutex.leaveMutex();
}

void GraphicalMovie::update_thumbnailed_status(int id)
{
  db_mutex.enterMutex();
  char *tmp_insert = sqlite3_mprintf("UPDATE Folders SET is_thumbnailed=1 where id='%q'",
				     conv::itos(id).c_str());
  db.execute(tmp_insert);
  sqlite3_free(tmp_insert);
  db_mutex.leaveMutex();
}

std::pair<int, bool> GraphicalMovie::db_info(const string& file, bool is_dir)
{
  int i = -1;
  int is_thumbnailed = 0;

  string good_file;

  if (is_dir)
    good_file = string_format::unique_folder_name(file);
  else
    good_file = file;

  db_mutex.enterMutex();

  SQLQuery *q = db.query("Folders", ("SELECT id, is_thumbnailed FROM %t WHERE filename='" + string_format::escape_db_string(good_file) + "'").c_str());
  if (q && q->numberOfTuples() > 0) {
    SQLRow &row = (*q)[0];
    i = conv::atoi(row["id"]);
    is_thumbnailed = conv::atoi(row["is_thumbnailed"]);
  }
  delete q;

  db_mutex.leaveMutex();

  return std::make_pair(i, is_thumbnailed == 1);
}

void GraphicalMovie::check_db_consistency()
{
  db_mutex.enterMutex();

  if (!db.hasTable("Folders")) {
    db_mutex.leaveMutex();
    return;
  }

  // make sure that we don't pop up a dialog as mms loads (before UI is ready)
  bool tmp_imdb_mes = imdb_message_not_displayed;
  imdb_message_not_displayed = false;

  //Need to call rdir for each folder listed in movie_folders, so that the "parent" db row is updated. Probably there's a better way of doing it
  foreach (const string& dir, movie_folders ) {
    vector<CIMDBMovie> tempfiles = rdir(dir);
  }

  imdb_message_not_displayed = tmp_imdb_mes;

  SQLQuery *q = db.query("Folders", "SELECT id, parent, filename FROM %t WHERE parent==0");
  if (q->numberOfTuples() >  movie_folders.size()) { //if there's more "root" dirs in DB than in config file, db should be updated
    for (int i = 0; i < q->numberOfTuples(); ++i) {
      string filename = (*q)[i]["filename"].c_str();
      bool found = false;
      foreach (string& dir, movie_folders) {
	if ( dir == filename )
	  found=true;
      }
      if (!found) {
	foreach (string& dir, movie_folders)
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

vector<CIMDBMovie> GraphicalMovie::rdir_hd(const string& argv, bool& db_load_succes, bool insert_into_db)
{
  vector<CIMDBMovie> cur_files;

  libfs_set_folders_first(opts.dir_order());

  if (insert_into_db) {
    for (file_iterator<file_t, default_order> i (argv); i != i.end (); i.advance(false)) {
      string filename = filesystem::FExpand(i->getName());
      if (isDirectory(filename) || check_type(filename, movie_conf->p_filetypes_m()) != emptyMyPair)
	insert_file_into_db(filename, argv);
    }

    vector<string> db_files = check_db_for_folders(argv);

    if (db_files.size() > 0) {
      foreach (string& file, db_files)
	if (!rdir_internal(file, argv, cur_files, db_load_succes, true))
	  break;
    }
  } else {
    for (file_iterator<file_t, default_order> i (argv); i != i.end (); i.advance(false)) {
      string filename = filesystem::FExpand(i->getName());
      if (!rdir_internal(filename, argv, cur_files, db_load_succes, true))
	break;
    }
  }

  return cur_files;
}

// read a dir and spit out a list of files and dirs.
vector<CIMDBMovie> GraphicalMovie::rdir(const string& argv)
{
  vector<CIMDBMovie> cur_files;

  vector<string> db_files = check_db_for_folders(argv);

  bool db_load_succes = false;

  if (db_files.size() > 0) {
    foreach (string& file, db_files)
      if (!rdir_internal(file, argv, cur_files, db_load_succes, true))
	break;
  } else {
    cur_files = rdir_hd(argv, db_load_succes, true);
  }

  // check if it's a root dir, we don't want to display the message for all
  // folders
  bool root_dir = false;

  foreach (const string& dir, movie_conf->p_movie_dirs())
    if (dir == argv) {
      root_dir = true;
      break;
    }

  if (!db_load_succes && imdb_message_not_displayed && root_dir) {
    DialogWaitPrint pdialog(5000);
    pdialog.add_line(dgettext("mms-movie", "No IMDb information found about videos"));
    pdialog.add_line(dgettext("mms-movie", "Use the reget video information action"));
    pdialog.add_line(dgettext("mms-movie", "to download information about a video"));
    pdialog.print();
    imdb_message_not_displayed = false;
  }

  return cur_files;
}

void GraphicalMovie::save_runtime_settings()
{
  std::ofstream file;

  string path = conf->p_var_data_dir() + "options/MovieGraphicalRuntime";

  file.open(path.c_str());

  if (!file) {
    print_critical(dgettext("mms-movie", "Could not write options to file ") + path, "MOVIE");
  } else {
    file << "imdb_warning_displayed," << imdb_message_not_displayed << std::endl;
  }

  file.close();
}

void GraphicalMovie::load_runtime_settings()
{
  // sane defaults
  bool imdb_warning = false;

  std::string path = conf->p_var_data_dir() + "options/MovieGraphicalRuntime";

  std::ifstream in;
  in.open(path.c_str(), std::ios::in);

  if (!in.is_open()) {
    print_critical(dgettext("mms-movie", "Could not open options file ") + path, "MOVIE");
  } else {

    string name, value, str;

    while (getline(in, str)) {

      string::size_type comma = str.find(",");

      if (comma == string::npos)
        continue;

      name = str.substr(0,comma);
      value = str.substr(comma+1);

      if (name == "imdb_warning_displayed")
	imdb_warning = conv::stob(value);
    }
    in.close();
  }

  imdb_message_not_displayed = imdb_warning;
}

