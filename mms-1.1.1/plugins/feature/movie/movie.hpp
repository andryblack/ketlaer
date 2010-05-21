#ifndef MOVIE_HPP
#define MOVIE_HPP

#include "config.h"

#include "module.hpp"
#include "options.hpp"
#include "config.hpp"
#include "gettext.hpp"

#include "movie_config.hpp"

#include "busy_indicator.hpp"

#include "multifile.hpp"
#include "common-feature.hpp"
#include "libfspp.hpp"
#include "plugins.hpp"

#include "boost.hpp"

#ifdef use_inotify
#include "notify.hpp"
#endif

#include <stack>
#include <vector>
#include <list>

class Movie : public Module
{
protected:
  Movie();

  bool search_compare(const Multifile& s);
  std::string get_name_from_file(const Multifile& e);

  // commands
  virtual void options() = 0;
  void exit();
  virtual void search_func() = 0;

  // helper function
  void add_standard(ExtraMenu& em);

  bool exit_loop;

  enum SUBTYPE { IDX, SRT, NOSUB };

  // returns a pair of type, path_to_subtitles
  std::pair<SUBTYPE, std::vector<std::string> > find_subtitles(const std::string& dir);

  bool printing_information;

  bool navigating_media;
  std::list<std::string> top_media_folders;

  // playback
  void playmovie(const Multifile file, bool window = false);
  void external_movie_playback(const std::string& path);
  void play_dvd_helper(const std::string& device);
  void playcd(int count);
  void playvcd();
  void playdvd();

  // list of folders from config, the names are normalized to have / at the end
  std::list<std::string> movie_folders;

#ifdef use_inotify
  virtual void fs_change(NotifyUpdate::notify_update_type type, const std::string& path) = 0;
#endif
  virtual bool reload_dir(const std::string& path) = 0;
  virtual void reload_current_dirs() = 0;

  std::string testdir(int *count);

  Multifile addfile(const std::string& name, const MyPair& filetype);
  Multifile add_dir(const std::string& filename);

  virtual void load_runtime_settings();

  // For traversing the filesystem. The first element is the dirs and the next is the
  // position in the list generated from the dirs
  std::stack<std::pair<std::list<std::string>, int > > folders;

  class PlayerPluginOpts: public Options
  {
  public:
    void add_opt(Option *_opt);
    void set_header(const std::string &_str){header =_str;};
    void save(){    }; //do not save from here
  };

  class MovieOpts : public Options
  {
  public:
    MovieOpts();
    ~MovieOpts();

    std::string dir_order();

    void dummy(const std::string& str);
    std::string movie_player();
    std::string vcd_player();
    std::string dvd_player();

    std::string display_mode();

    bool mainloop();

    void set_players_correctly();
    bool vcd_constructed;
    bool dvd_constructed;

    // evil hack
    void clear();

  private:
    bool players_set_correctly;

    Option *reload_p;
    Option *dir_order_p;
    Option *movie_player_p, *vcd_player_p, *dvd_player_p;
    Option *display_mode_p;
  };

  struct file_sort
  {
    bool operator()(const Multifile& lhs, const Multifile& rhs);
  };

  virtual void load_current_dirs() = 0;

  // control player
  void add_playback_keys();
  bool active_control_player();
  void player_play();
  void player_pause();
  void player_stop();
  void player_ff();
  void player_fb();
  void player_fullscreen();

  bool playback_command(const Input& i);
  bool fullscreen_check(const Input& i);

  MoviePlayer *active_player;

  bool movie_playing;
  bool movie_playback_shown;

  MovieConfig *movie_conf;

  std::string header_font;
  std::string search_font;
  std::string search_select_font;
  std::string normal_font;
  std::string position_font;
  std::string information_header_font;
  std::string information_font;
  std::string dialog_header_font;
  std::string dialog_font;
  std::string list_font;
  std::string extra_information_font;

  std::pair<int, int> header_size;		// Size of Header
  int header_box_size;

  int normal_font_height;
  int dialog_header_font_height;
  int dialog_font_height;
  int list_font_height;
  int information_font_height;
  int extra_information_font_height;

