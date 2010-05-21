#include "config.h"

#include "libfspp.hpp"

#include "module.hpp"

#include "input.hpp"
#include "config.hpp"
#include "common.hpp"
#include "renderer.hpp"
#include "text_object.hpp"
#include "common-feature.hpp"

#include "updater.hpp"

#include "boost.hpp"

#include <iostream>

using std::vector;
using std::list;
using std::string;

Module::Module()
  : search_mode(false), search_str(""), offset(0),
    search_help_offset(-1), search_need_cleanup(false),
    search_top_size(35), search_depth(gettext("current folder")),
    overlay_search("search marker"), id(1), visible(true),
    loaded_correctly(false), reload_needed(false)
{
  input_master = S_InputMaster::get_instance();
  themes = S_Themes::get_instance();
  render = S_Render::get_instance();
  conf = S_Config::get_instance();
#ifdef use_notify_area
  nArea = S_NotifyArea::get_instance();
#endif
  global = S_Global::get_instance();
  if (conf->p_media())
    cd = S_Cd::get_instance();

  int size = 55;

  string all_chars = global->get_valid_keys();

  letters.clear();

  string spaced_keys = "";
  vector<int> group_start;

  string font = "Vera/22";
  int start_pos = 85;

  if (conf->p_h_res() <= 800) {
    font = "Vera/16";
    start_pos = 15;
  }

  for (int i = 0; i < all_chars.size(); ++i) {
    if (all_chars[i] != '|') {
      spaced_keys += (all_chars.substr(i,1) + " ");
    }

    if (all_chars[i] == '|' || i == all_chars.size()-1) {

      spaced_keys = spaced_keys.substr(0, spaced_keys.size()-1);

      int letters_width = string_format::calculate_string_width(spaced_keys, font);

      spaced_keys = "";
      start_pos += 5 + letters_width;

      group_start.push_back(start_pos);
    }
  }

  int group_pos = 0;

  for (int i = 0; i < all_chars.size(); ++i) {
    string cur_char = all_chars.substr(i, 1);
    if (cur_char != "|") {
      letters.push_back(size);
      int letter_space = string_format::calculate_string_width(cur_char, font);
      size += (letter_space-11);
    } else {
      size = vector_lookup(group_start, group_pos);
      ++group_pos;
    }
  }
}

bool Module::init()
{
  return true;
}

void Module::folder_difference(const vector<string>& new_list,
			       const vector<string>& old_list,
			       vector<string>& new_files,
			       vector<string>& removed_files)
{
  // find what is new
  foreach (const string& new_item, new_list) {
    bool found_match = false;
    foreach (const string& old_item, old_list) {
      if (new_item == old_item) {
	found_match = true;
	break;
      }
    }
    if (!found_match)
      new_files.push_back(new_item);
  }

  // ... and what is removed
  foreach (const string& old_item, old_list) {
    bool found_match = false;
    foreach (const string& new_item, new_list) {
      if (old_item == new_item) {
	found_match = true;
	break;
      }
    }
    if (!found_match)
      removed_files.push_back(old_item);
  }
}

int Module::check_metadata()
{
  return 3000; // ms
}

int Module::check_search_letter()
{
  return 200; // ms
}

void Module::print_marked_search_letter()
{
  if (!list_contains(conf->p_input(), string("lirc")) && !list_contains(conf->p_input(), string("evdev")))
    return;

  if (search_mode) {

    search_need_cleanup = true;

    render->wait_and_aquire();

    string header_font = graphics::resolution_dependant_font_wrapper(28, conf);
    std::pair<int, int> header_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);

    int y_offset = static_cast<int>(header_size.second * 0.75) + search_top_size;

    if (overlay_search.elements.size() > 0)
      overlay_search.partial_cleanup();

    int marked_thickness = 28;
    int marked_height = 38;
    int magic_offset = -8;
    if (conf->p_h_res() <= 800) {
      marked_thickness = 22;
      marked_height = 30;
      magic_offset = -10;
    }

    magic_offset = 0;

    if (time(0)-conf->p_last_key() <= 2 && search_help_offset != -1)
      overlay_search.add(new RObj(vector_lookup(letters, search_help_offset)+magic_offset, y_offset+5,
				  marked_thickness, marked_height, 160, 150, 90, 255, 4));

    string keys = global->get_valid_keys();
    string spaced_keys = "";

    int start_pos = 85;

    if (conf->p_h_res() <= 800)
      start_pos = 15;

    for (int i = 0; i < keys.size(); ++i) {
      if (keys[i] != '|')
	spaced_keys += (keys.substr(i,1) + " ");

      if (keys[i] == '|' || i == keys.size()-1) {

	spaced_keys = spaced_keys.substr(0, spaced_keys.size()-1);

	string font = "Vera/22";

	if (conf->p_h_res() <= 800)
	  font = "Vera/16";

	int letters_width = string_format::calculate_string_width(spaced_keys, font);

	overlay_search.add(new RObj(start_pos-4, y_offset+5, letters_width, marked_height,
				    20, 30, 20, 255, 3));

	overlay_search.add(new TObj(spaced_keys, font,
				    start_pos, y_offset, themes->audio_font1,
				    themes->audio_font2, themes->audio_font3, 5));

	spaced_keys = "";
	start_pos = start_pos + 5 + letters_width;
      }
    }

    render->draw_and_release("search marker", true);
  }
}

