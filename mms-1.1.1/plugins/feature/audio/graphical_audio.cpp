#include "config.h"

#include "libfspp.hpp"

#include "graphical_audio.hpp"

#include "search.hpp"
#include "graphics.hpp"
#include "resolution.hpp"

#include "touch.hpp"

using std::string;
using std::vector;
using std::list;

GraphicalAudio::GraphicalAudio()
  : db((conf->p_var_data_dir() + "audio.db").c_str()), imms_cur_path(""),
    extracted_all_files(false), exit_choose_cover_loop(false), reload_dirs(false),
    is_in_add(false)
{
  check_db();
  check_db_consistency();
  ext_mask = file_tools::create_ext_mask(audio_conf->p_filetypes_a());

#ifdef use_imms
  imms = new IMMSC();
  imms->start();
  if (!imms->check_connection())
    std::cerr << dgettext("mms-audio", "Failed to connect to IMMS") << std::endl;
  imms->setup(true);
#endif

  visible = false;
  extracted_all_files = false;

  res_dependant_calc_2();

  S_ResolutionManagement::get_instance()->register_callback(boost::bind(&GraphicalAudio::res_dependant_calc_2, this));
}

void GraphicalAudio::res_dependant_calc_2()
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

  header_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
  header_box_size = static_cast<int>(header_size.second * 0.75);

  int top_height = 10 * 2 + header_size.second;

  int bottom_height = static_cast<int>(2 * button_playback_height * 0.85);

  // resolution independant stuff

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

  helper_letters_height = 24 + 25; // 24 = letters height, 25 = space
}

GraphicalAudio::~GraphicalAudio()
{
#ifdef use_imms
  delete imms;
#endif

  db_mutex.enterMutex();
  db.close();
  db_mutex.leaveMutex();
}

void GraphicalAudio::create_db()
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

  create_playlist_db();

  create_indexes();
}

void GraphicalAudio::create_playlist_db()
{
  vector<string> fields;
  fields.push_back("id INTEGER PRIMARY KEY");
  fields.push_back("audio_id INTEGER");
  db.createTable("Playlist", fields);
}

void GraphicalAudio::create_indexes()
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

void GraphicalAudio::check_db()
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

	create_db();
      }
    }
    delete q;
  } else {
    // create the whole db
    create_db();
  }
}

void GraphicalAudio::check_db_consistency()
{
  db_mutex.enterMutex();

  if (!db.hasTable("Folders")) {
    db_mutex.leaveMutex();
    return;
  }

  //Need to call rdir for each folder listed in audio_folders, so that the "parent" db row is updated. Probably there's a better way of doing it
  foreach (const string& dir, audio_folders ) {
    vector<Dbaudiofile> tempfiles = rdir(dir);
  }

  SQLQuery *q = db.query("Folders", "SELECT id, parent, filename FROM %t WHERE parent==0");
  if (q->numberOfTuples() >  audio_folders.size()) { //if there's more "root" dirs in DB than in config file, db should be updated
    for (int i = 0; i < q->numberOfTuples(); ++i) {
      string filename = (*q)[i]["filename"].c_str();
      bool found = false;
      foreach (string& dir, audio_folders) {
	if ( dir == filename )
	  found=true;
      }
      if ( ! found ) {
	foreach (string& dir, audio_folders)
	  if ( filename.find(dir, 0) != string::npos ) {
	    SQLQuery *q = db.query("Folders", ( "SELECT id FROM %t WHERE filename='" + dir + "'").c_str());
	    string parent_id =  (*q)[0]["id"].c_str();
	    db.execute(("UPDATE Folders SET parent = '" + parent_id + "' WHERE filename = '" + filename + "'").c_str());
	    insert_cover_into_db(filename, "dir"); //Insert the cover into db
	  }
      }
    }
  }
  db_mutex.leaveMutex();
}

void GraphicalAudio::reparse_file(const string& path)
{
  db_mutex.enterMutex();

  char *tmp = sqlite3_mprintf("DELETE FROM Folders WHERE filename LIKE '%q%%'", path.c_str());
  db.execute(tmp);
  sqlite3_free(tmp);

  tmp = sqlite3_mprintf("DELETE FROM Audio WHERE filename LIKE '%q%%'", path.c_str());
  db.execute(tmp);
  sqlite3_free(tmp);

  db.execute("DELETE FROM Album WHERE id NOT IN (SELECT DISTINCT Album FROM Audio)");
  db.execute("DELETE FROM Artist WHERE id NOT IN (SELECT DISTINCT Artist FROM Audio)");

  db_mutex.leaveMutex();

  // reload metadata
  Dbaudiofile die(path);
}

bool GraphicalAudio::reload_dir(const string& path)
{
  // generate new list
  vector<Dbaudiofile> hdfiles = rdir_hd(path, true);

  vector<Dbaudiofile> hd_sorted_by_id;
  add_tracks_from_dir(hdfiles, hd_sorted_by_id);

  vector<Dbaudiofile> dbfiles = rdir(path);

  vector<Dbaudiofile> db_sorted_by_id;
  add_tracks_from_dir(dbfiles, db_sorted_by_id);

  // find what is new and what is removed
  vector<string> new_list;
  foreach (Dbaudiofile& file, hd_sorted_by_id)
    new_list.push_back(file.path);

  vector<string> old_list;
  foreach (Dbaudiofile& file, db_sorted_by_id)
    old_list.push_back(file.path);

  vector<string> new_files;
  vector<string> removed_files;

  folder_difference(new_list, old_list, new_files, removed_files);

  // insert new into db
  foreach (string& path, new_files) {

    if (path[path.size() - 1] == '/')
      path = path.substr(0, path.size()-1);

    string parent_path = "";
    string::size_type pos_backslash = path.rfind('/');
    if (pos_backslash != string::npos) {
      parent_path = path.substr(0, pos_backslash+1);
    }

    insert_file_into_db(path, parent_path);
  }

  // remove old from db
  foreach (string& path, removed_files) {

    db_mutex.enterMutex();

    char *tmp = sqlite3_mprintf("DELETE FROM Folders WHERE filename LIKE '%q%%'", path.c_str());
    db.execute(tmp);
    sqlite3_free(tmp);

    tmp = sqlite3_mprintf("DELETE FROM Audio WHERE filename LIKE '%q%%'", path.c_str());
    db.execute(tmp);
    sqlite3_free(tmp);

    db_mutex.leaveMutex();

    // and from playlist
    for (vector<Dbaudiofile>::iterator pi = playlist.begin(); pi != playlist.end();) {
      if (pi->path.find(path) != string::npos) {
	remove_track_from_playlist(*pi);
	pi = playlist.erase(pi);
      } else
	++pi;
    }
  }

  if (removed_files.size() > 0) {
    // sync the other db's
    db_mutex.enterMutex();
    db.execute("DELETE FROM Album WHERE id NOT IN (SELECT DISTINCT Album FROM Audio)");
    db.execute("DELETE FROM Artist WHERE id NOT IN (SELECT DISTINCT Artist FROM Audio)");
    db_mutex.leaveMutex();
  }

  return (removed_files.size() > 0 || new_files.size() > 0);
}

void GraphicalAudio::reload_current_dirs()
{
  if (mode == PLAY && playlist.size() == 0) {
    mode = ADD;
    files = &audio_list;
  }
  else if (playlist_pos_int > playlist.size()-1)
    playlist_pos_int = playlist.size()-1;

  check_for_changes();
  load_current_dirs();

  if (audio_list.size() == 0)
    one_up();
}

string GraphicalAudio::id_to_filename(int db_id)
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

bool GraphicalAudio::change_dir_to_id(int db_id)
{
  db_mutex.enterMutex();

  SQLQuery *q = db.query("Folders", ("SELECT filename FROM %t WHERE id='" + conv::itos(db_id) + "'").c_str());
  if (q && q->numberOfTuples() > 0) {

    list<string> old_watch_folder = folders.top().first;

    SQLRow &row = (*q)[0];

    string filename = row["filename"];
    string cut_filename = filename;
    string name_in_folder, path, current_path = "";

    delete q;
    db_mutex.leaveMutex();

    if (filename[filename.size()-1] == '/')
      cut_filename = filename.substr(0, filename.size()-1);

    // FIXME: restore?
    for (int i = 0, size = folders.size(); i < size; ++i)
      folders.pop();

    string::size_type pos_backslash = cut_filename.rfind('/');
    if (pos_backslash != string::npos) {
      name_in_folder = cut_filename.substr(pos_backslash+1);
      path = cut_filename.substr(0, pos_backslash+1);
    }

    // FIXME: restore?
    audio_list = rdir(path);

    int pos_in_last_folder = 0;
    foreach (Dbaudiofile& file, audio_list) {
      if (file.path == filename)
	break;
      else
	++pos_in_last_folder;
    }

    list<string> dirs = audio_folders;

    foreach (string& folder, audio_folders)
      if (filename.find(folder) != string::npos) {
	current_path = path.substr(0, folder.size());
	path = path.substr(folder.size());
      }

    // stuff in db that is not in a folder that is currently subscribed
    assert(!current_path.empty());

    int pos;

    while ((pos = path.find('/')) != string::npos && path.size() != 1) {

      vector<Dbaudiofile> curlist = rdir(current_path);
      std::sort(curlist.begin(), curlist.end(), file_sort());

      current_path += path.substr(0, pos+1);

      int pos_in_folder = 0;
      foreach (Dbaudiofile& file, curlist) {
	if (file.path == current_path)
	  break;
	else
	  ++pos_in_folder;
      }

      folders.push(std::make_pair(dirs, pos_in_folder));

      path = path.substr(pos+1);

      dirs.clear();
      dirs.push_back(current_path);
    }

    folders.push(std::make_pair(dirs, pos_in_last_folder));

    loaded_correctly = true;

    return true;

  } else {
    delete q;
    db_mutex.leaveMutex();
    return false;
  }
}

void GraphicalAudio::reenter(int db_id)
{
  if (change_dir_to_id(db_id))
    mainloop(ADD);
}

