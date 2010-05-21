#include "config.h"

#include "libfspp.hpp"

#include "simple_audio.hpp"

#include "graphics.hpp"

using std::string;
using std::vector;
using std::list;

void SimpleAudio::reparse_file(const std::string& path)
{}

bool SimpleAudio::reload_dir(const string& dir)
{
  return false;
}

void SimpleAudio::reload_current_dirs()
{
  load_current_dirs();
}

void SimpleAudio::page_up()
{
  int jump = conf->p_jump();

  if (files->size() > jump) {
    int pos = position_int()-jump;
    if (position_int() == 0)
      set_position_int(files->size()-1+pos);
    else if (pos < 0)
      set_position_int(0);
    else
      set_position_int(pos);
  }
}

void SimpleAudio::page_down()
{
  int jump = conf->p_jump();

  if (files->size() > jump) {
    if (position_int() > (files->size() - jump) && position_int() != (files->size()-1))
      set_position_int(files->size()-1);
    else
      set_position_int((position_int()+jump)%files->size());
  }
}

string SimpleAudio::mainloop(AudioMode new_mode)
{
  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();
  Shutdown *sd = S_Shutdown::get_instance();

  visible = true;

  mainloop_top(new_mode);

  Input input;

  bool update_needed = true;

  while (!exit_loop)
    {
      if (update_needed) {
	print(*files);
	print_lcd_menu();
      }

      input = input_master->get_input_busy_wrapped(busy_indicator);

      if (sd->is_enabled()) {
	sd->cancel();
	continue;
      }

      update_needed = true;

      // general
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
	  exit();
	}
      else if (input.command == "page_up")
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
      // audio-add
      else if (mode == ADD) {

	if (input.command == "action")
	  {
	    add();
	  }
	else if (input.command == "right")
	  {
	    enter_dir();
	  }
	else if (input.command == "left")
	  {
	    leave_dir();
	  }
	else if (input.command == "add_all")
	  {
	    add_all();
	  }
	else if (input.command == "play_now")
	  {
	    if (vector_lookup(*files, position_int()).type != "dir")
		play_track_now();
	  }
	else if (input.command == "startmenu")
	  {
	    exit();
	  }
      }

      update_needed = !global->check_commands(input);
    }

  exit_loop = false;

  if (in_playlist)
    in_playlist = false;

  visible = false;

  return "";
}

void SimpleAudio::print_audiotrack_helper(int &x_pos)
{
}

void SimpleAudio::get_audiotrack_info(string& buffer, string& artist, string& album, string& title)
{
  if (audio_state->p->p_cur_nr().type == "web")
    buffer = format_time(audio_state->p->p_cur_time());
  else
    buffer = format_time(audio_state->p->p_cur_time(), audio_state->p->p_total_time());
  artist = audio_state->p->p_artist();
  album = audio_state->p->p_album();
  title = audio_state->p->p_title();
}

void SimpleAudio::secondary_menu_in_add(ExtraMenu &em)
{
  bool is_dir = (vector_lookup(*files, position_int()).type == "dir");

  std::string t = dgettext("mms-audio", "Add track to playlist");

  if (is_dir)
    t = dgettext("mms-audio", "Add directory to playlist");

  em.add_item(ExtraMenuItem(t, input_master->find_shortcut("action"),
			    boost::bind(&SimpleAudio::add, this)));

  if (is_dir)
    em.add_item(ExtraMenuItem(dgettext("mms-audio", "Enter directory"),
			      input_master->find_shortcut("right"),
			      boost::bind(&SimpleAudio::enter_dir, this)));

  if (folders.size() > 1)
    em.add_item(ExtraMenuItem(dgettext("mms-audio", "Go up one directory"),
			      input_master->find_shortcut("left"),
			      boost::bind(&SimpleAudio::leave_dir, this)));
}

void SimpleAudio::enter_dir()
{
  if (vector_lookup(*files, position_int()).type == "dir") {
    string temp_cur_dir = vector_lookup(*files, position_int()).path;

    vector<Simplefile> templist = rdir(temp_cur_dir);

    if (templist.size() != 0) {
      list<string> templs; templs.push_back(temp_cur_dir + "/");
      folders.push(std::make_pair(templs, 0));
      audio_list = templist;
      files = &audio_list;
    } else
      DialogWaitPrint pdialog(dgettext("mms-audio", "Folder is empty"), 1000);
  }
}

