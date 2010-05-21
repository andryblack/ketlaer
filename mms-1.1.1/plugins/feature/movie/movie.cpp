// used for #ifdef

//#define DLOG

#include "config.h"

#include "movie.hpp"

//Input
#include "input.hpp"

#include "common.hpp"
#include "common-feature.hpp"

#include "gettext.hpp"

#include "extra_menu.hpp"
#include "updater.hpp"
#include "shutdown.hpp"

#include "plugins.hpp"

#include "graphics.hpp"

#include "resolution.hpp"

#include <iostream>

#include <sys/shm.h>

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

using std::list;
using std::vector;
using std::string;

Movie::MovieOpts::MovieOpts()
  : movie_player_p(NULL), vcd_player_p(NULL), dvd_player_p(NULL)
{
#ifdef use_nls
  // gettext
  setlocale(LC_ALL, ""); /* set from the environment variables */
  bindtextdomain("mms-movie", mms_prefix"/share/locale");
  bind_textdomain_codeset("mms-movie", nl_langinfo(CODESET));
#endif

  // header
  header = "Movie Options";
  translated_header = dgettext("mms-movie", "Video Options");
  save_name = "MovieOptions";

  MovieConfig *movie_conf = S_MovieConfig::get_instance();

#ifdef use_inotify
  if (movie_conf->p_movie_dirs_no_watch().size() > 0) {
#else
  if (movie_conf->p_movie_dirs_no_watch().size() > 0 || movie_conf->p_movie_dirs().size() > 0) {
#endif
    vector<string> none;
    reload_p = new Option(false, dgettext("mms-movie", "reload"), "reload", 0, none, none);
    val.push_back(reload_p);
  }

  dir_order_p = go->dir_order();
  val.push_back(dir_order_p);

  vector<string> display_mode_values;
  vector<string> english_display_mode_values;

  display_mode_values.push_back(dgettext("mms-movie", "icon view"));
  english_display_mode_values.push_back("icon view");

  display_mode_values.push_back(dgettext("mms-movie", "list view"));
  english_display_mode_values.push_back("list view");

  display_mode_p = new Option(false, dgettext("mms-movie", "display mode"), "display mode", 0,
			    display_mode_values, english_display_mode_values);
  val.push_back(display_mode_p);

  // global options
  if (conf->p_cdpath().size() > 1)
    val.push_back(go->cd_device);

  if (go->audio_device_video->values.size() > 1)
    val.push_back(go->audio_device_video);
}

void Movie::MovieOpts::clear()
{
  val.clear();
}

void Movie::PlayerPluginOpts::add_opt(Option *_opt)
{
  val.push_back(_opt);
}

void Movie::MovieOpts::dummy(const std::string& str)
{
  string _pl;
  if (str == "movie")
    _pl = movie_player_p->values[movie_player_p->pos];
  else if (str == "dvd")
    _pl = dvd_player_p->values[dvd_player_p->pos];
  else
    _pl = vcd_player_p->values[vcd_player_p->pos];

  //first thing: we locate the plugin whose name matches _pl;
  Plugins *plugins = S_Plugins::get_instance();

  MoviePlayerPlugin *_movieplayer = plugins->find(plugins->movie_players, _pl);

  if (_movieplayer == 0) {
    print_warning(dgettext("mms-movie", "No such plugin player: ") + _pl, "MOVIE");
    return;
  }

  if (!_movieplayer->player->provides_options()) { // this one doesn't provide options
    Print pdialog(_pl+ dgettext("mms-movie", " does not provide any options"),
		  Print::SCREEN);
    return;
  }
  _movieplayer->player->get_opts()->load();

  PlayerPluginOpts *myopt = new PlayerPluginOpts;
  myopt->set_header(dgettext("mms-movie", "Options"));

  vector<Option*> plugin_values = _movieplayer->player->get_opts()->values();
  for (unsigned int i = 0; i < plugin_values.size(); ++i) {
    string name = plugin_values[i]->name;
    if (name != dgettext("mms-movie", "reload") && name != gettext("sorting") &&
	name != dgettext("mms-movie", "cd device") && name != gettext("audio device"))
      myopt->add_opt(plugin_values[i]);
  }

  myopt->mainloop();
  _movieplayer->player->get_opts()->save();
  delete myopt;
}

void Movie::MovieOpts::set_players_correctly()
{
  vector<string> movie_player_values;
  vector<string> english_movie_player_values;

  vector<string> vcd_player_values;
  vector<string> english_vcd_player_values;

  vector<string> dvd_player_values;
  vector<string> english_dvd_player_values;

  Plugins *plugins = S_Plugins::get_instance();
  foreach (MoviePlayerPlugin* plugin, plugins->movie_players) {
    if (plugin->player->movie_playback())
      movie_player_values.push_back(plugin->plugin_name());
    if (plugin->player->vcd_playback())
      vcd_player_values.push_back(plugin->plugin_name());
    if (plugin->player->dvd_playback())
      dvd_player_values.push_back(plugin->plugin_name());
  }

  english_movie_player_values = movie_player_values;
  english_vcd_player_values = vcd_player_values;
  english_dvd_player_values = dvd_player_values;

  assert(movie_player_values.size() > 0);
  assert(vcd_player_values.size() > 0);
  assert(dvd_player_values.size() > 0);

  movie_player_p = new Option(false, dgettext("mms-movie", "video player"), "movie player", 0,
			      movie_player_values, english_movie_player_values,
			      boost::bind(&MovieOpts::dummy, this, "movie"));
  val.push_back(movie_player_p);

  vcd_player_p = new Option(false, dgettext("mms-movie", "vcd player"), "vcd player", 0,
			    vcd_player_values, english_vcd_player_values,
			    boost::bind(&MovieOpts::dummy, this, "vcd"));
  val.push_back(vcd_player_p);

  dvd_player_p = new Option(false, dgettext("mms-movie", "dvd player"), "dvd player", 0,
			    dvd_player_values, english_dvd_player_values,
			    boost::bind(&MovieOpts::dummy, this, "dvd"));
  val.push_back(dvd_player_p);

  players_set_correctly = true;
}


Movie::MovieOpts::~MovieOpts()
{
  save();

  if (!S_MovieConfig::get_instance()->p_graphical_movie_mode())
    delete reload_p;

  delete dir_order_p;

  if (dvd_player_p != NULL)
    delete dvd_player_p;
  if (vcd_player_p != NULL)
    delete vcd_player_p;
  if (movie_player_p != NULL)
    delete movie_player_p;
}

string Movie::MovieOpts::dir_order()
{
  return dir_order_p->english_values[dir_order_p->pos];
}

string Movie::MovieOpts::movie_player()
{
  if (movie_player_p != NULL)
    return movie_player_p->values[movie_player_p->pos];
  else
    return "";
}

string Movie::MovieOpts::vcd_player()
{
  if (vcd_player_p != NULL)
    return vcd_player_p->values[vcd_player_p->pos];
  else
    return "";
}

string Movie::MovieOpts::dvd_player()
{
  if (dvd_player_p != NULL)
    return dvd_player_p->values[dvd_player_p->pos];
  else
    return "";
}

string Movie::MovieOpts::display_mode()
{
  if (display_mode_p != NULL)
    return display_mode_p->values[display_mode_p->pos];
  else
    return "";
}

bool Movie::MovieOpts::mainloop()
{
  return Options::mainloop();
}

Movie::Movie()
  : printing_information(false), active_player(0), movie_playing(false), navigating_media(false)
{
  movie_conf = S_MovieConfig::get_instance();

  res_dependant_calc();

  S_ResolutionManagement::get_instance()->register_callback(boost::bind(&Movie::res_dependant_calc, this));
}

void Movie::res_dependant_calc()
{
  header_font = graphics::resolution_dependant_font_wrapper(28, conf);
  search_font = graphics::resolution_dependant_font_wrapper(22, conf);
  search_select_font = graphics::resolution_dependant_font_wrapper(17, conf);
  normal_font = graphics::resolution_dependant_font_wrapper(13, conf);
  position_font = graphics::resolution_dependant_font_wrapper(18, conf);
  information_header_font = graphics::resolution_dependant_font_wrapper(25, conf);
  information_font = graphics::resolution_dependant_font_wrapper(18, conf);
  dialog_header_font = graphics::resolution_dependant_font_wrapper(20, conf);
  dialog_font = graphics::resolution_dependant_font_wrapper(17, conf);
  list_font = graphics::resolution_dependant_font_wrapper(16, conf);

  extra_information_font = graphics::resolution_dependant_font_wrapper(13, conf);

  normal_font_height = graphics::calculate_font_height(normal_font, conf);
  dialog_header_font_height = graphics::calculate_font_height(dialog_header_font, conf);
  dialog_font_height = graphics::calculate_font_height(dialog_font, conf);
  list_font_height = graphics::calculate_font_height(list_font, conf);
  information_font_height = graphics::calculate_font_height(information_font, conf);
  extra_information_font_height = graphics::calculate_font_height(extra_information_font, conf);

  header_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
  header_box_size = static_cast<int>(header_size.second * 0.75);
}

void Movie::set_folders()
{
  list<string> movie_dirs = movie_conf->p_movie_dirs(); // make compiler happy
  // make sure movie folders are not malformed
  foreach (string& dir, movie_dirs)
    if (dir[dir.size()-1] != '/') {
      movie_folders.push_back(dir + '/');
    } else
      movie_folders.push_back(dir);

#ifdef use_inotify
  S_Notify::get_instance()->register_plugin("movie", movie_folders,
					    boost::bind(&Movie::fs_change, this, _1, _2));
#endif

  list<string> movie_dirs_no_watch = movie_conf->p_movie_dirs_no_watch(); // make compiler happy
  // make sure movie folders are not malformed
  foreach (string& dir, movie_dirs_no_watch)
    if (dir[dir.size()-1] != '/') {
      movie_folders.push_back(dir + '/');
    } else
      movie_folders.push_back(dir);

  reset();
}

void Movie::save_runtime_settings()
{}

void Movie::load_runtime_settings()
{}

void Movie::reset()
{
  for (int i = 0, size = folders.size(); i < size; ++i)
    folders.pop();

  folders.push(std::make_pair(movie_folders, 0));
}

void Movie::startup_updater()
{
  load_runtime_settings();

  global->register_movie_playback_func(boost::bind(&Movie::external_movie_playback, this, _1));

  opts.set_players_correctly();
  add_playback_keys();
}

void Movie::add_playback_keys()
{
  Playback p = Playback("Movie");
  p.add_command(GlobalCommand(boost::bind(&Movie::player_play, this), "play", dgettext("mms-movie", "Play")));
  p.add_command(GlobalCommand(boost::bind(&Movie::player_pause, this), "pause", dgettext("mms-movie", "Pause")));
  p.add_command(GlobalCommand(boost::bind(&Movie::player_stop, this), "stop", dgettext("mms-movie", "Stop")));
  p.add_command(GlobalCommand(boost::bind(&Movie::player_ff, this), "ff", dgettext("mms-movie", "Fast forward")));
  p.add_command(GlobalCommand(boost::bind(&Movie::player_fb, this), "fb", dgettext("mms-movie", "Fast backward")));
  global->add_playback(p);
}

bool Movie::active_control_player()
{
  return (active_player != 0 && active_player->control_playback() && movie_playing);
}

void Movie::player_play()
{
  if (active_control_player())
    active_player->play();
}

void Movie::player_pause()
{
  if (active_control_player())
    active_player->pause();
}

void Movie::player_stop()
{
  if (active_control_player())
    active_player->stop();
}

void Movie::player_ff()
{
  if (active_control_player())
    active_player->ff();
}

void Movie::player_fb()
{
  if (active_control_player())
    active_player->fb();
}

void Movie::player_fullscreen()
{
  if (active_control_player())
    active_player->switch_screen_size();
}

bool Movie::playback_command(const Input& i)
{
  return (i.command == "play" || i.command == "pause" || i.command == "stop" ||
	  i.command == "ff" || i.command == "fb");
}

bool Movie::fullscreen_check(const Input& i)
{
  if (!playback_command(i))
    if (active_control_player() && active_player->fullscreen() && active_player->playing()) {
      player_fullscreen();
      return true;
    }
  return false;
}

bool Movie::file_sort::operator()(const Multifile& lhs, const Multifile& rhs)
{
  return sort_order(lhs.lowercase_name, rhs.lowercase_name,
		    lhs.path.empty() ? lhs.filenames.front() : lhs.path,
		    rhs.path.empty() ? rhs.filenames.front() : rhs.path,
		    !lhs.path.empty(), !rhs.path.empty(),
		    get_class<Movie>(dgettext("mms-movie", "Video"))->get_opts()->dir_order());
}

void Movie::exit()
{
  exit_loop = true;
}

void Movie::add_standard(ExtraMenu& em)
{
  if (render->device->supports_wid() && movie_conf->p_playback_in_mms())
    em.add_item(ExtraMenuItem(dgettext("mms-movie", "Switch video playback to fullscreen"),
			      input_master->find_shortcut("switch_fullscreen"),
			      boost::bind(&Movie::player_fullscreen, this)));

  em.add_item(ExtraMenuItem(dgettext("mms-movie", "Search"), input_master->find_shortcut("search"),
			    boost::bind(&Movie::search_func, this)));
  em.add_item(ExtraMenuItem(dgettext("mms-movie", "Options"), input_master->find_shortcut("options"),
			    boost::bind(&Movie::options, this)));
}

string Movie::get_name_from_file(const Multifile& e)
{
  return e.name;
}

bool Movie::search_compare(const Multifile& s)
{
  return (s.lowercase_name.substr(0, search_str.size()) == lowercase_search_str);
}

std::pair<Movie::SUBTYPE, vector<string> > Movie::find_subtitles(const string& dir)
{
  SUBTYPE type = NOSUB;

  vector<string> subtitles;

  libfs_set_folders_first(opts.dir_order());

  for (file_iterator<file_t, default_order> i (dir); i != i.end (); ++i) {
    string name = i->getName();
    if (name.substr(name.size()-3) == "srt") {
      if (type == NOSUB)
	type = SRT;
      subtitles.push_back(name);
    } else if (name.substr(name.size()-3) == "idx") {
      if (type == NOSUB)
	type = IDX;
      subtitles.push_back(name.substr(0, name.size()-4));
    }
  }

  return std::make_pair(type, subtitles);
}

void Movie::external_movie_playback(const string& path)
{
  Multifile m;
  m.filetype = "file";
  m.filenames.push_back(path);

  playmovie(m, true);
}

void Movie::playmovie(const Multifile file, bool window)
{
  DialogWaitPrint pdialog(dgettext("mms-movie", "Starting playback..."), 1000);

  Plugins *plugins = S_Plugins::get_instance();

  MoviePlayerPlugin *plugin;

  if (file.filetype == "dvd")
    plugin = plugins->find(plugins->movie_players, opts.dvd_player());
  else if (file.filetype == "vcd")
    plugin = plugins->find(plugins->movie_players, opts.vcd_player());
  else
    plugin = plugins->find(plugins->movie_players, opts.movie_player());

  if (plugin == 0) {
    std::cerr << "something really wrong, movie player not found!" << std::endl;
    return;
  }

  active_player = plugin->player;

  S_BusyIndicator::get_instance()->disable();

  while (!pdialog.cleaned()) {
    // wait for dialog to clean up
    mmsUsleep(100*1000);
  }

  if (file.filetype == "dvd")
    active_player->play_dvd(file.filenames.front());
  else if (file.filetype == "vcd")
    active_player->play_vcd(cd->get_device());
  else {

    string subcommand = "";
    vector<string> empty_path;
    std::pair<SUBTYPE, vector<string> > subtitles = std::make_pair(NOSUB, empty_path);

    string cur_files = "";

    // support playlist files
    if (file.filetype == "pls")
      cur_files += " -playlist ";

    foreach (const string& filename, file.filenames)
      cur_files += " " + string_format::escape_string(filename);

    active_player->play_movie(cur_files, window);
  }

  // make sure the screensaver doesn't kick in right after movie finishes
  conf->s_last_key(time(0));

  if (!active_player->wants_exclusivity())
    render->complete_redraw();

  S_BusyIndicator::get_instance()->enable();
}

string Movie::testdir(int *count)
{
  *count = 0;

  libfs_set_folders_first(opts.dir_order());

  for (file_iterator<file_t, default_order> i (cd->get_mount_point()); i != i.end (); ++i)
    {
      if (global->check_stop_bit()) {
	*count = 0;
	break;
      }

      string filename = filesystem::FExpand(i->getName());

      if (isDirectory (filename)) {

	string cur_dir = string_format::lowercase(filename.substr(filename.rfind("/")+1));
	if (cur_dir == "svcd")
	  return "svcd";

	if (cur_dir == "vcd")
	  return "vcd";

	if (cur_dir == "video_ts")
	  return "dvd";
      } else {
	if (check_type(filename, movie_conf->p_filetypes_m()) != emptyMyPair)
	  ++(*count);
      }
    }

  if (*count == 0)
    return "empty";
  else
    // none of the above found, it must be a divx
    return "divx";
}

// play a vcd
void Movie::playvcd()
{
  Plugins *plugins = S_Plugins::get_instance();
  MoviePlayerPlugin *plugin = plugins->find(plugins->movie_players, opts.vcd_player());

  if (plugin == 0) {
    std::cerr << "something really wrong, movie player not found!" << std::endl;
    return;
  }

  S_BusyIndicator::get_instance()->disable();

  //play disc
  plugin->player->play_vcd(cd->get_device());

  // make sure the screensaver doesn't kick in right after movie finishes
  conf->s_last_key(time(0));

  S_BusyIndicator::get_instance()->enable();
}

// play a dvd
void Movie::playdvd()
{
  Plugins *plugins = S_Plugins::get_instance();
  MoviePlayerPlugin *plugin = plugins->find(plugins->movie_players, opts.dvd_player());

  if (plugin == 0) {
    std::cerr << "something really wrong, movie player not found!" << std::endl;
    return;
  }

  S_BusyIndicator::get_instance()->disable();

  play_dvd_helper(cd->get_device());

  // make sure the screensaver doesn't kick in right after movie finishes
  conf->s_last_key(time(0));

  S_BusyIndicator::get_instance()->enable();
}

void Movie::play_dvd_helper(const string& device)
{
  //play disc
  Plugins *plugins = S_Plugins::get_instance();
  MoviePlayerPlugin *plugin = plugins->find(plugins->movie_players, opts.dvd_player());

  if (plugin == 0) {
    std::cerr << "something really wrong, movie player not found!" << std::endl;
    return;
  }

  plugin->player->play_dvd(device);
}

// ====================================================================
//
// dir
//
// ====================================================================

// Add a file to the list
Multifile Movie::addfile(const string& name, const MyPair& filetype)
{
  Multifile r;
  r.id = ++id;
  r.filenames.push_back(name);
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

// Add a moviedir as "one file" to the movielist
Multifile Movie::add_dir(const string& filename)
{
  Multifile output;

  string type = "";

  vector<string> known_filetypes;

  int nr_of_elements = 0;

  libfs_set_folders_first(opts.dir_order());

  for (file_iterator<file_t, default_order> i (filename); i != i.end (); i.advance(false))
    {
      string name = filesystem::FExpand(i->getName());

      if (string_format::lowercase(name).rfind("video_ts") != string::npos) {
	type = "dvd";
	nr_of_elements++;
	break;
      }

      string short_name = name.substr(name.size()-3, 2);

      if (string_format::lowercase(short_name) == "cd") {
	type = "cd";
	nr_of_elements++;
	break;
      } else if (check_type(name, movie_conf->p_filetypes_m()) != emptyMyPair) {
	known_filetypes.push_back(name);
	type = "movie";
      }

      ++nr_of_elements;
    }

  if (nr_of_elements == 0) {
    output.type = "empty-dir";
    return output;
  }

  if (type == "dvd") {

    output.id = ++id;
    string::size_type pos = filename.rfind("/");
    output.name = filename.substr(pos+1);
    output.type = "dir-single";
    output.path = filename;
    output.filetype = "dvd";

    libfs_set_folders_first(opts.dir_order());

    for (file_iterator<file_t, default_order> i (filename);
	 i != i.end (); i.advance(false)) {
      if (string_format::lowercase(i->getName()).rfind("/video_ts") != string::npos) {
	output.filenames.push_back(i->getName());
	break;
      }
    }

  } else if (type == "cd") {

    string::size_type pos = filename.rfind("/");

    output.id = ++id;
    output.name = filename.substr(pos+1);
    output.type = "dir-single";
    output.path = filename;

    libfs_set_folders_first(opts.dir_order());

    for (file_iterator<file_t, default_order> i (filename); i != i.end (); i.advance(false)) {
      file_t cur_file = *i;

      string name = cur_file.getName();
      string short_name = name.substr(name.size()-3, 2);

      if (string_format::lowercase(short_name) == "cd") {
	for (file_iterator<file_t, default_order> i2 (name); i2 != i2.end(); i2.advance(false)) {
	  cur_file = *i2;

	  const MyPair filetype = check_type(cur_file.getName(), movie_conf->p_filetypes_m());
	  if (filetype != emptyMyPair) {
	    output.filetype = filetype.second;
	    output.filenames.push_back((*i2).getName());
	  } else
	    output.filetype = "dir";
	}
      }
    }

  } else if (!known_filetypes.empty()) {
    output.id = ++id;
    string::size_type pos = filename.rfind("/");
    output.name = filename.substr(pos+1);
    output.path = filename;

    output.type = "dir-single";
    if (known_filetypes.size() > 1) {
      string first = known_filetypes.front();
      vector<string>::iterator i = known_filetypes.begin();
      ++i;
      foreach (string& filetype, known_filetypes)
	if (string_format::difference(first, filetype) > 1) {
	  output.type = "dir-multi";
	  break;
	}
    }

    if (conf->p_convert())
      output.name = string_format::convert(output.name);

    const MyPair filetype = check_type(known_filetypes.front(), movie_conf->p_filetypes_m());
    if (filetype != emptyMyPair)
      output.filetype = filetype.second;
    else
      output.filetype = "dir";
    foreach (string& filetype, known_filetypes)
      output.filenames.push_back(filetype);

  } else {

    output.id = ++id;
    string::size_type pos = filename.rfind("/");
    output.name = filename.substr(pos+1);
    if (nr_of_elements > 1)
      output.type = "dir-multi";
    else
      output.type = "dir";
    output.path = filename;
    output.filetype = "dir";
  }

  output.lowercase_name = string_format::lowercase(output.name);

  return output;
}