vector<std::pair<string, int> > GraphicalAudio::gen_search_list(const string& search_word)
{
  string parent = "", audio_parent = "";

  if (input_master->current_saved_map() == "playlist") {

    string ids = "";
    foreach (Dbaudiofile& file, playlist)
      ids += conv::itos(file.db_id) + ", ";

    ids = ids.substr(0, ids.size()-2);

    parent = "id in (" + ids + ") AND";
    audio_parent = "Folders.filename in (SELECT Folders.filename FROM Folders WHERE id in (" + ids + ")) AND";

  } else if (input_master->current_saved_map() == "audio" && search_depth == dgettext("mms-audio", "current folder")) {
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
    audio_parent = "Folders.filename in (SELECT Folders.filename FROM Folders WHERE parent in (" + parent_ids + ")) AND";
  }

  vector<std::pair<string, int> > result;
  map<int, bool> ids;

  db_mutex.enterMutex();

  // folders
  SQLQuery *q = db.query("Folders", ("SELECT id, parent, filename, name FROM %t WHERE " + parent + " is_folder=1 AND fuzzycmp('" + search_word + "', lname, 1) LIMIT 100").c_str());

  if (q) {

    for (int i = 0; i < q->numberOfTuples(); ++i) {

      SQLRow &row = (*q)[i];

      bool result_ok = false;

      if (input_master->current_saved_map() == "audio" && search_depth == dgettext("mms-audio", "current folder")) {
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
	string out = row["name"] + "/";
	int id = conv::atoi(row["id"]);
	result.push_back(std::make_pair(out, id));
	ids.insert(std::make_pair(id, false));
      }
    }
  }
  delete q;

  q = db.query("Audio", ("SELECT Folders.id as id, Folders.filename as filename, Folders.name as name, Artist.name as Artist, Album.name as Album, Title FROM %t, Artist, Album, Folders WHERE " + audio_parent + " is_folder=0 AND (Folders.lname LIKE '%%" + search_word + "%%' OR lTitle LIKE '%%" + search_word + "%%' OR Album.lname LIKE '%%" + search_word + "%%' OR Artist.lname LIKE '%%" + search_word + "%%') AND Audio.Artist = Artist.id AND Audio.Album = Album.id AND Folders.filename = Audio.filename LIMIT 100").c_str());

  if (q) {
    for (int i = 0; i < q->numberOfTuples(); ++i) {

      SQLRow &row = (*q)[i];

      string out = row["Artist"] + " - " + row["Album"] + " - " + row["Title"];

      if (out == " -  - ")
	out = row["name"];

      int id = conv::atoi(row["id"]);

      if (ids.find(id) == ids.end()) {
	result.push_back(std::make_pair(out, id));
	ids.insert(std::make_pair(id, false));
      }
    }
  }
  delete q;

  db_mutex.leaveMutex();

  return result;
}

int GraphicalAudio::check_search()
{
  return 1000; // ms
}

bool GraphicalAudio::search_is_graphical_view(const std::vector<Dbaudiofile>& new_files)
{
  return !(mode == PLAY || no_folders_in_dir(new_files));
}

void GraphicalAudio::search_func()
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

  screen_updater->trigger.add(TriggerElement("search marker", empty, boost::bind(&GraphicalAudio::print_marked_search_letter, this),
					     boost::bind(&GraphicalAudio::clean_up_search_print, this)));
  screen_updater->timer.add(TimeElement("search marker", boost::bind(&GraphicalAudio::check_search_letter, this),
					boost::bind(&GraphicalAudio::print_marked_search_letter, this)));

  int pos = input_master->graphical_search<Dbaudiofile>(*files,
							boost::bind(&GraphicalAudio::print, this, _1),
							boost::bind(&GraphicalAudio::gen_search_list, this, _1),
							boost::bind(&GraphicalAudio::id_to_filename, this, _1),
							boost::bind(&GraphicalAudio::search_is_graphical_view, this, _1),
							images_per_row, search_mode, search_str, lowercase_search_str,
							search_depth, offset, search_help_offset,
							S_BusyIndicator::get_instance(), mode == ADD);

  screen_updater->trigger.del("search marker");
  screen_updater->timer.del("search marker");

  force_search_print_cleanup();

  if (list_contains(conf->p_input(), string("lirc")) || list_contains(conf->p_input(), string("evdev")))
    search_top_size -= search_size;

  if (pos != -1 && search_depth == dgettext("mms-audio", "current folder")) {
    if (mode == ADD)
      folders.top().second = pos;
    else
      playlist_pos_int = pos;
  } else if (pos != -1)
    change_dir_to_id(pos);
}

void GraphicalAudio::page_up()
{
  int jump = conf->p_jump();

  if (mode == ADD) {
    if (search_mode)
      jump = images_per_row*rows_search;
    else
      jump = images_per_row*rows;
  }

  if (files->size() > jump) {
    int new_pos = position_int()-jump;
    if (position_int() == 0)
      set_position_int(files->size()-1+new_pos);
    else if (new_pos < 0)
      set_position_int(0);
    else
      set_position_int(new_pos);
  }
}

void GraphicalAudio::page_down()
{
  int jump = conf->p_jump();

  if (mode == ADD) {
    if (search_mode)
      jump = images_per_row*rows_search;
    else
      jump = images_per_row*rows;
  }

  if (files->size() > jump) {
    if (position_int() > (files->size() - jump) && position_int() != (files->size()-1))
      set_position_int(files->size()-1);
    else
      set_position_int((position_int()+jump)%files->size());
  }
}