string SimpleAudio::get_name_from_file(const Simplefile& s)
{
  return s.to_string();
}

string SimpleAudio::get_short_name_from_file(const Simplefile& s)
{
  return get_name_from_file(s);
}

void SimpleAudio::print_element(const Simplefile& r, const Simplefile& position, int y)
{
  string name = r.name;
  string_format::format_to_size(name, normal_font, conf->p_h_res()-(75+85+10), true);

  if (r == position)
    render->current.add(new PFObj(themes->general_marked_large, 70, y,
				  conf->p_h_res()-75-70, list_font_height, 2, true));

  if (r == audio_state->p->p_cur_nr() && mode == PLAY && audio_state->p_playing()) {
    render->current.add(new TObj(name, normal_font, 75, y,
				 themes->audio_marked_font1, themes->audio_marked_font2,
				 themes->audio_marked_font3, 3));
  } else {

    if (isDirectory(r.path))
      name.append("/");

    render->current.add(new TObj(name, normal_font, 75, y,
				 themes->audio_font1, themes->audio_font2,
				 themes->audio_font3, 3));
  }

  // FIXME: inefficient
  int qpos = audio_state->queue_pos(r);

  if (qpos != 0)
    render->current.add(new TObj(conv::itos(qpos), normal_font, conf->p_h_res()-85, y,
				 themes->audio_font1, themes->audio_font2,
				 themes->audio_font3, 3));
}

void SimpleAudio::print(const vector<Simplefile>& cur_files)
{
  print_top();

  int pos = position_int();
  if (search_mode && cur_files.size() > 0)
    pos = offset % cur_files.size();

  if (cur_files.size() > 0)
    print_range<Simplefile>(cur_files, vector_lookup(cur_files, pos), pos,
			    boost::bind(&SimpleAudio::print_element, this, _1, _2, _3), list_font_height);

  print_buttom(cur_files.size(), pos);
}

vector<Simplefile> SimpleAudio::add_dir(const string& dir, bool from_media)
{
  vector<Simplefile> cur_files;

  libfs_set_folders_first(opts.dir_order());

  for (file_iterator<file_t, default_order> i (dir); i != i.end (); ++i)
    {
      if (global->check_stop_bit()) {
	cur_files.clear();
	break;
      }

      string filename = filesystem::FExpand(i->getName());

      const MyPair filetype = check_type(filename, audio_conf->p_filetypes_a());

      if (filetype != emptyMyPair)
        cur_files.push_back(addsimplefile(filename, filetype, from_media));
    }

  return cur_files;
}

vector<Simplefile> SimpleAudio::rdir(const string& argv)
{
  vector<Simplefile> cur_files;

  libfs_set_folders_first(opts.dir_order());

  for (file_iterator<file_t, default_order> i (argv); i != i.end (); i.advance(false))
    {
      if (global->check_stop_bit()) {
	cur_files.clear();
	break;
      }

      string filename = filesystem::FExpand(i->getName());

      if (isDirectory (filename))
	{
	  // dir
	  Simplefile r;
	  r.id = ++id;
	  r.name = filename.substr(argv[argv.size()-1] == '/' ? argv.size() : argv.size() + 1);
	  if (conf->p_convert())
	    r.name = string_format::convert(r.name);
	  r.lowercase_name = string_format::lowercase(r.name);
	  r.path = filename;
	  r.type = "dir";
	  cur_files.push_back(r);
	} else {
	  // file
	  const MyPair filetype = check_type(filename, audio_conf->p_filetypes_a());
	  if (filetype != emptyMyPair)
	    cur_files.push_back(addsimplefile(filename, filetype));
	}
    }

  return cur_files;
}

void SimpleAudio::read_dirs()
{
  vector<Simplefile> playlist_new;

  // process audio dir
  audio_list = parse_dir(audio_folders);

  if (audio_folders.size() >= 1)
    std::sort(audio_list.begin(), audio_list.end(), file_sort());

  if (audio_list.size() != 0)
    // set position to the first file
      folders.top().second = 0;

  if (mode == ADD)
    files = &audio_list;
  else
    files = &playlist;
}
