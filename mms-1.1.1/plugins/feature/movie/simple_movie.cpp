//#define DLOG

#include "simple_movie.hpp"

#include "common-feature.hpp"

#include "startmenu.hpp"
#include "graphics.hpp"
#include "extra_menu.hpp"
#include "busy_indicator.hpp"
#include "shutdown.hpp"

#include "boost.hpp"

#include "libfspp.hpp"

// ostringstream
#include <sstream>

using std::string;
using std::vector;
using std::list;

// commands
void SimpleMovie::secondary_menu()
{
  ExtraMenu em;

  em.add_item(ExtraMenuItem(dgettext("mms-movie", "Play video"), input_master->find_shortcut("action"),
			    boost::bind(&SimpleMovie::action_play, this)));

  if (vector_lookup(files, folders.top().second).type != "file")
    em.add_item(ExtraMenuItem(dgettext("mms-movie", "Enter directory"), input_master->find_shortcut("right"),
			      boost::bind(&SimpleMovie::enter_dir ,this)));

  if (folders.size() > 1)
    em.add_item(ExtraMenuItem(dgettext("mms-movie", "Go up one directory"), input_master->find_shortcut("left"),
			      boost::bind(&SimpleMovie::go_back ,this)));

  em.add_item(ExtraMenuItem(dgettext("mms-movie", "Return to startmenu"), input_master->find_shortcut("back"),
			    boost::bind(&SimpleMovie::exit ,this)));

  add_standard(em);

  foreach (ExtraMenuItem& item, global->menu_items)
    em.add_persistent_item(item);

  conf->s_sam(true);

  em.mainloop();
}

void SimpleMovie::go_back()
{
  if (!(folders.size() == 1)) {
    folders.pop();
    load_current_dirs();
  }
}

bool SimpleMovie::reload_dir(const string& path)
{
  return false;
}

void SimpleMovie::reload_current_dirs()
{
  load_current_dirs();
}

void SimpleMovie::enter_dir()
{
  if (vector_lookup(files, folders.top().second).type != "file") {
    string temp_cur_dir = vector_lookup(files ,folders.top().second).path;
    vector<Multifile> templist = rdir(temp_cur_dir);

    if (templist.size() != 0) {
      list<string> templs; templs.push_back(temp_cur_dir + "/");
      folders.push(std::make_pair(templs, 0));
      files = templist;
    } else {
      dprintf("Simplemovie says Folder is empty\n");
      DialogWaitPrint pdialog(dgettext("mms-movie", "Folder is empty"), 1000);
    }
  }
}

void SimpleMovie::search_func()
{
  S_BusyIndicator::get_instance()->idle();

  input_master->search<Multifile>(files, &folders.top().second,
				  boost::bind(&SimpleMovie::print, this, _1),
				  boost::bind(&SimpleMovie::search_compare, this, _1),
				  boost::bind(&SimpleMovie::get_name_from_file, this, _1),
				  search_mode, search_str, lowercase_search_str, offset);
}