string GraphicalAudio::mainloop(AudioMode new_mode)
{
  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();
  Shutdown *sd = S_Shutdown::get_instance();

  search_top_size = 24;

  visible = true;

  mainloop_top(new_mode);

  if (mode == ADD)
    is_in_add = true;

  exit_fall_through = false;

  Input input;

  bool update_needed = true;

  render->device->animation_section_begin(true);
  int curlayer = render->device->get_current_layer();
  render->device->switch_to_layer(curlayer+1);
  render->device->reset_layout_attribs_nowait();
  render->device->set_layout_alpha(0.0, curlayer+1);
  if (new_mode == !Audio::PLAY){
    render->device->animation_zoom(0,0,1,1,20,curlayer+1);
    render->device->animation_fade(0,1,40,curlayer+1);
    render->device->animation_fade(1,0,40,curlayer);
  } else{
    render->device->animation_zoom(3,3,1,1,40,curlayer+1);
    render->device->animation_fade(0,1,50,curlayer+1);
    render->device->animation_fade(1,0,50,curlayer);
  }

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
	print_modifying(*files);
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

      // general
      if (input.command == "page_up")
 	{
	  page_up();
 	}
      else if (input.command == "page_down")
 	{
	  page_down();
 	}
      else if (input.command == "second_action")
	{
	  secondary_menu();
	}
      else if (input.command == "search")
	{
	  search_func();
	}
      else if (input.command == "options")
	{
	  options();
	}
      else if (input.command == "cover" && vector_lookup(*files, position_int()).type == "dir")
	{
	  choose_cover();
	}
      else if (input.command == "info" && input.mode == "graphical")
	{
	  print_information();
	}
      // playlist
      else if (mode == PLAY && input.command == "move_up")
	{
	  move_up();
	}
      else if (mode == PLAY && input.command == "move_down")
	{
	  move_down();
	}
      else if (mode == PLAY && input.command == "delete")
	{
	  delete_track();
	}
      else if (mode == PLAY && input.command == "queue")
	{
	  queue_track();
	}
      else if (mode == PLAY && input.command == "clear_list")
	{
	  clear_playlist();
	}
      else if (mode == PLAY && input.command == "save_playlist")
	{
	  save_playlist_func();
	}
      else if (mode == PLAY && input.command == "action")
	{
	  play_track();
	}
      else if (mode == PLAY && input.command == "startmenu" && input.mode == "general")
	{
	  exit_playlist();
	}
      else if (mode == ADD && input.command == "startmenu" && input.mode == "general")
	{
	  exit();
	}
      // audio-add and no folders
      else if (mode == ADD && !no_folders_in_dir(*files)) {
	if (input.command == "prev")
	  {
	    if (files->size() > images_per_row) {
	      for (int i = 0; i < images_per_row; ++i) {
		if (folders.top().second == 0) {
		  folders.top().second = files->size()-1;
		  i += images_per_row-files->size()%images_per_row;
		}
		else
		  folders.top().second -= 1;
	      }
	    }
	  }
	else if (input.command == "next")
	  {
	    if (files->size() > images_per_row) {
	      for (int i = 0; i < images_per_row; ++i) {
		if (folders.top().second == files->size()-1 && i == 0) {
		  folders.top().second = 0;
		  break;
		} else if (folders.top().second == files->size()-1) {
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
	      folders.top().second = files->size() - 1;
	  }
	else if (input.command == "right")
	  {
	    folders.top().second = (folders.top().second+1)%files->size();
	  }
	else if (input.command == "back")
	  {
	    one_up();
	  }
	else if (input.command == "add_dir")
	  {
	    if (vector_lookup(*files, position_int()).type == "dir")
	      add();
	  }
	else if (input.command == "action")
	  {
	    if (vector_lookup(*files, position_int()).type == "dir")
	      enter_dir();
	    else
	      add();
	  }
	else if (input.command == "add_all")
	  {
	    add_all();
	  }
  	else if (input.command == "play_now")
	  {
	    if (audio_conf->p_audio_mode_playnow()) {
	      if (vector_lookup(*files, position_int()).type == "dir")
		play_now();
	      else
		play_track_now();
	    }
	  }
      } else {
	// general and no folders
	if (input.command == "prev")
	  {
	    if (position_int() != 0)
	      set_position_int(position_int()-1);
	    else
	      set_position_int(files->size()-1);
	  }
	else if (input.command == "next")
	  {
	    set_position_int((position_int()+1)%files->size());
	  }
	else if (input.command == "back" && input.mode == "general")
	  {
	    one_up();
	  }
	// audio-add and no folders
	else if (mode == ADD && input.command == "action")
	  {
	    add();
	  }
	else if (mode == ADD && input.command == "add_all")
	  {
	    add_all();
	  }
	else if (mode == ADD && input.command == "play_now")
	  {
	    if (vector_lookup(*files, position_int()).type != "dir")
		play_track_now();
	  }
	else if (mode == ADD && input.command == "similar")
	  {
	    if (vector_lookup(*files, position_int()).type != "dir")
	      generate_similar_track_playlist();
	  }
	
      }
      update_needed = !global->check_commands(input);
    }

  render->device->animation_section_begin();
  render->device->layer_active_no_wait(false);
  render->device->switch_to_layer(curlayer);
  render->device->reset_layout_attribs_nowait();
  render->device->animation_fade(0,1,80,curlayer);
  render->device->animation_section_end();

  if (!exit_fall_through)
    exit_loop = false;

  if (!is_in_add)
    exit_loop = false;

  exit_fall_through = false;

  if (in_playlist)
    in_playlist = false;

  if (is_in_add && mode == ADD)
    is_in_add = false;

  visible = false;

  return "";
}

void GraphicalAudio::one_up()
{
  if (search_mode)
    return;

  if (mode == ADD && folders.size() > 1)
    leave_dir();
  else
    exit();
}

string GraphicalAudio::get_cover(const Simplefile& file)
{
  string path = "";

  if (file.type == "media-track") {
    path = file.media_id;
  } else {
    if (file.type == "dir")
      path = string_format::unique_folder_name(file.path);
    else {
      string::size_type dir_seperator = file.path.rfind('/');
      if (dir_seperator != string::npos)
	path = string_format::unique_folder_name(file.path.substr(0, dir_seperator+1));
    }
  }

  string cover_path = "";

  db_mutex.enterMutex();

  SQLQuery *q = db.query("Covers", ("SELECT Cover FROM %t WHERE Path='" + string_format::escape_db_string(path) + "'").c_str());
  if (q && q->numberOfTuples() > 0) {
    SQLRow &row = (*q)[0];
    cover_path = row["Cover"];
  }
  delete q;

  db_mutex.leaveMutex();

  return cover_path;
}

vector<string> GraphicalAudio::genres_for_artist(const string& artist)
{
  db_mutex.enterMutex();

  vector<string> genres;

  SQLQuery *q = db.query("Genre", ("SELECT Genre.name as name FROM %t, Artist, GAudio WHERE Genre.id = GAudio.gid AND GAudio.aid = Artist.id AND Artist.lname = '" + string_format::lowercase(artist) + "'").c_str());
  if (q->numberOfTuples() > 0) {
    for (int i = 0; i < q->numberOfTuples(); ++i) {
      genres.push_back((*q)[i]["name"]);
    }
  }

  db_mutex.leaveMutex();

  return genres;
}

void GraphicalAudio::generate_similar_track_playlist()
{
  Dbaudiofile d = vector_lookup(*files, position_int());
  if (load_playlist_from_db(LastFM::lookup_similar_tracks(d.artist, d.title)))
    toggle_playlist();
  else
    DialogWaitPrint pdialog(dgettext("mms-audio", "No similar tracks found in local db"), 3000);
}

bool GraphicalAudio::load_playlist_from_db(const std::vector<LastFM::Track>& tracks)
{
  vector<Dbaudiofile> db_audio_files;  

  foreach (const LastFM::Track& track, tracks) {
    SQLQuery *q = db.query("Audio", ("SELECT filename FROM %t, Artist WHERE lTitle = '" + string_format::escape_db_string(string_format::lowercase(track.title)) + "' AND Artist.id = Audio.Artist AND Artist.lname = '" + string_format::escape_db_string(string_format::lowercase(track.artist))  + "'").c_str());

    if (q && q->numberOfTuples() > 0) {
      SQLRow &row = (*q)[0];
      db_audio_files.push_back(Dbaudiofile(row["filename"]));
    }

    delete q;
  }

  if (db_audio_files.size() > 0) {
    set_playlist(db_audio_files);
    audio_state->p->stop();
    return true;
  } else
    return false;
}

void GraphicalAudio::secondary_menu_in_add(ExtraMenu &em)
{
  bool is_dir = (vector_lookup(*files, position_int()).type == "dir");

  if (is_dir) {
    em.add_item(ExtraMenuItem(dgettext("mms-audio", "Enter directory"), input_master->find_shortcut("action"),
			      boost::bind(&GraphicalAudio::enter_dir, this)));

    em.add_item(ExtraMenuItem(dgettext("mms-audio", "Add directory to playlist"), input_master->find_shortcut("add_dir"),
			      boost::bind(&GraphicalAudio::add, this)));

    em.add_item(ExtraMenuItem(dgettext("mms-audio", "Choose cover for folder"), input_master->find_shortcut("cover"),
			      boost::bind(&GraphicalAudio::choose_cover, this)));

  } else {
    em.add_item(ExtraMenuItem(dgettext("mms-audio", "Add track to playlist"), input_master->find_shortcut("action"),
			      boost::bind(&GraphicalAudio::add, this)));
    em.add_item(ExtraMenuItem(dgettext("mms-audio", "Generate playlist of similar tracks"), 
			      input_master->find_shortcut("similar"),
			      boost::bind(&GraphicalAudio::generate_similar_track_playlist, this)));
  }

  if (folders.size() > 1)
    em.add_item(ExtraMenuItem(dgettext("mms-audio", "Go up one directory"), input_master->find_shortcut("back"),
			      boost::bind(&GraphicalAudio::leave_dir, this)));
  else
    em.add_item(ExtraMenuItem(dgettext("mms-audio", "Return to startmenu"), input_master->find_shortcut("back"),
			      boost::bind(&GraphicalAudio::exit, this)));
}

void GraphicalAudio::secondary_menu_commands(ExtraMenu &em)
{
  if (vector_lookup(*files, position_int()).type != "dir")
    em.add_item(ExtraMenuItem(dgettext("mms-audio", "Print information"), input_master->find_shortcut("info"),
			      boost::bind(&GraphicalAudio::print_information, this)));
}

void GraphicalAudio::enter_dir()
{
  if (vector_lookup(*files, position_int()).type == "dir") {
    string temp_cur_dir = vector_lookup(*files, position_int()).path;

    int nr_files = 0;
    for (file_iterator<file_t, default_order> i (temp_cur_dir); i != i.end (); i.advance(false))
      if (!isDirectory(i->getName()) && check_type(i->getName(), audio_conf->p_filetypes_a()) != emptyMyPair)
	++nr_files;

    ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

    screen_updater->status_progressbar = 0;
    screen_updater->total_progressbar = nr_files;
    screen_updater->header = dgettext("mms-audio", "Extracting metadata");

    screen_updater->timer.add(TimeElement("progressbar", &check_progressbar, &print_progressbar));

    vector<Dbaudiofile> templist = rdir(temp_cur_dir);

    screen_updater->timer.del("progressbar");
    cleanup_progressbar();

    if (templist.size() != 0) {

      list<string> templs;
      if (temp_cur_dir[temp_cur_dir.size()-1] != '/')
	templs.push_back(temp_cur_dir + '/');
      else
	templs.push_back(temp_cur_dir);
      folders.push(std::make_pair(templs, 0));
      audio_list = templist;
      std::sort(audio_list.begin(), audio_list.end(), file_sort());

      // sort by track id
      vector<Dbaudiofile> temp_files;
      add_tracks_from_dir(audio_list, temp_files);
      audio_list = temp_files;

      files = &audio_list;

      S_BackgroundUpdater::get_instance()->run_once(boost::bind(&GraphicalAudio::check_for_changes, this));

    } else
      DialogWaitPrint pdialog(dgettext("mms-audio", "Folder is empty"), 1000);
  }
}

void GraphicalAudio::check_for_changes()
{
  bool reload = false;

  list<string> top_folders = folders.top().first;

  foreach (string& dir, top_folders)
    if (reload_dir(dir))
      reload = true; // no break since we want to update the db for all folders

  reload_dirs = reload;
}

void GraphicalAudio::print_audiotrack_helper(int &x_pos)
{
  string pic = get_cover(audio_state->p->p_cur_nr());

  if (!pic.empty()) {
    x_pos = 2 * button_playback_height + 7;

    int size = 2 * button_playback_height - 7;

    render->create_scaled_image_wrapper_upscaled(pic, size, size);

    int pic_y_pos = conf->p_v_res()-size-7;
    audio_state->playback.add(new PFObj(pic, 7 + global->get_playback_offset(), pic_y_pos, size, size, false, 1));
  }

  x_pos += global->get_playback_offset();
}

void GraphicalAudio::get_audiotrack_info(string& buffer, string& artist, string& album, string& title)
{
  // the extracted information is more reliant for VBR mp3 than the audio
  // player, same for artist, album and title
  if (!audio_state->p->p_cur_nr().path.empty() && audio_state->p->p_cur_nr().type != "web" &&
      audio_state->p->p_cur_nr().type != "media-file" && audio_state->p->p_cur_nr().type != "media-track") {

    Dbaudiofile dbaf(audio_state->p->p_cur_nr().path);
    buffer = format_time(audio_state->p->p_cur_time(), dbaf.length != 0 ? dbaf.length : audio_state->p->p_total_time());
    artist = string_format::convert(dbaf.artist, true);
    album = string_format::convert(dbaf.album, true);
    title = string_format::convert(dbaf.title, true);
    if (title.empty() && artist.empty()){
      title = audio_state->p->p_title();
      artist = audio_state->p->p_artist();
      album = audio_state->p->p_album();
    }

  } else {
    if (audio_state->p->p_cur_nr().type == "web")
      buffer = format_time(audio_state->p->p_cur_time());
    else
      buffer = format_time(audio_state->p->p_cur_time(), audio_state->p->p_total_time());
    artist = audio_state->p->p_artist();
    album = audio_state->p->p_album();
    title = audio_state->p->p_title();
  }
}

bool GraphicalAudio::print_audio_screensaver_helper(int &x_pos, int &y, int &max_x_pic,
						    int &max_y_pic, bool vert_center)
{
  string pic = get_cover(audio_state->p->p_cur_nr());

  if (!pic.empty()) {
    render->create_scaled_image_wrapper_upscaled(pic, max_x_pic, max_y_pic);
    PFObj *p = new PFObj(pic, 60, 60, max_x_pic, max_y_pic, false, 3);
    if (vert_center)
      y = (conf->p_v_res()-p->real_h)/2;
    p->y = y;
    p->real_y = p->y - 1;
    audio_state->playback_fullscreen.add(p);
    max_x_pic = p->w;
    max_y_pic = p->h;
    return true;
  } else {
    x_pos = 60;
    return false;
  }
}

void GraphicalAudio::print_choose_cover(const vector<string>& covers, int cover_pos, Overlay& o)
{
  render->wait_and_aquire();

  S_Touch::get_instance()->clear();

  if (o.elements.size() > 0)
    o.partial_cleanup();

  int x, y, max_x = conf->p_h_res()-250, max_y = 90;
  if (conf->p_v_res() == 480)
    max_y = 50;

  int cover_width = static_cast<int>(image_width*1.35)+25;
  int cover_height = static_cast<int>(image_height*1.35)+15;

  int covers_per_row = (max_x-30)/cover_width;
  int image_rows = (conf->p_v_res()-(2*max_y))/cover_height;

  int max = std::min(covers_per_row*image_rows, int(covers.size()));

  int box_height = cover_height*(((max % covers_per_row == 0) ? 0 : 1) + max/covers_per_row)+30+50;

  x = (conf->p_h_res()-(max_x+30))/2;
  y = (conf->p_v_res()-(box_height))/2;

  o.add(new RObj(x-15+2, y-15+2, max_x+30+1, box_height+1, 0, 0, 0, 200, 0));
  o.add(new RObj(x-15, y-15, max_x+30, box_height,
		 themes->dialog_background1, themes->dialog_background2,
		 themes->dialog_background3, themes->dialog_background4, 1));

  string header = dgettext("mms-audio", "choose a cover");

  int x_size = string_format::calculate_string_width(header, dialog_header);

  o.add(new TObj(header, dialog_header, (conf->p_h_res()-x_size)/2, y-5,
		 themes->dialog_font1, themes->dialog_font2, themes->dialog_font3, 3));
  y += 50;

  // print the range
  int pos_in_list = cover_pos;
  int screen_pos, start;

  if (covers.size() > image_rows*covers_per_row) {
    if (pos_in_list < std::ceil(double(image_rows)/2.0)*covers_per_row) {
      screen_pos = pos_in_list;
      start = 0;
    } else {
      screen_pos = pos_in_list%covers_per_row+covers_per_row;
      if (screen_pos == covers_per_row-1)
	screen_pos = 2*covers_per_row-1;
      start = pos_in_list-screen_pos;
    }
  } else {
    screen_pos = pos_in_list;
    start = 0;
  }

  x = (conf->p_h_res()-covers_per_row*cover_width)/2;
  int x_start = x;

  for (int j = 0; j < image_rows*covers_per_row && j < covers.size(); ++j) {

    string pic = vector_lookup(covers, start);

    int pic_width = image_width;
    int pic_height = image_height;

    if (screen_pos == j) {
      pic_width = static_cast<int>(1.35*image_width);
      pic_height = static_cast<int>(1.35*image_height);
    }

    render->create_scaled_image_wrapper_upscaled(pic, pic_width, pic_height);
    PFObj *p = new PFObj(pic, x, y, pic_width, pic_height, false, 3);

    p->x = p->real_x = x + (cover_width - p->real_w)/2;
    p->y = p->real_y = y + (cover_height - p->real_h)/2;

    p->real_x -= 1;
    p->real_y -= 1;

    o.add(p);

    S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&GraphicalAudio::find_cover_element_and_do_action, this, pic)));

    x += cover_width;

    if (start == covers.size()-1) {
      if ((j+1)%covers_per_row != 0)
	j += (covers_per_row-(j+1)%covers_per_row);

      x = x_start;
      y += cover_height;

      start = 0;
      continue;

    } else
      ++start;

    if ((j+1)%covers_per_row == 0) {
      y += cover_height;
      x = x_start;
    }
  }

  render->draw_and_release("choose cover screen", true);
}