  void res_dependant_calc();

public:

  void set_folders();

  virtual void save_runtime_settings();

#ifdef use_inotify
  // notify
  virtual void reparse_current_dir() = 0;
#endif

  MovieOpts* get_opts() { return &opts; }

  MovieOpts opts;

  void set_movie_playback(bool playback)
  {
    movie_playing = playback;
  }

  virtual void read_dirs() = 0;
  virtual void determine_media() = 0;
  virtual int files_size() = 0;

  void startup_updater();

  void reset();
};

template <typename T>
class MovieTemplate : public Movie
{
protected:

  void print_lcd_menu()
  {
    if (!global->lcd_output_possible())
      return;

    // dir
    std::string dir = "/";
    if (folders.size() != 1) {
      std::stack<std::pair<std::list<std::string>, int > > tmp_folders = folders;

      std::string name = tmp_folders.top().first.front();

      for (int number_of_slashes = tmp_folders.size(); number_of_slashes > 0; --number_of_slashes) {
	int p = name.rfind('/');
	if (p != std::string::npos) {
	  if (number_of_slashes != tmp_folders.size())
	    dir = name.substr(p) + dir;
	  name = name.substr(0, p);
	} else
	  break;
      }
    }

    // current
    std::string cur_name = "> " + get_name_from_file(vector_lookup(files, folders.top().second));

    std::string prev_name = "";

    if (files.size() > 2) {
      // prev
      int prev_pos = folders.top().second - 1;
      if (prev_pos == -1)
	prev_pos = files.size()-1;

      prev_name = get_name_from_file(vector_lookup(files, prev_pos));
    }

    std::string next_name = "";

    if (files.size() > 1) {
      // next
      int next_pos = folders.top().second + 1;
      if (next_pos == files.size())
	next_pos = 0;

      next_name = get_name_from_file(vector_lookup(files, next_pos));
    }

    if (global->lcd_rows() > 1) global->lcd_add_output(string_format::pretty_print_string_copy(dir,global->lcd_pixels()));
    if (global->lcd_rows() > 2) global->lcd_add_output(string_format::pretty_print_string_copy(prev_name,global->lcd_pixels()));
    global->lcd_add_output(cur_name);
    if (global->lcd_rows() > 3) global->lcd_add_output(string_format::pretty_print_string_copy(next_name,global->lcd_pixels()));

    global->lcd_print();
  }

  void playcd(int count)
  {
    if (count == 1) { //play disc

      std::list<MyPair> filetypes = movie_conf->p_filetypes_m();
      std::string file_types;

      foreach (MyPair& filetype, filetypes)
	file_types += filetype.first + "|";

      file_types = file_types.substr(0, file_types.size()-1);

      //-type f -printf \"'%p'

      std::string play_cmd = "find '" + cd->get_mount_point() +
	"' -regextype posix-egrep -follow -maxdepth 8 -iregex '.*\\.(" +
	file_types + ")' | xargs ";

      Plugins *plugins = S_Plugins::get_instance();
      MoviePlayerPlugin *plugin = plugins->find(plugins->movie_players, opts.movie_player());

      if (plugin == 0) {
	std::cerr << "something really wrong, movie player not found!" << std::endl;
	return;
      }

      if (plugin->player->wants_exclusivity())
	render->device->unlock();

      plugin->player->play_disc(play_cmd);

      if (plugin->player->wants_exclusivity())
	render->device->lock();

    } else {

      navigating_media = true;
      top_media_folders.clear();
      top_media_folders.push_back(cd->get_mount_point());

      std::vector<T> backup_files;

      backup_files = files;

      std::stack<std::pair<std::list<std::string>, int > > backup_folders = folders;

      std::list<std::string> dirs;
      dirs.push_back(cd->get_mount_point());

      files = parse_dir(dirs);

      while (!folders.empty())
	folders.pop();

      folders.push(std::make_pair(dirs, 0));

      S_BackgroundUpdater::get_instance()->run_once(boost::bind(&MovieTemplate::check_for_changes, this));

      mainloop();

      files = backup_files;
      folders = backup_folders;
      navigating_media = false;
    }

    //umount the disc
    run::external_program("umount '" + cd->get_mount_point() + "'");
  }