void Module::clean_up_search_print()
{
  if (search_need_cleanup) {
    render->wait_and_aquire();
    overlay_search.cleanup();
    render->image_mut.leaveMutex();
    search_need_cleanup = false;
  }
}

void Module::force_search_print_cleanup()
{
  render->wait_and_aquire();
  overlay_search.cleanup();
  render->image_mut.leaveMutex();
}

void Module::print_range_top(int int_position, int list_size, int element_size,
			     int& y, int& start, int& range)
{
  string header_font = graphics::resolution_dependant_font_wrapper(28, conf);
  std::pair<int, int> header_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);

  y = 10 * 2 + header_size.second;

  if (search_mode)
    y += search_top_size + static_cast<int>((conf->p_v_res()*1.0/600)*25);

  string button_playback_font = graphics::resolution_dependant_font_wrapper(17, conf);
  int button_playback_height = graphics::calculate_font_height(button_playback_font, conf);

  int bottom = static_cast<int>(2 * button_playback_height * 0.85);

  range = (conf->p_v_res()-y-bottom-25)/element_size;

  int width = conf->p_h_res()-2*55;
  int height = range*element_size+25;

  render->current.add(new RObj(55, y-15, width, height,
			       themes->general_rectangle_color1, themes->general_rectangle_color2,
			       themes->general_rectangle_color3, themes->general_rectangle_color4, 1));

  start = int_position - (range/2);

  // special cases
  if (range < list_size) {
    if (int_position >= list_size - (range/2)) // end
      start = list_size - range;
    else if ((range/2) >= int_position) // beginning
      start = 0;
  } else { // playlist smaller than range
    start = 0;
    range = list_size;
  }
}

// ====================================================================
//
// dir
//
// ====================================================================

// Add a file to the list
Simplefile Module::addsimplefile(const string& name, const MyPair& filetype, bool from_media)
{
  Simplefile r;
  r.id = ++id;
  r.path = name;

  string::size_type pos = name.rfind("/");
  if (pos != string::npos)
    r.name = name.substr(pos+1, name.size()-((pos+1)+(filetype.first.size()+1)));
  else
    r.name = name.substr(0, name.size()-(filetype.first.size()+1));

  if (conf->p_convert())
    r.name = string_format::convert(r.name);

  r.lowercase_name = string_format::lowercase(r.name);

  if (from_media)
    r.type = "media-file";
  else
    r.type = filetype.second;

  return r;
}

// database stuff

string Module::get_parent_id_movie(const string& parent, SQLDatabase& db, const std::list<string>& top_folders)
{
  return get_parent_id_common(parent, db, top_folders,
			      "INSERT INTO Folders VALUES(NULL, '%q', '%q', '%q', '%q', '0', '%q')");
}

string Module::get_parent_id(const string& parent, SQLDatabase& db, const std::list<string>& top_folders)
{
  return get_parent_id_common(parent, db, top_folders,
			      "INSERT INTO Folders VALUES(NULL, '%q', '%q', '%q', '%q', '%q')");
}

string Module::get_parent_id_common(const string& parent, SQLDatabase& db,
				    const std::list<string>& top_folders, const string& db_insert_string)
{
  string parent_id = "";
  string good_parent = string_format::unique_folder_name(parent);

  SQLQuery *q = db.query("Folders", ("SELECT id FROM %t WHERE filename='" + string_format::escape_db_string(good_parent) + "'").c_str());
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
    string parents_parent = string_format::unique_folder_name(par.substr(0, par.rfind('/')));

    bool is_parent_top_folder = false;

    foreach (const string& dir, top_folders)
      if (dir == good_parent)
	is_parent_top_folder = true;

    string insert_parent_id = "0";

    if (!is_parent_top_folder)
      insert_parent_id = get_parent_id_common(parents_parent, db, top_folders, db_insert_string);

    //The last field in the Folders table ( is_folder ) is an integer. Is there any reason to add a String instead of an Integer ?
    char *tmp = sqlite3_mprintf(db_insert_string.c_str(),
				insert_parent_id.c_str(), good_parent.c_str(), name.c_str(),
				string_format::lowercase(name).c_str(), "1");
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

  return parent_id;
}