void GraphicalAudio::choose_cover()
{
  // find all covers available
  covers.clear();
  cover_pos = 0;

  MyPair filetype;

  libfs_set_folders_first(opts.dir_order());

  for (file_iterator<file_t, default_order> i (vector_lookup(*files, position_int()).path);
       i != i.end (); ++i)
    {
      // if the user cancels, just don't show anything
      if (global->check_stop_bit())
	return;

      filetype = check_type(i->getName(), conf->p_filetypes_p());

      if (filetype != emptyMyPair)
	covers.push_back(i->getName());
    }

  if (covers.size() == 0) {
    DialogWaitPrint pdialog(dgettext("mms-audio", "No covers found"), 2000);
    return;
  }

  // display a list and let the user select one
  S_BusyIndicator::get_instance()->idle();

  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
  screen_updater->timer.deactivate("audio_fullscreen");

  Overlay overlay = Overlay("dialog");

  Input input;

  int max_x = conf->p_h_res()-250;
  int cover_width = static_cast<int>(image_width*1.35)+25;
  int covers_per_row = (max_x-30)/cover_width;

  while (!exit_choose_cover_loop) {

    print_choose_cover(covers, cover_pos, overlay);

    input = input_master->get_input();

    if (input.key == "touch_input") {
      S_Touch::get_instance()->run_callback();
      continue;
    }

    if (input.command == "back" && input.mode == "general")
      {
	exit_choose_cover_loop = true;
      }
    else if (input.command == "left")
      {
	if (cover_pos != 0)
	  --cover_pos;
	else
	  cover_pos = covers.size() - 1;
      }
    else if (input.command == "right")
      {
	if (cover_pos != covers.size()-1)
	  ++cover_pos;
	else
	  cover_pos = 0;
      }
    else if (input.command == "prev")
      {
	if (covers.size() > covers_per_row) {
	  cover_pos = cover_pos - covers_per_row;
	  if (cover_pos < 0)
	    cover_pos = covers.size() - 1 + cover_pos;
	}
      }
    else if (input.command == "next")
      {
	cover_pos = (cover_pos + covers_per_row) % covers.size();
      }
    else if (input.command == "action")
      {
	use_cover();
      }
  }

  render->wait_and_aquire();
  overlay.cleanup();
  render->image_mut.leaveMutex();

  exit_choose_cover_loop = false;

  screen_updater->timer.activate("audio_fullscreen");
}

void GraphicalAudio::find_cover_element_and_do_action(const string& pic)
{
  int p = 0;
  foreach (string& cover, covers) {
    if (cover == pic) {
      cover_pos = p;
      break;
    } else
      ++p;
  }

  use_cover();
}

void GraphicalAudio::use_cover()
{
  string cur_dir = string_format::unique_folder_name(vector_lookup(*files, position_int()).path);

  db_mutex.enterMutex();
  db.execute(("UPDATE Covers SET Cover = '" + string_format::escape_db_string(vector_lookup(covers, cover_pos)) + "' WHERE Path = '" + string_format::escape_db_string(cur_dir) + "'").c_str());
  db_mutex.leaveMutex();

#if 0
  if (!file_exists(render->image_get_fullpath(cover, image_width, image_height))) {
    render->create_scaled_image_wrapper(cover, image_width, image_height);
    // to insert the image into the transmap so that it won't fail when
    // we draw the screen again
    bool tmp;
    render->image_dimensions(render->image_get_fullpath(cover, image_width, image_height), &tmp);
  }
#endif

  exit_choose_cover_loop = true;
}