  virtual std::vector<T> parse_dir(const std::list<std::string>& dirs) = 0;

  // commands
  void action_play()
  {
    Multifile e = vector_lookup(files, folders.top().second);
    int nfiles = e.filenames.size();
    if (nfiles > 0)
      playmovie(e);
    else
      DialogWaitPrint pdialog(dgettext("mms-movie", "Folder is empty"), 1000);
  }

  virtual void page_down() = 0;
  virtual void page_up() = 0;

  void update_db_current_dirs()
  {
    foreach (std::string& dir, folders.top().first){
      file_tools::remove_full_dir_from_cache(dir);
      reload_dir(dir);
    }
  }

  void options()
  {
    visible = false;

    S_BusyIndicator::get_instance()->idle();

    if (opts.mainloop()) {
      S_BusyIndicator::get_instance()->busy();

      update_db_current_dirs();

      reparse_current_dir();

      S_BusyIndicator::get_instance()->idle();
    }

    opts.save();

    visible = true;
  }

  void movie_mainloop_common(const Input& input)
  {
    if (input.command == "page_up")
      {
	page_up();
      }
    else if (input.command == "page_down")
      {
	page_down();
      }
    else if (input.command == "options")
      {
	options();
      }
    else if (input.command == "startmenu")
      {
	exit();
      }
    else if (input.command == "switch_fullscreen")
      {
	if (render->device->supports_wid() && movie_conf->p_playback_in_mms())
	  player_fullscreen();
      }
  }

  std::vector<T> files;

public:
  int files_size() { return files.size(); }

  void load_current_dirs()
  {
    files = parse_dir(folders.top().first);

    if (folders.top().first.size() >= 1)
      std::sort(files.begin(), files.end(), file_sort());
  }

  virtual void check_for_changes() {}

  void read_dirs()
  {
    // process movie dir
    files = parse_dir(movie_folders);

    if (movie_folders.size() >= 1)
      std::sort(files.begin(), files.end(), file_sort());

    S_BackgroundUpdater::get_instance()->run_once(boost::bind(&MovieTemplate<T>::check_for_changes, this));
  }

  virtual void print(const std::vector<T>& files_to_print) = 0;

#ifdef use_inotify
  // notify
  void fs_change(NotifyUpdate::notify_update_type type, const std::string& path)
  {
    std::string dir =  filesystem::FExpand(path);
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

    if (!(active_control_player() && active_player->fullscreen()) &&
	!global->playback_in_fullscreen && !printing_information && visible && reparsed_current_dir)
      print(files);
  }
#endif

  void reparse_current_dir()
  {
    reload_current_dirs();

    // sanity check
    if (folders.top().second > files.size()-1)
      folders.top().second = 0;
  }

  void determine_media()
  {
    Multifile m;

    bool data_disc = cd->data_disc();
    cd->close();

    if (data_disc) {
      //mount the disc
      run::external_program("mount '" + cd->get_mount_point() + "'");

      int count;
      std::string mediatype = testdir(&count);

      if (mediatype == "empty") {
	Print pdialog(dgettext("mms-movie", "The disc does not contain any recognizable files"),
		      Print::SCREEN);
	return;
      }

      if (mediatype != "divx")
	//umount the disc
	run::external_program("umount '" + cd->get_mount_point() + "'");

      if (mediatype == "vcd")
	m.filetype = "vcd";
      else if (mediatype == "svcd")
	m.filetype = "vcd";
      else if (mediatype == "dvd")
	m.filetype = "dvd";
      else if (mediatype == "divx") {
	playcd(count);
	return;
      }
    } else {
      Print pdialog(dgettext("mms-movie", "The disc does not contain any recognizable files"),
		    Print::SCREEN);
      return;
    }

    m.filenames.push_back(cd->get_device());
    playmovie(m, false);
  }
};

#endif