void SimpleMovie::page_up()
{
  int jump = conf->p_jump();

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

void SimpleMovie::page_down()
{
  int jump = conf->p_jump();

  if (files.size() > jump) {
    if (folders.top().second > (files.size() - jump) && folders.top().second != (files.size()-1))
      folders.top().second = files.size()-1;
    else
      folders.top().second = (folders.top().second+jump)%files.size();
  }
}

string SimpleMovie::mainloop()
{
  visible = true;

  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();
  Shutdown *sd = S_Shutdown::get_instance();

  Input input;

  bool update_needed = true;

  while (!exit_loop)
    {
      if (update_needed) {
	print(files);
	print_lcd_menu();
      }

      input = input_master->get_input_busy_wrapped(busy_indicator);

      if (sd->is_enabled()) {
	sd->cancel();
	continue;
      }

      update_needed = true;

      if (input.command == "prev")
 	{
	  if (folders.top().second != 0)
	    folders.top().second -= 1;
	  else
	    folders.top().second = files.size() - 1;
 	}
      else if (input.command == "next")
 	{
	  folders.top().second = (folders.top().second+1)%files.size();
 	}
      else if (input.command == "left")
	{
	  go_back();
	}
      else if (input.command == "right")
	{
	  enter_dir();
	}
      else if (input.command == "search")
	{
	  search_func();
	}
      else if (input.command == "back")
	{
	  exit();
	}
      else if (input.command == "action")
	{
	  action_play();
	}
      else if (input.command == "second_action")
	{
	  secondary_menu();
        }
      else
	MovieTemplate<Multifile>::movie_mainloop_common(input);

      update_needed = !global->check_commands(input);
    }

  exit_loop = false;

  visible = false;

  return "";
}

// print a video element
void SimpleMovie::print_movie_element(const Multifile& r, const Multifile& position, int y)
{
  string name = r.name;

  if (r.type != "file")
    name += "/";

  string_format::format_to_size(name, list_font, conf->p_h_res()-75-155, true);

  if (r == position)
    render->current.add(new PFObj(themes->general_marked_large, 60, y+2,
				  conf->p_h_res()-2*60, list_font_height, 2, true));

  std::ostringstream out; out << r.filenames.size();

  render->current.add(new TObj(name, list_font, 75, y,
			       themes->movie_font1, themes->movie_font2,
			       themes->movie_font3, 3));
  render->current.add(new TObj(r.filetype, list_font, conf->p_h_res()-145, y,
			       themes->movie_font1, themes->movie_font2,
			       themes->movie_font3, 3));
  render->current.add(new TObj(out.str(), list_font, conf->p_h_res()-85, y,
			       themes->movie_font1, themes->movie_font2,
			       themes->movie_font3, 3));
}

void SimpleMovie::print(const vector<Multifile>& cur_files)
{
  render->prepare_new_image();

  render->current.add(new PObj(themes->movie_background, 0, 0, 0, SCALE_FULL));

  std::pair<int, int> header_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
  int header_box_size = static_cast<int>(header_size.second * 0.75);

  render->current.add(new PFObj(themes->startmenu_movie_dir, 25, 10, header_box_size, header_box_size, 2, true));

  if (themes->show_header) {

    string header = dgettext("mms-movie", "Videos");

    if (folders.size() > 1) {
      string top_folder = folders.top().first.front();

      if (top_folder[top_folder.size()-1] == '/')
	top_folder = top_folder.substr(0, top_folder.size()-1);

      assert(top_folder.rfind('/') != string::npos);
      header += " - " + top_folder.substr(top_folder.rfind('/')+1);
      string_format::format_to_size(header, header_font, conf->p_h_res()-220, false);
    }

    std::pair<int, int> header_size = string_format::calculate_string_size(header, header_font);

    render->current.add(new TObj(header, header_font, 100, (70-header_size.second)/2,
				 themes->movie_header_font1, themes->movie_header_font2,
				 themes->movie_header_font3, 2));
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
  }

  if (cur_files.size() > 0) {
    int pos = folders.top().second;
    if (search_mode && cur_files.size() > 0)
        pos = offset % cur_files.size();

    print_range<Multifile>(cur_files, vector_lookup(cur_files, pos), pos,
			   boost::bind(&SimpleMovie::print_movie_element, this, _1, _2, _3), list_font_height);
  }

  std::ostringstream pos;

  if (search_mode)
    if (cur_files.size() > 0)
      pos << (offset % cur_files.size()) + 1 << "/" << cur_files.size();
    else
      pos << "";
  else
    pos << folders.top().second+1 << "/" << cur_files.size();

  int x = string_format::calculate_string_width(pos.str(), position_font);

  render->current.add(new TObj(pos.str(), position_font, conf->p_h_res()-(60+x), 20,
			       themes->movie_font1, themes->movie_font2,
			       themes->movie_font3, 3));

  render->draw_and_release("movie");
}

vector<Multifile> SimpleMovie::parse_dir(const list<string>& dirs)
{
  vector<Multifile> vec_files;

  foreach (const string& dir, dirs) {
    vector<Multifile> tempfiles = rdir(dir);
    vec_files.insert(vec_files.end(), tempfiles.begin(), tempfiles.end());
  }

  return vec_files;
}

// read a dir and spit out a list of files and dirs. For movies.
vector<Multifile> SimpleMovie::rdir(const string& argv)
{
  vector<Multifile> cur_files;

  libfs_set_folders_first(opts.dir_order());

  for (file_iterator<file_t, default_order> i (argv); i != i.end (); i.advance(false))
    {
      if (global->check_stop_bit()) {
	cur_files.clear();
	break;
      }

      string filename = filesystem::FExpand(i->getName());

      if (isDirectory (filename)) { // dir
	Multifile dir = add_dir(filename);
	if ((dir.filenames).size() != 0 || dir.filetype == "dir")
	  cur_files.push_back(dir);
      } else { // file
	MyPair filetype = check_type(filename, movie_conf->p_filetypes_m());
	if (filetype != emptyMyPair)
	  cur_files.push_back(addfile(filename, filetype));
      }
    }
  return cur_files;
}