void GraphicalAudio::print_information()
{
  if (isDirectory(vector_lookup(*files, position_int()).path))
    return;

  Dbaudiofile dbaf = vector_lookup(*files, position_int());

  vector<string> genres = genres_for_artist(dbaf.artist);
  if (genres.size() == 0) // fetch
    genres = LastFM::lookup_and_insert_genres(dbaf.artist, db, db_mutex);

  visible = false;

  S_BusyIndicator::get_instance()->idle();

  printing_information = true;

  render->prepare_new_image();

  render->current.add(new PObj(themes->audio_background, 0, 0, 0, SCALE_FULL));

  render->current.add(new PFObj(themes->startmenu_music_dir, 25, 10, header_box_size, header_box_size, 2, true));

  if (themes->show_header) {
    string header = dgettext("mms-audio", "Audio - Information");

    render->current.add(new TObj(header, header_font, 25 + header_box_size + 10, 10 + (header_box_size - header_size.second)/2,
				 themes->audio_header_font1, themes->audio_header_font2, themes->audio_header_font3, 2));
  }

  int y = 100;

  // lookup information each time see this information as the rating of the
  // song might have changed
  if (dbaf.type != "media-track" && dbaf.type != "media-file" && dbaf.type != "web")
    dbaf = Dbaudiofile(dbaf.path);

  string pic = get_cover(vector_lookup(*files, position_int()));

  int max_x_pic = static_cast<int>(conf->p_h_res()/3.0);
  int max_y_pic = static_cast<int>(conf->p_v_res()/2.5);

  int x = max_x_pic + 35 + 35;
  if (pic.empty())
    x = 50;

  if (!pic.empty()) {
    render->create_scaled_image_wrapper_upscaled(pic, max_x_pic, max_y_pic);
    PFObj *p = new PFObj(pic, 35, y+5, max_x_pic, max_y_pic, true, 3);
    render->current.add(p);
    x = p->w + 35 + 35;
  }

  int info_height = graphics::calculate_font_height(information_font, conf);

  int w, first_column_width = 0;
  w = string_format::calculate_string_width(dgettext("mms-audio", "Artist:"), information_font);
  first_column_width = std::max(w, first_column_width);
  w = string_format::calculate_string_width(dgettext("mms-audio", "Album:"), information_font);
  first_column_width = std::max(w, first_column_width);
  w = string_format::calculate_string_width(dgettext("mms-audio", "Title:"), information_font);
  first_column_width = std::max(w, first_column_width);
  w = string_format::calculate_string_width(dgettext("mms-audio", "Year:"), information_font);
  first_column_width = std::max(w, first_column_width);
  w = string_format::calculate_string_width(dgettext("mms-audio", "Genres:"), information_font);
  first_column_width = std::max(w, first_column_width);
  w = string_format::calculate_string_width(dgettext("mms-audio", "Bitrate:"), information_font);
  first_column_width = std::max(w, first_column_width);
  w = string_format::calculate_string_width(dgettext("mms-audio", "Length:"), information_font);
  first_column_width = std::max(w, first_column_width);
#ifdef use_imms
  w = string_format::calculate_string_width(dgettext("mms-audio", "Rating:"), information_font);
  first_column_width = std::max(w, first_column_width);
#endif
  w = string_format::calculate_string_width(dgettext("mms-audio", "Track:"), information_font);
  first_column_width = std::max(w, first_column_width);

  first_column_width += 10;

  string artist = string_format::convert(dbaf.artist, true);
  string_format::format_to_size(artist, information_font, conf->p_h_res()-(x+first_column_width)-25, true);
  render->current.add(new TObj(dgettext("mms-audio", "Artist:"), information_font, x, y, themes->audio_font1,
			       themes->audio_font2, themes->audio_font3, 3));
  render->current.add(new TObj(artist, information_font,
			       x+first_column_width, y,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));
  y += info_height;

  string album = string_format::convert(dbaf.album, true);
  string_format::format_to_size(album, information_font, conf->p_h_res()-(x+first_column_width)-25, true);
  render->current.add(new TObj(dgettext("mms-audio", "Album:"), information_font, x, y,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));
  render->current.add(new TObj(album, information_font,
			       x+first_column_width, y,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));
  y += info_height;

  string title = string_format::convert(dbaf.title, true);
  string_format::format_to_size(title, information_font, conf->p_h_res()-(x+first_column_width)-25, true);
  render->current.add(new TObj(dgettext("mms-audio", "Title:"), information_font, x, y,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));
  render->current.add(new TObj(title, information_font,
			       x+first_column_width, y,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));
  y += info_height;

  render->current.add(new TObj(dgettext("mms-audio", "Genres:"), information_font, x, y,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));

  string sgenres;
  foreach (string& genre, genres)
    sgenres += genre + ", ";

  sgenres = sgenres.substr(0, sgenres.size()-2);

  int i = 0;
  int text_width = conf->p_h_res()-(x+first_column_width)-25;

  while (i != sgenres.size()) {
    string descr = string_format::get_line(sgenres, i, text_width, information_font);
    
    render->current.add(new TObj(descr, information_font,
				 x+first_column_width, y,
				 themes->audio_font1, themes->audio_font2,
				 themes->audio_font3, 3));
    y += info_height;
  }

  // else
  if (sgenres.size() == 0)
    y += info_height;

  render->current.add(new TObj(dgettext("mms-audio", "Year:"), information_font, x, y,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));
  render->current.add(new TObj(conv::itos(dbaf.year), information_font,
			       x+first_column_width, y,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));
  y += info_height;

  render->current.add(new TObj(dgettext("mms-audio", "Bitrate:"), information_font, x, y,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));
  render->current.add(new TObj(conv::itos(dbaf.bitrate), information_font,
			       x+first_column_width, y,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));
  y += info_height;

  std::ostringstream time;

  if (remaining_seconds(dbaf.length) < 10)
    time << seconds_to_minutes(dbaf.length) << ":0" << remaining_seconds(dbaf.length);
  else
    time << seconds_to_minutes(dbaf.length) << ":" << remaining_seconds(dbaf.length);

  render->current.add(new TObj(dgettext("mms-audio", "Length:"), information_font, x, y,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));
  render->current.add(new TObj(time.str(), information_font,
			       x+first_column_width, y,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));
  y += info_height;

#ifdef use_imms
  std::ostringstream rating;
  if (dbaf.rating != -1)
    rating << dbaf.rating;
  else
    rating << dgettext("mms-audio", "not rated yet");

  render->current.add(new TObj(dgettext("mms-audio", "Rating:"), information_font, x, y,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));
  render->current.add(new TObj(rating.str(), information_font, x+first_column_width, y,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));
  y += info_height;
#endif

  render->current.add(new TObj(dgettext("mms-audio", "Track:"), information_font, x, y,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));
  render->current.add(new TObj(conv::itos(dbaf.track), information_font,
			       x+first_column_width, y,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));
  y += info_height;

  render->draw_and_release("Audio information");

  input_master->get_input();

  printing_information = false;

  visible = true;
}

bool GraphicalAudio::file_exists_in_db(const string& file)
{
  return (db_id(file, false) != -1);
}

int GraphicalAudio::db_id(const string& file, bool is_dir)
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

string GraphicalAudio::get_name_from_file(const Dbaudiofile& d)
{
  return d.to_string();
}

string GraphicalAudio::get_short_name_from_file(const Dbaudiofile& d)
{
  return d.short_to_string();
}

void GraphicalAudio::print_element(Dbaudiofile& dbaf, const Dbaudiofile& position,
				   int y, bool picture_dir)
{
  const int spacing = 14;

  if (!dbaf.is_fetched() && dbaf.type != "media-track" && dbaf.type != "media-file" &&
      dbaf.type != "web") {
    Dbaudiofile tmp_dbaf(dbaf.path);
    tmp_dbaf.set_values(dbaf);
    dbaf = tmp_dbaf;
  }

  string name;
  int max_width;

  // predict max. width of song length + some space (this also takes queue into account)
  int max_time_size = string_format::calculate_string_width("   0000:00", list_font);

  int text_extra_spacing = 0;

  if (picture_dir && mode == ADD && (search_depth == dgettext("mms-audio", "current folder") || !search_mode))
  {
    name = dbaf.get_name_from_file_in_picture_dir();

    text_extra_spacing = 3 * spacing;

    // bg rectangle is p_h_res - 2 * 55 (s. module.cpp)
    // spacing: 1x rect to text, 1x text to picture, 1x picture to rect
    int cover_size = std::min(int((conf->p_h_res()-2*70)*0.33), conf->p_v_res() - 200 - 100);
    max_width = conf->p_h_res() - cover_size - 2 * 55 - 3 * spacing - max_time_size;
  } else {
    name = get_name_from_file(dbaf);

    text_extra_spacing = 2 * spacing;

    max_width = conf->p_h_res() - 2 * 55 - 2 * spacing - max_time_size;
  }

  string_format::format_to_size(name, list_font, max_width + text_extra_spacing, true);
  pair<int, int> name_size = string_format::calculate_string_size(name, list_font);

  int x = 55 + spacing;

  TouchArea::callback_func func;

  // touch
  if (mode == ADD)
    func = boost::bind(&GraphicalAudio::find_element_and_do_action_add, this, dbaf);
  else
    func = boost::bind(&GraphicalAudio::find_element_and_do_action_playlist, this, dbaf);

  S_Touch::get_instance()->register_area(TouchArea(rect(x, y, max_width, list_font_height), 2, func));

  // marked position ?
  if (dbaf == position)
  {
    // selection bar is spacing/2 larger on both sides
    PObj *p = new PFObj(themes->general_marked, x - spacing / 2, y,
			max_width + max_time_size + spacing, list_font_height, 2, true);
    render->current.add(p);
  }

  if (isDirectory(dbaf.path))
    name.append("/");

  // are we in the playlist and is this the currently playing song?
  if (mode == PLAY && dbaf.id == audio_state->p->p_cur_nr().id && audio_state->p_playing())
    render->current.add(new TObj(name, list_font, x, y + (list_font_height - name_size.second) / 2,
                                 themes->audio_marked_font1, themes->audio_marked_font2,
                                 themes->audio_marked_font3, 3));
  else
    render->current.add(new TObj(name, list_font, x, y + (list_font_height - name_size.second) / 2,
				 themes->audio_font1, themes->audio_font2,
				 themes->audio_font3, 3));

  // print the songs length if applicable
  if (dbaf.length != 0) {
    char temp[512];
    snprintf(temp, 512, "%d:%02d", dbaf.length/60, dbaf.length%60);
    int x_size = string_format::calculate_string_width(temp, list_font);
    if (mode == PLAY && dbaf.id == audio_state->p->p_cur_nr().id && audio_state->p_playing())
      render->current.add(new TObj(temp, list_font, x + max_width + max_time_size - x_size,
				   y + (list_font_height - name_size.second) / 2,
				   themes->audio_marked_font1, themes->audio_marked_font2,
				   themes->audio_marked_font3, 3));
    else
      render->current.add(new TObj(temp, list_font, x + max_width + max_time_size - x_size,
				   y + (list_font_height - name_size.second) / 2,
				   themes->audio_font1, themes->audio_font2,
				   themes->audio_font3, 3));
  }

  // FIXME: inefficient
  int qpos = audio_state->queue_pos(dbaf);

  if (qpos != 0)
    render->current.add(new TObj(conv::itos(qpos), list_font, conf->p_h_res()-165, 
				 y + (list_font_height - name_size.second) / 2,
				 themes->audio_font1, themes->audio_font2,
				 themes->audio_font3, 3));
}

void GraphicalAudio::print_modifying(vector<Dbaudiofile>& cur_files)
{
  if (mode == PLAY || no_folders_in_dir(cur_files))
    print_tracklist(cur_files);
  else
    print_graphical(cur_files);
}

bool GraphicalAudio::no_folders_in_dir(const vector<Dbaudiofile>& cur_files)
{
  foreach (const Dbaudiofile& file, cur_files)
    if (file.type == "dir")
      return false;

  return true;
}

void GraphicalAudio::options_reloaded()
{
  extracted_all_files = false;
}

string GraphicalAudio::get_cover_from_dir(const string& dir)
{
  string pic = "";

  vector<string> pictures_found;

  MyPair filetype;

  // FIXME: is this right? Do we still use .front-cover.jpg? Or just legacy?

  // check for special cover first
  if (file_exists(dir + "/.front-cover.jpg")) {
    pic = dir + "/.front-cover.jpg";
  } else {
    libfs_set_folders_first(opts.dir_order());

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

string GraphicalAudio::get_first_cover_in_dir(const string& dir)
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

      libfs_set_folders_first(opts.dir_order());

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

void GraphicalAudio::print_pic_element(int cur_pos, int j, int& x, int& y, const Dbaudiofile& s,
				       int& y_pos_offset, double scale1, double scale2, const std::string& pic)
{
  int pic_width = static_cast<int>(image_width/scale1);
  int pic_height = static_cast<int>(image_height/scale1);
  int pic_width_all = image_width_all;
  int pic_height_all_eks_text = image_height_all_eks_text;

  int max_pic_width = static_cast<int>(pic_width * scale2);
  int max_pic_height = static_cast<int>(pic_height * scale2);

  if (cur_pos == j) {
    pic_width = max_pic_width;
    pic_height = max_pic_height;
    pic_width_all += pic_width - image_width;
    pic_height_all_eks_text += pic_height - image_height;
    x -= (pic_width - image_width)/2;
    y_pos_offset = (pic_height - image_height)/2;
    y -= y_pos_offset;
  }

  render->create_scaled_image_wrapper_upscaled(pic, max_pic_width, max_pic_height);
  // use pic with max_width and max_height and scale it in software
  PFObj *p = new PFObj(pic, x, y, max_pic_width, max_pic_height, pic_width, pic_height, 3);

  p->x = p->real_x = x + (pic_width_all - p->real_w)/2;
  p->y = p->real_y = y + (pic_height_all_eks_text - p->real_h)/2;

  p->real_x -= 1;
  p->real_y -= 1;

  if (cur_pos == j) {
    x += (pic_width - image_width)/2;
    y += y_pos_offset * 2; // shift text also
  }

  render->current.add(p);

  S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&GraphicalAudio::find_element_and_do_action_add, this, s)));

  string name = get_name_from_file(s);

  string name_first_part = name;
  string_format::format_to_size(name_first_part, normal_font, image_width_all-10, false);
  string name_second_part = name.substr(name_first_part.size());

  if (name_second_part.size() > 0 && name_second_part[0] != ' ') {
    // splitting words
    string::size_type last_whole_word = name_first_part.rfind(' ');
    if (last_whole_word != string::npos) {
      name_first_part = name_first_part.substr(0, last_whole_word);
      name_second_part = name.substr(name_first_part.size());
    }
  }
  string_format::format_to_size(name_second_part, normal_font, image_width_all-10, false);

  int x_size = string_format::calculate_string_width(name_first_part, normal_font);
  int x_size2 = string_format::calculate_string_width(name_second_part, normal_font);

  if (cur_pos == j) {
    render->current.add(new TObj(name_first_part, normal_font, x + (image_width_all-x_size)/2,
				   y+image_height_all_eks_text,
				   themes->audio_marked_font1, themes->audio_marked_font2,
				   themes->audio_marked_font3, 3));
    render->current.add(new TObj(name_second_part, normal_font,
				   x + (image_width_all-x_size2)/2,
				   y+image_height_all_eks_text+normal_font_height,
				   themes->audio_marked_font1, themes->audio_marked_font2,
				   themes->audio_marked_font3, 3));
    y -= y_pos_offset;
  } else {
    render->current.add(new TObj(name_first_part, normal_font, x + (image_width_all-x_size)/2,
				   y+image_height_all_eks_text,
				   themes->audio_font1, themes->audio_font2,
				   themes->audio_font3, 3));
    render->current.add(new TObj(name_second_part, normal_font,
				   x + (image_width_all-x_size2)/2,
				   y+image_height_all_eks_text+normal_font_height,
				   themes->audio_font1, themes->audio_font2,
				   themes->audio_font3, 3));
  }
}

void GraphicalAudio::find_element(const Dbaudiofile& audio_file)
{
  int p = 0;
  foreach (Dbaudiofile& f, *files) {
    if (f == audio_file) {
      set_position_int(p);
      break;
    } else
      ++p;
  }
}

void GraphicalAudio::find_element_and_do_action_playlist(const Dbaudiofile& audio_file)
{
  find_element(audio_file);

  if (search_mode) {
    input_master->got_touch_action();
    return;
  } else
    play_track();
}

void GraphicalAudio::find_element_and_do_action_add(const Dbaudiofile& audio_file)
{
  find_element(audio_file);

  if (search_mode) {
    input_master->got_touch_action();
    return;
  } else {
    if (vector_lookup(*files, position_int()).type == "dir")
      enter_dir();
    else
      add();
  }
}

void GraphicalAudio::print_graphical(vector<Dbaudiofile>& cur_files)
{
  render->prepare_new_image();

  render->current.add(new PObj(themes->audio_background, 0, 0, 0, SCALE_FULL));

  PObj *back = new PFObj(themes->startmenu_music_dir, 25, 10, header_box_size, header_box_size, 2, true);

  render->current.add(back);

  S_Touch::get_instance()->register_area(TouchArea(rect(back->x, back->y, back->w, back->h), back->layer, boost::bind(&GraphicalAudio::one_up, this)));

  if (themes->show_header) {

    string header = dgettext("mms-audio", "Audio Library");

    if (folders.size() > 1) {
      string top_folder = folders.top().first.front();

      if (top_folder[top_folder.size()-1] == '/')
	top_folder = top_folder.substr(0, top_folder.size()-1);

      assert(top_folder.rfind('/') != string::npos);
      header += " - " + string_format::convert(top_folder.substr(top_folder.rfind('/')+1));
      string_format::format_to_size(header, header_font, conf->p_h_res()-250, false);
    }

    TObj *t = new TObj(header, header_font, 25 + header_box_size + 10, 10 + (header_box_size - header_size.second)/2,
		       themes->audio_header_font1, themes->audio_header_font2,
		       themes->audio_header_font3, 2);

    render->current.add(t);

    S_Touch::get_instance()->register_area(TouchArea(rect(t->x, t->y, t->w, t->h), t->layer, boost::bind(&GraphicalAudio::toggle_playlist, this)));
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

    string s = dgettext("mms-audio", "Search: ");
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
  int screen_pos, start, pos_in_list, cur_rows, cur_image_height_all;

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
      screen_pos = pos_in_list;
      start = 0;
    } else if (pos_in_list >= cur_files.size() - (images_per_row+nr_in_last_row)) { // last two rows
      start = cur_files.size()-((cur_rows-1)*images_per_row+nr_in_last_row);
      screen_pos = pos_in_list-start;
    } else {
      screen_pos = pos_in_list%images_per_row+images_per_row;
      if (screen_pos == images_per_row-1)
	screen_pos = 2*images_per_row-1;
      start = pos_in_list-screen_pos;
    }
  } else {
    screen_pos = pos_in_list;
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

  for (int j = 0; j < cur_rows*images_per_row && j < cur_files.size(); ++j) {

    Dbaudiofile s = vector_lookup(cur_files, start);

    int y_pos_offset = 0;

    if (s.type == "dir") {

      string pic = get_cover(s);

      if (file_exists(pic)) {
        print_pic_element(screen_pos, j, x, y, s, y_pos_offset, 1., 1.35, pic);
      } else {
        string picture_path;
        if (screen_pos ==j) picture_path = render->default_path + themes->audio_dir_marked;
        else picture_path = render->default_path + themes->general_normal_dir;
        print_pic_element(screen_pos, j, x, y, s, y_pos_offset, 1.15, 1.4, picture_path);
      }

    } else {

      string picture_path = render->default_path + themes->audio_file;

      if (!s.is_fetched() && s.type != "web" && s.type != "media-file" && s.type != "media-track") {
        Dbaudiofile tmp_dbaf(s.path);
        tmp_dbaf.set_values(s);
        vector_lookup(cur_files, start) = tmp_dbaf;
        s = tmp_dbaf;
      }

      print_pic_element(screen_pos, j, x, y, s, y_pos_offset, 1.0, 1.4, picture_path);

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

  print_buttom(cur_files.size(), pos_in_list);
}

void GraphicalAudio::print_tracklist(vector<Dbaudiofile>& cur_files)
{
  render->prepare_new_image();

  render->current.add(new PObj(themes->audio_background, 0, 0, 0, SCALE_FULL));

  PObj *back = new PFObj(themes->startmenu_music_dir, 25, 10, header_box_size, header_box_size, 2, true);

  render->current.add(back);

  if (mode == ADD)
    S_Touch::get_instance()->register_area(TouchArea(rect(back->x, back->y, back->w, back->h), back->layer, boost::bind(&GraphicalAudio::one_up, this)));

  if (themes->show_header) {

    string header = dgettext("mms-audio", "Audio Library");

    if (mode == PLAY) {

      header = dgettext("mms-audio", "Audio - Playlist");

    } else if (folders.size() > 1) {

      string top_folder = folders.top().first.front();

      if (top_folder[top_folder.size()-1] == '/')
	top_folder = top_folder.substr(0, top_folder.size()-1);

      assert(top_folder.rfind('/') != string::npos);
      header += " - " + string_format::convert(top_folder.substr(top_folder.rfind('/')+1));
      string_format::format_to_size(header, header_font, conf->p_h_res()-250, false);
    }

    TObj *t = new TObj(header, header_font, 25 + header_box_size + 10, 10 + (header_box_size - header_size.second)/2,
		       themes->audio_header_font1, themes->audio_header_font2,
		       themes->audio_header_font3, 2);

    render->current.add(t);

    S_Touch::get_instance()->register_area(TouchArea(rect(t->x, t->y, t->w, t->h), t->layer, boost::bind(&GraphicalAudio::toggle_playlist, this)));
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

    string s = dgettext("mms-audio", "Search: ");
    int x_size = string_format::calculate_string_width(s, search_font);
    string_format::format_to_size(sw, search_font, max_x-x_size, true);

    int search_text_height = string_format::calculate_string_size(s + sw, search_font).second;

    render->current.add(new TObj(s + sw, search_font, 47 + search_size + 10,
				 y + (search_size + 5 - search_text_height)/2,
				 themes->search_font1, themes->search_font2, themes->search_font3, 3));

    if (mode != PLAY) {
      int search_select_height = string_format::calculate_string_size(search_depth, search_select_font).second;

      render->current.add(new TObj(search_depth, search_select_font, max_x,
				   y + (search_size + 5 - search_select_height)/2,
				   themes->search_font1, themes->search_font2, themes->search_font3, 3));
    }
  }

  string pic = "";
  if (position_int() < files->size())
    pic = get_cover(vector_lookup(*files, position_int()));

  int pos = 0;

#ifdef use_mouse

  if (audio_conf->p_display_buttons() && mode == PLAY) {

    int y_buttons = (conf->p_v_res()-7*40)/2;

    PObj *p = new PObj("buttons/PLAY.png", 20, y_buttons+0*40, 2, NOSCALING);
    render->current.add(p);
    S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&GraphicalAudio::play, this)));

    p = new PObj("buttons/PAUSE.png", 20, y_buttons+1*40, 2, NOSCALING);
    render->current.add(p);
    S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&GraphicalAudio::pause, this)));

    p = new PObj("buttons/STOP.png", 20, y_buttons+2*40, 2, NOSCALING);
    render->current.add(p);
    S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&GraphicalAudio::stop, this)));

    p = new PObj("buttons/PREV.png", 20, y_buttons+3*40, 2, NOSCALING);
    render->current.add(p);
    S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&GraphicalAudio::prev_track, this)));

    p = new PObj("buttons/NEXT.png", 20, y_buttons+4*40, 2, NOSCALING);
    render->current.add(p);
    S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&GraphicalAudio::next_track, this)));

    p = new PObj("buttons/REV.png", 20, y_buttons+5*40, 2, NOSCALING);
    render->current.add(p);
    S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&GraphicalAudio::fb, this)));

    p = new PObj("buttons/FF.png", 20, y_buttons+6*40, 2, NOSCALING);
    render->current.add(p);
    S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&GraphicalAudio::ff, this)));
  }
#endif

  if (cur_files.size() > 0) {

    pos = position_int();
    if (search_mode)
      pos = offset;

    Dbaudiofile currently_selected;
    if (pos != -1 && cur_files.size() > pos)
      currently_selected = vector_lookup(cur_files, pos);
    else
      pos = 0;

    print_range_modifying<Dbaudiofile>(cur_files, currently_selected, pos,
				       boost::bind(&GraphicalAudio::print_element, this, _1, _2, _3, !pic.empty()), list_font_height);

    if (mode == ADD && (search_depth == dgettext("mms-audio", "current folder") || !search_mode)) {

      if (!pic.empty()) {

	if (cur_files.size() > 0) {

	  Dbaudiofile dbaf = vector_lookup(cur_files, pos);

	  int max_y_pic = std::min(int((conf->p_h_res()-2*70)*0.33), conf->p_v_res() - 200 - 100);
	  int max_x_pic = max_y_pic;

	  string artist = string_format::convert(string_format::trim(dbaf.artist), true);
	  string album = string_format::convert(string_format::trim(dbaf.album), true);

	  string_format::format_to_size(artist, information_bigger_font, max_x_pic, false);
	  string_format::format_to_size(album, information_font, max_x_pic, false);

	  int y_artist = y_start - 10;

	  if (conf->p_v_res() == 405)
	    y_artist = 85;

	  int y_album = y_artist + extract_font_size(information_bigger_font)*2;
	  int y_image = y_album + (int)extract_font_size(information_font)*2.5;

	  if (search_mode) {
	    y_artist += search_top_size;
	    y_album += search_top_size;
	    y_image += search_top_size;
	  }

	  render->create_scaled_image_wrapper_upscaled(pic, max_x_pic, max_y_pic);

	  int x_start = conf->p_h_res() - max_x_pic - 70;

	  PFObj *p = new PFObj(pic, x_start, y_image, max_x_pic, max_y_pic, false, 3);

	  render->current.add(new TObj(artist, information_bigger_font, x_start, y_artist,
				       themes->audio_font1, themes->audio_font2,
				       themes->audio_font3, 3));

	  render->current.add(new TObj(album, information_font, x_start, y_album,
				       themes->audio_font1, themes->audio_font2,
				       themes->audio_font3, 3));

	  render->current.add(p);
	}
      }
    }
  }

  print_buttom(cur_files.size(), pos);
}

vector<Dbaudiofile> GraphicalAudio::add_dir(const string& dir, bool from_media)
{
  int nr_files = 0;
  for (file_iterator<file_t, default_order> i (dir); i != i.end (); ++i)
    if (!isDirectory(i->getName()) && check_type(i->getName(), audio_conf->p_filetypes_a()) != emptyMyPair)
      ++nr_files;

  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

  screen_updater->timer.deactivate("audio_fullscreen");

  screen_updater->status_progressbar = 0;
  screen_updater->total_progressbar = nr_files;
  screen_updater->header = dgettext("mms-audio", "Extracting metadata");

  screen_updater->timer.add(TimeElement("progressbar", &check_progressbar, &print_progressbar));

  vector<Dbaudiofile> cur_files;

  string cur_directory = "";
  string parent_dir = dir;
  vector<Dbaudiofile> files_in_dir;

  libfs_set_folders_first(opts.dir_order());

  for (file_iterator<file_t, default_order> i (dir); i != i.end (); ++i)
    {
      if (global->check_stop_bit()) {
	cur_files.clear();
	break;
      }

      string filename = filesystem::FExpand(i->getName());

      if (cur_directory != i.currentDirectory()) {
	cur_directory = i.currentDirectory();
	add_tracks_from_dir(files_in_dir, cur_files);
      }

      if (isDirectory(filename)) {
	insert_file_into_db(filename, cur_directory);
      } else {
	const MyPair filetype = check_type(filename, audio_conf->p_filetypes_a());

	if (filetype != emptyMyPair) {

	  insert_file_into_db(filename, cur_directory);

	  Dbaudiofile d(addsimplefile(filename, filetype, from_media));
	  d.db_id = db_id(filename, false);
	  db_mutex.enterMutex();
	  d.get_info_from_db(d.path, this);
	  db_mutex.leaveMutex();
	  files_in_dir.push_back(d);

	  ++screen_updater->status_progressbar;
	}
      }
    }

  // add tracks from the last directory
  add_tracks_from_dir(files_in_dir, cur_files);

  screen_updater->timer.del("progressbar");
  cleanup_progressbar();

  screen_updater->timer.activate("audio_fullscreen");

  return cur_files;
}

void GraphicalAudio::remove_track_from_playlist(const Dbaudiofile& p)
{
  assert(p.playlist_id != -1);

  db_mutex.enterMutex();

  char *tmp = sqlite3_mprintf("DELETE FROM Playlist WHERE id='%q'", conv::itos(p.playlist_id).c_str());
  db.execute(tmp);
  sqlite3_free(tmp);

  db_mutex.leaveMutex();
}

void GraphicalAudio::remove_all_tracks_from_playlist()
{
  db_mutex.enterMutex();
  db.execute("DELETE FROM Playlist");
  db_mutex.leaveMutex();
}

void GraphicalAudio::ids(Dbaudiofile& p)
{
  p.db_id = db_id(p.path, false);
}

void GraphicalAudio::add_track_to_playlist(Dbaudiofile& p)
{
  db_mutex.enterMutex();

  char *tmp = sqlite3_mprintf("INSERT INTO Playlist VALUES(NULL, '%q')", conv::itos(p.db_id).c_str());
  db.execute(tmp);
  sqlite3_free(tmp);

  p.playlist_id = db.last_index();

  db_mutex.leaveMutex();

  AudioTemplate<Dbaudiofile>::add_track_to_playlist(p);
}

void GraphicalAudio::new_playlist()
{
  db_mutex.enterMutex();
  db.execute("DELETE FROM Playlist");
  db_mutex.leaveMutex();
}

void GraphicalAudio::add_tracks_to_playlist(const std::vector<Dbaudiofile>& vp)
{
  db_mutex.enterMutex();

  db.execute("BEGIN TRANSACTION");

  foreach (Dbaudiofile p, vp) {
    char *tmp = sqlite3_mprintf("INSERT INTO Playlist VALUES(NULL, '%q')", conv::itos(p.db_id).c_str());
    db.execute(tmp);
    sqlite3_free(tmp);

    p.playlist_id = db.last_index();

    AudioTemplate<Dbaudiofile>::simple_add_track_to_playlist(p);
  }

  db.execute("END TRANSACTION");

  db_mutex.leaveMutex();

  // faster this way
  shuffle_list.randomize();

  save_playlist("last");
}

void GraphicalAudio::add_tracks_from_dir(vector<Dbaudiofile> &files_in_dir,
					 vector<Dbaudiofile> &cur_files)
{
  // check if all the files are missing the track tag
  bool missing_track = true;
  foreach (Dbaudiofile& file, files_in_dir)
    if (file.track != 0) {
      missing_track = false;
      break;
    }

  if (missing_track) {

    // just add all the tracks, we don't care about the order since its
    // probably ok already
    foreach (Dbaudiofile& file, files_in_dir)
      cur_files.push_back(file);

    files_in_dir.clear();

  } else {

    while (!files_in_dir.empty()) {
      // find the track with the smallest id
      int smallest_track = 1000000;
      vector<Dbaudiofile>::iterator element;

      for (vector<Dbaudiofile>::iterator fid = files_in_dir.begin(),
	     fid_end = files_in_dir.end(); fid != fid_end; ++fid)
	if (fid->track < smallest_track) {
	  smallest_track = fid->track;
	  element = fid;
	}

      assert(smallest_track != 1000000);
      cur_files.push_back(*element);
      files_in_dir.erase(element);
    }
  }
}

bool GraphicalAudio::rdir_internal(const string& filename, const string& argv,
				   vector<Dbaudiofile> &cur_files, bool in_db)
{
  if (global->check_stop_bit()) {
    cur_files.clear();
    return false;
  }

  if (isDirectory (filename))
    {
      if(!file_tools::dir_contains_known_files(filename, ext_mask, 5))
	return true;

      // dir
      Simplefile r;

      r.id = ++id;
      r.name = filename.substr(argv[argv.size()-1] == '/' ? argv.size() : argv.size() + 1);

      if (r.name[r.name.size()-1] == '/')
	r.name = r.name.substr(0, r.name.size()-1);

      r.name = string_format::convert(r.name);
      r.lowercase_name = string_format::lowercase(r.name);
      if (filename[filename.size()-1] != '/')
	r.path = filename + '/';
      else
	r.path = filename;
      r.type = "dir";
      Dbaudiofile d(r);
      if (in_db) {
	d.db_id = db_id(filename, true);
	assert(d.db_id != -1);
      }
      cur_files.push_back(d);
    } else {
      // file
      const MyPair filetype = check_type(filename, audio_conf->p_filetypes_a());
      if (filetype != emptyMyPair) {
	Dbaudiofile d(addsimplefile(filename, filetype, navigating_media));

	db_mutex.enterMutex();
	d.get_info_from_db(d.path, this);
	db_mutex.leaveMutex();

	if (in_db) {
	  d.db_id = db_id(filename, false);
	  assert(d.db_id != -1);
	}
	cur_files.push_back(d);
	++(S_ScreenUpdater::get_instance())->status_progressbar;
      }
    }

  return true;
}

vector<string> GraphicalAudio::check_db_for_folders(const string& parent)
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

void GraphicalAudio::insert_file_into_db(const string& filename, const string& parent)
{
  bool is_dir = isDirectory(filename);

  string good_filename;

  if (is_dir)
    good_filename = string_format::unique_folder_name(filename);
  else
    good_filename = filename;

  string::size_type i;
  if ((i = filename.rfind('/')) == string::npos)
    i = 0;
  string name = filename.substr((i != 0) ? i+1 : i);
  string path = filename.substr(0, i+1);

  if (is_dir)
    path = good_filename;

  db_mutex.enterMutex();

  // check if already exists
  SQLQuery *q = db.query("Folders", ("SELECT id FROM %t WHERE filename='" + string_format::escape_db_string(good_filename) + "'").c_str());
  if (q && q->numberOfTuples() > 0) {
    delete q;
    db_mutex.leaveMutex();
    // check for cover again
    insert_cover_into_db(path, is_dir ? "dir" : "file");
    return;
  } else
    delete q;

  string parent_id = get_parent_id(parent, db, navigating_media ? top_media_folders : audio_folders);

  if (!is_dir && (i = name.rfind('.')) != string::npos)
    name = name.substr(0, i);

  char *tmp_insert = sqlite3_mprintf("INSERT INTO Folders VALUES(NULL, '%q', '%q', '%q', '%q', '%q')",
				     parent_id.c_str(), good_filename.c_str(), name.c_str(),
				     string_format::lowercase(name).c_str(), conv::itos(is_dir).c_str());
  db.execute(tmp_insert);
  sqlite3_free(tmp_insert);
  db_mutex.leaveMutex();

  // and cover
  insert_cover_into_db(path, is_dir ? "dir" : "file");
}

void GraphicalAudio::insert_cover_into_db(const string& filename, const string& type)
{
  bool found_cover = false;
  bool was_empty = false;

  db_mutex.enterMutex();

  SQLQuery *q2 = db.query("Covers", ("SELECT Cover FROM %t WHERE Path='" + string_format::escape_db_string(filename) + "'").c_str());
  if (q2 && q2->numberOfTuples() > 0) {
    if ((*q2)[0]["Cover"] != "")
      found_cover = true;
    else
      was_empty = true;
  }
  delete q2;

  db_mutex.leaveMutex();

  if (!found_cover) {

    string cover = "";

    if (type == "file") {
      GSimplefile s;
      s.path = filename;
      cover = s.find_cover_in_current_dir();
    } else if (type == "media-track") {
      recurse_mkdir(conf->p_var_data_dir(),"cddb_data", &cover);
      cover += filename;
      cover += ".jpg";

      // check if the cover even exists before inserting it
      if (!file_exists(cover))
	cover = "";

    } else if (type == "dir") {
      cover = get_first_cover_in_dir(filename);
    } else {
      assert(false);
    }

    db_mutex.enterMutex();

    char *tmp_insert_2;

    if (was_empty)
      tmp_insert_2 = sqlite3_mprintf("UPDATE Covers SET Cover = '%q' WHERE Path = '%q'",
					   cover.c_str(), filename.c_str());
    else
      tmp_insert_2 = sqlite3_mprintf("INSERT INTO Covers VALUES('%q', '%q')",
					   filename.c_str(), cover.c_str());
    db.execute(tmp_insert_2);
    sqlite3_free(tmp_insert_2);

    db_mutex.leaveMutex();
  }
}

vector<Dbaudiofile> GraphicalAudio::rdir_hd(const string& argv, bool insert_into_db)
{
  libfs_set_folders_first(opts.dir_order());

  if (insert_into_db) {

    bool inserted = false;

    for (file_iterator<file_t, default_order> i (argv); i != i.end (); i.advance(false)) {
      string filename = filesystem::FExpand(i->getName());
      if (isDirectory(filename) || check_type(filename, audio_conf->p_filetypes_a()) != emptyMyPair) {
	insert_file_into_db(filename, argv);
	inserted = true;
      }
    }

    if (inserted)
      return rdir(argv);
    else {
      vector<Dbaudiofile> cur_files;
      return cur_files;
    }

  } else {

    vector<Dbaudiofile> cur_files;

    for (file_iterator<file_t, default_order> i (argv); i != i.end (); i.advance(false)) {
      string filename = filesystem::FExpand(i->getName());
      if (!rdir_internal(filename, argv, cur_files, false))
	break;
    }

    return cur_files;
  }
}

vector<Dbaudiofile> GraphicalAudio::rdir(const string& argv)
{
  vector<Dbaudiofile> cur_files;

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

void GraphicalAudio::read_dirs()
{
  while (!all_metadata_files.empty())
    all_metadata_files.pop();

  vector<Dbaudiofile> playlist_new;

  int nr_files = 0;

  foreach (string& folder, audio_folders)
    for (file_iterator<file_t, default_order> i (folder); i != i.end (); i.advance(false))
      if (!isDirectory (i->getName()) &&
	  check_type(i->getName(), audio_conf->p_filetypes_a()) != emptyMyPair)
	++nr_files;

  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

  screen_updater->status_progressbar = 0;
  screen_updater->total_progressbar = nr_files;
  screen_updater->header = dgettext("mms-audio", "Extracting metadata");

  screen_updater->timer.add(TimeElement("progressbar", &check_progressbar, &print_progressbar));

  // process audio dir
  audio_list = parse_dir(audio_folders);

  screen_updater->timer.del("progressbar");
  cleanup_progressbar();

  if (audio_folders.size() >= 1)
    std::sort(audio_list.begin(), audio_list.end(), file_sort());

  if (audio_list.size() != 0)
    // set position to the first file
    folders.top().second = 0;

  if (mode == ADD)
    files = &audio_list;
  else
    files = &playlist;

  S_BackgroundUpdater::get_instance()->run_once(boost::bind(&GraphicalAudio::check_for_changes, this));
}

Simplefile GraphicalAudio::intelligent_random_next()
{
#ifdef use_imms
  db_mutex.enterMutex();

  imms->playlist_changed(playlist.size());
  next_track_pos = -1;
  //  imms->select_next();

  int time_left = audio_state->p->p_total_time() - audio_state->p->p_cur_time();

  if (time_left < 3)
    time_left = 0;

  bool forced = time_left;

  Simplefile cur_nr = audio_state->p->p_cur_nr();

  if (imms_cur_path != cur_nr.path) {
    // something strange happened, we are not playing the track we IMMS thinks
    // we are
    for (int i = 0, end = playlist.size(); i < end; ++i)
      if (vector_lookup(playlist, i).id == cur_nr.id) {
	imms->start_song(i, cur_nr.path);
	break;
      }
  }

  imms->end_song(!time_left, forced, false);

  imms->select_next();

  int i = 0;

  while (next_track_pos == -1) {
    if (i > 20) {
      std::cerr << dgettext("mms-audio", "something wrong in intelligent random") << std::endl;
      db_mutex.leaveMutex();
      Simplefile s;
      return s;
    }

    mmsUsleep(50*1000); // wait for next track
    ++i;
  }

  Simplefile next_file = vector_lookup(playlist, next_track_pos);

  imms_cur_path = next_file.path;

  imms->start_song(next_track_pos, imms_cur_path);

  audio_state->direction = Audio_s::ADDED;

  db_mutex.leaveMutex();

  return next_file;
#else
  // bogus to make compiler happy
  Simplefile s;
  return s;
#endif
}

void GraphicalAudio::intelligent_random_prev()
{
#ifdef use_imms
  intelligent_random_next();
#endif
}

void GraphicalAudio::extract_metadata()
{
  if (!extracted_all_files && (conf->p_idle_time() > 0) &&
      (opts.metadata_extract() == dgettext("mms-audio", "yes"))) {

    int nr_extracts = 3;

    // check if we are idle
    int idle_time = time(0)-conf->p_last_key();
    if (idle_time < 3)
      return;
    else if (idle_time > 60 * conf->p_idle_time())
      nr_extracts = 4;

    if (all_metadata_files.empty()) {
      list<string> dirs = audio_folders;
      libfs_set_folders_first(opts.dir_order());
      foreach (string& dir, dirs)
	for (file_iterator<file_t, default_order> i (dir); i != i.end (); ++i)
	  all_metadata_files.push(i->getName());
      // optimization, we don't want to both make the list and start extracting
      return;
    }

    while (!all_metadata_files.empty()) {
      if (!file_exists_in_db(all_metadata_files.top())) {
	Dbaudiofile(all_metadata_files.top());
	if (nr_extracts == 0)
	  return;
	else
	  --nr_extracts;
      }
      all_metadata_files.pop();
    }

    extracted_all_files = true;
  }
}

void GraphicalAudio::startup_updater()
{
  Audio::startup_updater();

  S_BackgroundUpdater::get_instance()->timer.add(TimeElement("search marker", boost::bind(&GraphicalAudio::check_metadata,
											  this),
							     boost::bind(&GraphicalAudio::extract_metadata, this)));

  S_Search::get_instance()->register_module(SearchModule(dgettext("mms-audio", "Audio"),
							 boost::bind(&GraphicalAudio::gen_search_list, this, _1),
							 boost::bind(&GraphicalAudio::reenter, this,  _1)));
}

void GraphicalAudio::audio_screensaver_next_helper(int x_pos, int& y)
{
  x_pos = 60;

  y += static_cast<int>(0.65 * screensaver_normal_font_height);

  string next_line;

  if (opts.shuffle() != dgettext("mms-audio", "completely random")) {
    Simplefile sf = next_helper(false);
    Dbaudiofile next_track = Dbaudiofile(sf.path);

    if (audio_state->p->p_cur_nr().type != "web") {
      if (sf.type == "media-track" || (next_track.artist.empty() && next_track.title.empty()))
	next_line = dgettext("mms-audio", "Next track: ") + string_format::convert(sf.name, true);
      else {
	next_line = dgettext("mms-audio", "Next track: ") + string_format::convert(next_track.artist, true) + " - " + string_format::convert(next_track.title, true);
      }
    }

  } else {
    next_line = dgettext("mms-audio", "Next track: random");
  }

  string_format::format_to_size(next_line, screensaver_normal_font, conf->p_h_res()-x_pos-30, true);
  audio_state->playback_fullscreen.add(new TObj(next_line, screensaver_normal_font, x_pos, y,
						themes->audio_playback_font1, themes->audio_playback_font2,
						themes->audio_playback_font3, 1));
  y += screensaver_normal_font_height;
}
