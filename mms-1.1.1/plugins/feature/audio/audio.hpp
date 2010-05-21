#ifndef AUDIO_HPP
#define AUDIO_HPP

#include "config.h"

#include "startmenu.hpp"
#include "module.hpp"
#include "simplefile.hpp"
#include "shuffle_list.hpp"
#include "options.hpp"
#include "singleton.hpp"
#include "common-feature.hpp"
#include "updater.hpp"
#include "rand.hpp"
#include "busy_indicator.hpp"
#include "shutdown.hpp"

#include "audio_s.hpp"
#include "shoutcast.hpp"

#include "lastfm.hpp"

//lyrics
#include "lyrics.hpp"

#include "audio_config.hpp"
#include "radio_config.hpp"

#ifdef use_inotify
#include "notify.hpp"
#endif

#include "boost.hpp"

// ostringstream
#include <sstream>

#include <fstream>

#include <stack>
#include <vector>
#include <cassert>
#include "CD_Tag.h"

class Audio : public Module
{
  friend class AudioPlayer;
  friend class Updater;
protected:

  Audio();

  // class definitions
  class AudioOpts : public Options
  {
  public:
    AudioOpts();
    ~AudioOpts();

    std::string dir_order()
    {
      return dir_order_p->english_values[dir_order_p->pos];
    }

    std::string repeat()
    {
      return repeat_p->values[repeat_p->pos];
    }

    std::string shutdown()
    {
      return shutdown_p->values[shutdown_p->pos];
    }

    std::string shuffle()
    {
      return shuffle_p->values[shuffle_p->pos];
    }

    std::string lyrics()
    {
      return lyrics_p->values[lyrics_p->pos];
    }

    std::string time_mode()
    {
      return time_mode_p->values[time_mode_p->pos];
    }

    std::string bitrate()
    {
      return bitrate_p->values[bitrate_p->pos];
    }

    std::string metadata_extract()
    {
      return extract_meta_p->values[extract_meta_p->pos];
    }

    std::vector<Option*> values()
    {
      return val;
    }

  private:
    Option *reload_p, *dir_order_p, *repeat_p, *shuffle_p, *shutdown_p, *lyrics_p, *time_mode_p, *extract_meta_p, *bitrate_p;
  };

  // list of folders from config, the names are normalized to have / at the end
  std::list<std::string> audio_folders;
  void set_folders();

  bool exit_fall_through;

  long buffering_time_end;
  void set_buffering_timeout();

  // commands
  void exit();
  void exit_playlist();
  // audio add specific
  virtual void enter_dir() = 0;
  void leave_dir();
  // playlist specific
  void save_playlist_func();
  // playlist
  virtual void play() = 0;
  void pause();
  void stop();
  void next_track();
  void prev_track();
  void ff();
  void fb();
  void vol_plus();
  void vol_minus();
  void mute();
  void toggle_playlist_helper();
  virtual void toggle_playlist() = 0;
  virtual void toggle_load_playlist() = 0;

  virtual bool load_playlist_from_db(const std::vector<LastFM::Track>& tracks) = 0;

  bool navigating_media;
  std::list<std::string> top_media_folders;

  bool in_choose_playlist;
  bool exit_choose_playlist_loop;
  void exit_choose_playlist();

  virtual void audio_screensaver_next_helper(int x, int& y) = 0;

  virtual void options() = 0;

  int playlist_pos_int;
  virtual void radio_action() {};
  virtual void radio_playlist_add() {};
  void back_radio();

  typedef std::pair<std::string, std::string> RadioStation;

  std::vector<RadioStation> radio_stations;
  void save_radio_stations();
  void radio_my_stations_add();
  void radio_my_stations_remove();

  void action_load_playlist();
  void delete_playlist();
  std::vector<std::string> saved_playlists;
  int saved_playlist_pos;
  bool returned, return_value;

  bool exit_loop;
  bool change_mode;

  bool printing_information;

  // radio
  bool in_genre;
  bool radio_update_needed;

  vector<RadioStation> cur_radio_files;
  vector<string> print_radio_files;

  int radio_pos;
  int old_radio_pos;
  bool exit_radio;

  std::string format_time(int cur_time, int total_time);
  std::string format_time(int time);
  std::string lcd_format_time(int cur_time, int total_time);

  const int position_int();
  void set_position_int(int pos);

  void find_radio_element_and_run_callback(const string& element);
  void find_choose_playlist_element_and_run_callback(const std::string& element);

  void print_choose_playlist_string_element(const std::string& r, const std::string& position, int y);
  void print_radio_string_element(const std::string& r, const std::string& position, int y);

  void print_choose_playlist(const std::vector<std::string>& files, const std::string& position, const int int_position);
  void print_radio_list(const std::vector<std::string>& files, const std::string& position, const int int_position);

  // print helper functions
  void print_top();
  void print_buttom(int files_size, int position);

  // print audiotrack helper
  virtual void print_audiotrack_helper(int &x_pos) = 0;
  virtual bool print_audio_screensaver_helper(int &x_pos, int &y, int& max_x_pic,
					      int& max_y_pic, bool vert_center) = 0;
  virtual void get_audiotrack_info(std::string& buffer, std::string& artist, std::string& album, std::string& title) = 0;

  // lyrics
  int lyr_disp;
  int lyr_last;
  double lyr_freq;
  double lyr_period;

  void show_lyrics();
  bool is_screensaver_lyrics();

  // helper function for fullscreen
  void print_audio_screensaver();
  void print_audio_screensaver_lyrics(std::string artist, std::string album,
				      std::string title, std::string buffer);
  void print_audio_screensaver_standard(std::string artist, std::string album,
					std::string title, std::string buffer);

  int displayed_volume;

#ifdef use_notify_area
  void nArea_activate();
#endif
  void print_audiotrack();
  void print_lcd_audiotrack();
  int print_audio_time();

  int check_audio_time();
  virtual void check_audio_state() = 0;

  int check_audio_fullscreen();
  
  virtual void print_audio_fullscreen() = 0;
  virtual void stop_ssaver() = 0;
  virtual void start_ssaver() = 0;
  void process_ssaver_input(const Input&);
  virtual bool is_ssaver_running() = 0;
  virtual void cleanup_audio_fullscreen() = 0;
  virtual void init_ssaver_thread() = 0;
  virtual void kill_ssaver_thread() = 0;

  void show_mute();

  int check_volume();
  void show_volume();
  void clean_up_volume();

  virtual void add_audio_cd() = 0;
  virtual void add_data_cd() = 0;

  virtual void cd_menu() = 0;

  bool mounted_media;

  // For traversing the filesystem. The first element is the dirs and the next is the
  // position in the list generated from the dirs
  std::stack<std::pair<std::list<std::string>, int > > folders;

  // used when adding files to the playlist
  int playlist_id;

  ShuffleList shuffle_list;

  // playlist
  bool choose_playlist();
  virtual void save_playlist(const std::string& playlistfile, bool overwrite = false) = 0;
  virtual bool load_last_playlist() = 0;

  std::string mainloop()
  { return ""; }

  AudioOpts opts;

  int opts_offset;

  struct file_sort
  {
    bool operator()(const Simplefile& lhs, const Simplefile& rhs);
  };

  void set_audio_player();


  // helper functions
  bool check_mode(const Input& input, bool disable_mode_change);
  bool check_playlist_mode(const Input& input, bool disable_mode_change);

  // external
  bool was_playing;
  bool was_paused;
  bool was_started;

  virtual void load_current_dirs() = 0;

  virtual int get_real_playlist_pos() = 0;

  bool radio_stations_downloaded;

  AudioConfig *audio_conf;
  RadioConfig *radio_conf;

  bool disabled_busy_indicator;

  bool display_message;

  std::string header_font;
  std::string search_font;
  std::string search_select_font;
  std::string normal_font;
  std::string position_font;
  std::string dialog_header;
  std::string information_bigger_font;
  std::string information_font;
  std::string list_font;
  std::string button_playback_font;
  std::string volume_font;

  std::string screensaver_lyrics_artist_font;
  std::string screensaver_lyrics_album_font;
  std::string screensaver_lyrics_normal_font;
  std::string screensaver_lyric_font;

  std::string screensaver_artist_font;
  std::string screensaver_album_font;
  std::string screensaver_normal_font;

  std::pair<int, int> header_size;		// Size of Header
  int header_box_size;
  int button_playback_height;
  int list_font_height;
  int normal_font_height;

  int screensaver_lyrics_artist_font_height;
  int screensaver_lyrics_album_font_height;
  int screensaver_lyrics_normal_font_height;
  int screensaver_lyric_font_height;

  int screensaver_artist_font_height;
  int screensaver_album_font_height;
  int screensaver_normal_font_height;

  void res_dependant_calc();

public:

  void load_radio_stations();

  void check_shutdown();
#ifdef use_notify_area
  void naAudioPlayTrack();
  void naAudioOptions();
#endif

  void save_runtime_settings();
  void add_playback_keys();

  // playlist public
  std::vector<std::string> list_playlists();
  virtual bool load_playlist(const std::string& playlistfile) = 0;


#ifdef use_inotify
  // notify
  virtual void fs_change(NotifyUpdate::notify_update_type type, const std::string& path) = 0;
#endif
  virtual void reparse_current_dir() = 0;
  virtual void next_audio_track() = 0;
  bool play_now_warning;

  Audio_s *audio_state;
  

  AudioOpts* get_opts()
  {
    return &opts;
  }

  void pos_umount();
  void check_mount_before(const std::string& track_type);
  void check_mount_after(const std::string& track_type);

  virtual void prev_helper() = 0;
  virtual Simplefile next_helper(bool modify) = 0;

  virtual int playlist_size() = 0;
  virtual int audiolist_size() = 0;
  virtual bool last_element_in_playlist() = 0;

  virtual void reset() = 0;
  virtual void read_dirs() = 0;

  void begin_external();
  void end_external();

  enum AudioMode { ADD, PLAY };

  AudioMode mode;

  // entry points
  virtual std::string mainloop(AudioMode mode) = 0;
  void detect_cdformat();
  void choose_radio();

  void startup_updater();

  virtual void update_playlist_view() = 0;
};

template <typename T>
class AudioTemplate : public Audio
{
protected:
  AudioTemplate()
  : in_playlist(false), files(&audio_list), ssaver_thread_initted(false),
  ssaver_die(false), saverrunning(false), curlayer(0)
  {
    pthread_mutex_init(&ssaver_mut, NULL);
    pthread_cond_init(&ssaver_switch, NULL);
    reset();
  }
  bool in_playlist;

/* For Audio ScreenSaver */
  pthread_mutex_t ssaver_mut;
  pthread_cond_t ssaver_switch;
  pthread_t thread_run;
  bool ssaver_thread_initted;
  volatile bool ssaver_bool;
  volatile bool ssaver_die;
  volatile bool saverrunning;
  int curlayer;


  void mainloop_top(AudioMode new_mode)
  {
    if (new_mode == ADD) {
      input_master->set_map("audio");
      files = &audio_list;
      mode = ADD;
    } else {
      input_master->set_map("playlist");
      files = &playlist;
      mode = PLAY;
    }
  }

  virtual void update_playlist_view()
  {
    if (in_playlist && !audio_state->fullscreen_info && !printing_information && visible)
      print(*files);
  }

  virtual void secondary_menu_commands(ExtraMenu &em) {};
  virtual void secondary_menu_in_add(ExtraMenu &em) = 0;

  void jump_to_playing_track()
  {
    Simplefile cur_track = audio_state->p->p_cur_nr();

    if (cur_track.id == 0) // deleted track playing
      return;

    int pos = 0;

    foreach (T& t, playlist) {
      if (t.id == cur_track.id) {
	playlist_pos_int = pos;
	return;
      }

      ++pos;
    }
  }

  // commands
  void secondary_menu_helper(ExtraMenu &em)
  {
    if (mode == ADD) {

      secondary_menu_in_add(em);

      if (vector_lookup(*files, position_int()).type != "dir")
	em.add_item(ExtraMenuItem(dgettext("mms-audio", "Add all tracks to playlist"),
				  input_master->find_shortcut("add_all"),
				  boost::bind(&AudioTemplate<T>::add_all, this)));

      if (audio_conf->p_audio_mode_playnow() &&
	  vector_lookup(*files, position_int()).type == "dir")
	em.add_item(ExtraMenuItem(dgettext("mms-audio", "Play now"),
				  input_master->find_shortcut("play_now"),
				  boost::bind(&AudioTemplate<T>::play_now, this)));
      else if (vector_lookup(*files, position_int()).type != "dir")
	em.add_item(ExtraMenuItem(dgettext("mms-audio", "Play track now"),
				  input_master->find_shortcut("play_now"),
				  boost::bind(&AudioTemplate<T>::play_track_now, this)));

      if (folders.size() > 1)
	em.add_item(ExtraMenuItem(dgettext("mms-audio", "Return to startmenu"),
				  input_master->find_shortcut("startmenu"),
				  boost::bind(&AudioTemplate<T>::exit, this)));

    } else {
      em.add_item(ExtraMenuItem(dgettext("mms-audio", "Play track"),
				input_master->find_shortcut("action"),
				boost::bind(&AudioTemplate<T>::play_track, this)));
      em.add_item(ExtraMenuItem(dgettext("mms-audio", "Move track up"),
				input_master->find_shortcut("move_up"),
				boost::bind(&AudioTemplate<T>::move_up, this)));
      em.add_item(ExtraMenuItem(dgettext("mms-audio", "Move track down"),
				input_master->find_shortcut("move_down"),
				boost::bind(&AudioTemplate<T>::move_down, this)));
      em.add_item(ExtraMenuItem(dgettext("mms-audio", "Delete track"),
				input_master->find_shortcut("delete"),
				boost::bind(&AudioTemplate<T>::delete_track, this)));
      em.add_item(ExtraMenuItem(dgettext("mms-audio", "Jump to currently playing track"),
				input_master->find_shortcut("jump"),
				boost::bind(&AudioTemplate<T>::jump_to_playing_track, this)));
      string message = dgettext("mms-audio", "Queue track");
      if (audio_state->queue_pos(vector_lookup(*files, position_int())) != 0)
	message = dgettext("mms-audio", "Dequeue track");
      em.add_item(ExtraMenuItem(message,
				input_master->find_shortcut("queue"),
				boost::bind(&AudioTemplate<T>::queue_track, this)));

      em.add_item(ExtraMenuItem(dgettext("mms-audio", "Return to startmenu"),
				input_master->find_shortcut("startmenu"),
				boost::bind(&AudioTemplate<T>::exit_playlist, this)));

      em.add_item(ExtraMenuItem(dgettext("mms-audio", "Clear playlist"),
				input_master->find_shortcut("clear_list"),
				boost::bind(&AudioTemplate<T>::clear_playlist, this)));
      em.add_item(ExtraMenuItem(dgettext("mms-audio", "Save playlist"),
				input_master->find_shortcut("save_playlist"),
				boost::bind(&AudioTemplate<T>::save_playlist_func, this)));
    }

    em.add_item(ExtraMenuItem(dgettext("mms-audio", "Load playlist"),
			      input_master->find_shortcut("playlist_mode"),
			      boost::bind(&AudioTemplate<T>::toggle_load_playlist, this)));

    secondary_menu_commands(em);

    em.add_item(ExtraMenuItem(dgettext("mms-audio", "Search"),
			      input_master->find_shortcut("search"),
			      boost::bind(&AudioTemplate<T>::search_func, this)));
    em.add_item(ExtraMenuItem(dgettext("mms-audio", "Options"),
			      input_master->find_shortcut("options"),
			      boost::bind(&AudioTemplate<T>::options, this)));

    foreach (ExtraMenuItem& item, global->menu_items)
      em.add_persistent_item(item);

    conf->s_sam(true);
  }

  void secondary_menu()
  {
    ExtraMenu em;
    secondary_menu_helper(em);
    em.mainloop();
  }

  void play_now()
  {
    if (add_dir(vector_lookup(*files, position_int()).path).size() == 0) {
      DialogWaitPrint pdialog(dgettext("mms-audio", "Folder is empty"), 1000);
      return;
    }

    if (play_now_warning) {
      DialogWaitPrint pdialog(5000);
      pdialog.add_line(dgettext("mms-audio", "Clearing playlist to play contents of folder"));
      pdialog.add_line(dgettext("mms-audio", "The old playlist has been saved as 'autosaved'"));
      pdialog.print();
      play_now_warning = false;
    }

    save_playlist("autosaved", true);

    clear_playlist();
    // don't change mode to playlist
    change_mode = false;
    display_message = false;
    add();
    display_message = true;
    play_track();
  }

  void play_track_now()
  {
    add();
    playlist_pos_int = playlist.size()-1;
    play_track();
  }

  void print_lcd_menu()
  {
    if (!global->lcd_output_possible())
      return;

    std::string header = (mode == ADD) ? "Audio" : "Playlist";

    // current
    std::string cur_name = get_short_name_from_file(vector_lookup(*files, position_int()));

    cur_name = "> " + cur_name;

    std::string prev_name = "";

    if (files->size() > 2) {
      // prev
      int prev_pos = position_int() - 1;
      if (prev_pos == -1)
	prev_pos = files->size()-1;

      prev_name = get_short_name_from_file(vector_lookup(*files, prev_pos));
    }

    std::string next_name = "";

    if (files->size() > 1) {
      // next
      int next_pos = position_int() + 1;
      if (next_pos == files->size())
	next_pos = 0;

      next_name = get_short_name_from_file(vector_lookup(*files, next_pos));
    }

    if (global->lcd_rows() > 1) global->lcd_add_output(string_format::pretty_pad_string(header,global->lcd_pixels(),' '));
    if (global->lcd_rows() > 2) global->lcd_add_output(string_format::pretty_print_string_copy(prev_name,global->lcd_pixels()));
    global->lcd_add_output(cur_name);
    if (global->lcd_rows() > 3) global->lcd_add_output(string_format::pretty_print_string_copy(next_name,global->lcd_pixels()));
    global->lcd_print();
  }

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
    
    if (type == NotifyUpdate::WRITE_CLOSE_FILE || type == NotifyUpdate::MOVE)
      reparse_file(path);
    else
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
	if (audio_list.size() == 0) {
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

    // FIXME: could be nicer in that it could try to find the element that one
    // was standing on before

    // sanity check
    if (folders.top().second > audio_list.size()-1 && mode == ADD)
      folders.top().second = audio_list.size()-1;

    if (!audio_state->fullscreen_info && !printing_information && visible && reparsed_current_dir)
      print(*files);
  }
#endif

  void reparse_current_dir()
  {
    reload_current_dirs();

    // sanity check
    if (folders.top().second > audio_list.size()-1 && mode == ADD)
      folders.top().second = 0;
  }

  virtual std::string get_name_from_file(const T& d) = 0;
  virtual std::string get_short_name_from_file(const T& d) = 0;
  virtual void print(const std::vector<T>& files) = 0;

  bool search_compare(const T& t)
  {
    // optimization
    if (search_str.empty())
      return true;

    std::string name = get_name_from_file(t);

    return (string_format::lowercase(name.substr(0, search_str.size())) == lowercase_search_str);
  }

  virtual void search_func() = 0;

  virtual void options_reloaded() = 0;

  virtual bool reload_dir(const std::string& path) = 0;
  virtual void reparse_file(const std::string& path) = 0;
  virtual void reload_current_dirs() = 0;

  void update_db_current_dirs()
  {
    foreach (string& dir, folders.top().first){
      file_tools::remove_full_dir_from_cache(dir);
      reload_dir(dir);
    }
  }

  void options()
  {
    visible = false;

    S_BusyIndicator::get_instance()->idle();

    std::string random_mode_before = opts.shuffle();

    if (opts.mainloop()) {
      S_BusyIndicator::get_instance()->busy();

      update_db_current_dirs();

      reparse_current_dir();

      input_master->set_map("audio");
      mode = ADD;
      files = &audio_list;

      if (folders.top().second > audio_list.size()-1)
	folders.top().second = 0;

      options_reloaded();

      S_BusyIndicator::get_instance()->idle();
    }

    if (opts.shuffle() != random_mode_before) {
      shuffle_list.randomize();
      audio_state->empty_played_tracks();
    }

    opts.save();

    visible = true;
  }

  void add_all()
  {
    bool added = false;

    foreach (T& item, audio_list) {
      if (item.type != "dir") {
	added = true;

	add_track_to_playlist(item);
      }
    }

    if (added) {
      DialogWaitPrint pdialog(dgettext("mms-audio", "Added directory to playlist"), 1000);
      save_playlist("last");
    } else
      DialogWaitPrint pdialog(dgettext("mms-audio", "Folder is empty"), 1000);
  }

  std::vector<T> get_audio_cd_list()
  {
    // FIXME?
    //    audio_state->p->update_cdrom(cd->get_device());

    // add elements to the playlist

    std::vector<T> audiolist_new;

    T tmp_track;

    Overlay overlay = Overlay("dialog");

    DialogStepPrint pdialog(dgettext("mms-audio", "Looking up CD in database"), overlay);

    CD_Tag *cdtag = new CD_Tag;
    cdtag->verbosity= VERBOSE_ALL;
    if (cdtag->TagCD(cd->get_device().c_str()) == -1) {
      delete cdtag;
      return audiolist_new;
    }

    int _ret = 0;
    int ret = 0;
    std::string _filetitle = "";

    recurse_mkdir(conf->p_var_data_dir(),"cddb_data", &_filetitle);
    _filetitle += cdtag->CDDB_Id();

    if (file_exists(_filetitle) && cdtag->LoadEntryData(_filetitle.c_str()))
      ret = 1;
    else
      ret = cdtag->QueryCDDB();

    pdialog.print_step2();

    if (ret > 1) {
      //Multiple matches....
      ExtraMenu em(dgettext("mms-audio", "Please, choose among:"));
      std::string _str, str;
      for (int i = 0; i < ret; ++i){
	str.clear();
	cdtag->GetEntryInfo(i, "genre", &_str);
	str = _str + ": ";
	cdtag->GetEntryInfo(i, "artist", &_str);
	str += _str;
	str += " - ";
	cdtag->GetEntryInfo(i, "album", &_str);
	str += _str;
	str += " - ";
	cdtag->GetEntryInfo(i, "DYEAR", &_str);
	str += _str;
	em.add_item(ExtraMenuItem(str, "",NULL));
      }
      _ret = 1+em.mainloop();
    } else if (ret == 1)
      _ret = 1;

    std::string _artist = "";
    std::string _album = "";
    std::string _title = "";

    if (_ret >0){
      cdtag->GetEntryInfo(_ret - 1, "artist", &_artist);
      cdtag->GetEntryInfo(_ret - 1, "album", &_album);
      cdtag->SaveEntryData(_ret - 1, _filetitle.c_str() , true);
    }

    for (int i = 1; i <= cdtag->NumOfTracks() ; ++i) {
      if (global->check_stop_bit()) {
	audiolist_new.clear();
	break;
      }

      tmp_track.id = ++id;
      std::ostringstream name,_buf;

      if (i < 10)
	name << "Track 0" << i;
      else
	name << "Track " << i;

      if (_ret >0){
	_buf << "TTITLE" << i-1;
	cdtag->GetEntryInfo(_ret-1, _buf.str().c_str(), &_title);
      }

      if (_ret < 1 || (_artist.empty() && _album.empty() && _title.empty())) {
	tmp_track.name = name.str().substr(0, 8);
      } else
	tmp_track.name = _artist + " - " + _album + " - " + _title;
      tmp_track.lowercase_name = string_format::lowercase(tmp_track.name);
      tmp_track.path = audio_state->p->cd_track_path(i);
      tmp_track.type="media-track";
      tmp_track.media_id=cdtag->CDDB_Id();

      audiolist_new.push_back(tmp_track);
    }

    string cover = "";

    recurse_mkdir(conf->p_var_data_dir(),"cddb_data", &cover);
    cover += cdtag->CDDB_Id();
    cover += ".jpg";

    if (_ret > 0 && !file_exists(cover))
      cdtag->DownloadCDCover(cover.c_str());

    insert_cover_into_db(cdtag->CDDB_Id(), "media-track");

    delete cdtag;
    return audiolist_new;
  }

  virtual void insert_cover_into_db(const std::string& filename, const std::string& type)
  {}

  void radio_action()
  {
    Shoutcast *shoutcast = Shoutcast::get_instance();

    if (radio_pos == 0 && in_genre)
      {
	// self-defined radio stations

	vector<RadioStation> tmp_cur_radio_files;
	vector<string> tmp_print_radio_files;

	int counter = 0;
	foreach (RadioStation& station, radio_stations) {
	  RadioStation tmp = station;
	  tmp.first = itostr(++counter, 3) + ". " + station.first;
	  tmp_print_radio_files.push_back(tmp.first);
	  tmp_cur_radio_files.push_back(tmp);
	}

	if (tmp_cur_radio_files.size() != 0) {
	  cur_radio_files = tmp_cur_radio_files;
	  print_radio_files = tmp_print_radio_files;
	  radio_update_needed = true;
	  in_genre = false;
	  old_radio_pos = radio_pos;
	  radio_pos = 0;
	} else {
	  DialogWaitPrint pdialog(dgettext("mms-audio", "No personal radio stations found"), 2000);
	  return;
	}
      }
    else if (in_genre)
      {
	std::list<TopStation> shoutcast_toplist =
	  shoutcast->get_toplist(vector_lookup(cur_radio_files, radio_pos).second);

	if (shoutcast_toplist.size() == 0) {
	  list<string> messages;
	  messages.push_back(dgettext("mms-audio", "The genre contains no stations"));
	  messages.push_back(dgettext("mms-audio", "or the stations could not be downloaded"));
	  print_dialog_wait(messages, 2000);
	  return;
	}

	print_radio_files.clear();
	cur_radio_files.clear();

	foreach (TopStation& station, shoutcast_toplist) {
	  print_radio_files.push_back(station.first);
	  cur_radio_files.push_back(station);
	}

	radio_update_needed = true;
	in_genre = false;
	old_radio_pos = radio_pos;
	radio_pos = 0;
      }
    else
      {
	std::pair<std::string, std::string> station = vector_lookup(cur_radio_files, radio_pos);

	T s;
	s.id = ++playlist_id;
	s.name = station.first;
	s.lowercase_name = string_format::lowercase(station.first);
	s.path = string_format::trim(station.second);
	s.type = "";

	if (s.path.find("rtsp://") == 0 || s.path.find("rtp://") == 0) {
	  if (!audio_state->p->supports_rtp())
	    DialogWaitPrint pdialog(dgettext("mms-audio", "Alsaplayer does not support rtsp"), 2000);
	  else
	    s.type = "web";
	} else if (s.path.find("http://") == 0)
	  s.type = "web";
	else if (s.path.find("mmsh://") == 0 || s.path.find("mms://") == 0)
	  s.type = "web";

	if (!s.type.empty()) {
	  DialogWaitPrint pdialog(dgettext("mms-audio", "Starting playback"), 1000);
	  audio_state->external_plugin_playback(s);
	}
	else
	  DialogWaitPrint pdialog(dgettext("mms-audio", "Failed to connect to the radio station"), 3000);
      }
  }

  void radio_playlist_add()
  {
    Shoutcast *shoutcast = Shoutcast::get_instance();

    typedef std::pair<std::string, std::string> RadioStation;

    std::pair<std::string, std::string> station = vector_lookup(cur_radio_files, radio_pos);

    T s;
    s.id = ++playlist_id;
    s.name = station.first;
    s.lowercase_name = string_format::lowercase(station.first);
    s.path = station.second;
    s.type = "";

    if (s.path.find("rtsp://") == 0 || s.path.find("rtp://") == 0) {
      if (!audio_state->p->supports_rtp())
	DialogWaitPrint pdialog(dgettext("mms-audio", "Alsaplayer does not support rtsp"), 2000);
      else
	s.type = "web";
    } else if (s.path.find("http://") == 0)
      s.type = "web";
    else if (s.path.find("mmsh://") == 0 || s.path.find("mms://") == 0)
      s.type = "web";

    if (!s.type.empty()) {
      DialogWaitPrint pdialog(dgettext("mms-audio", "Added track to playlist"), 1000);

      add_track_to_playlist(s);

      save_playlist("last");
    }

    if (playlist.size() == 1)
      // first element added so we need to set the pos
      playlist_pos_int = 0;

    if (!audio_state->p_playing()) {
      // setup
      AudioMode old_mode = mode;
      mode = PLAY;
      files = &playlist;
      int old_playlist_pos = playlist_pos_int;
      playlist_pos_int = playlist.size() - 1;

      play_track();

      // reset
      playlist_pos_int = old_playlist_pos;
      files = &audio_list;
      mode = old_mode;
    }
  }

  // audio add specific
  // helper function
  virtual void add_track_to_playlist(T& p)
  {
    p.id = ++playlist_id;
    playlist.push_back(p);
    shuffle_list.add_track(p);

    if (playlist.size() == 1)
      // first element added so we need to set the pos
      playlist_pos_int = 0;
  }

  void simple_add_track_to_playlist(T& p)
  {
    p.id = ++playlist_id;
    playlist.push_back(p);
    shuffle_list.simple_add_track(p);

    if (playlist.size() == 1)
      // first element added so we need to set the pos
      playlist_pos_int = 0;
  }

  virtual void new_playlist()
  {}

  virtual void add_tracks_to_playlist(const std::vector<T>& vp)
  {
    foreach (T p, vp)
      add_track_to_playlist(p);

    save_playlist("last");
  }

  virtual std::vector<T> add_dir(const std::string& dir, bool from_media = false) = 0;

  void add()
  {
    std::string message;

    T file = vector_lookup(*files, position_int());

    // add file or dir to playlist add the buttom of the list
    if (file.type == "dir") {

      add_tracks_to_playlist(add_dir(file.path));

      if (display_message)
	message = dgettext("mms-audio", "Added directory to playlist");

    } else {
      add_track_to_playlist(file);

      save_playlist("last");

      if (display_message)
	message = dgettext("mms-audio", "Added track to playlist");
    }
    if (display_message)
      DialogWaitPrint pdialog(message, 1000);
  }

  void play_track()
  {
    // stop movie playback if needed
    if (global->is_playback("Movie")) {
      Input stop;
      stop.command = "stop";
      stop.mode = "playback";

      global->check_commands(stop);

      global->set_playback("Audio");
    }

    if (audio_state->p->is_playing() && audio_state->p_playing()) {
      LastFM::end_of_song(audio_state->p->p_artist(), audio_state->p->p_album(),
			  audio_state->p->p_title(), audio_state->p->p_total_time(),audio_state->p->p_cur_time());
    }

    T cur_track = vector_lookup(playlist, playlist_pos_int);

    check_mount_before(cur_track.type);

    audio_state->p->set_cur_nr(cur_track);

    audio_state->p->addfile(cur_track);

    audio_state->p->set_streaming(cur_track.type == "web");

    // make sure we reset this if it is set
    audio_state->is_external_source = false;

    check_mount_after(cur_track.type);

    if (audio_state->played_tracks.size() == 0) {
      // notify the random track generation that we want to start with this track
      shuffle_list.track_played(cur_track);
    }

    if (opts.shuffle() == dgettext("mms-audio", "completely random"))
      audio_state->direction = Audio_s::ADDED;

    set_buffering_timeout();
  }

  void prev()
  {
    if (position_int() != 0)
      set_position_int(position_int()-1);
    else
      set_position_int(files->size()-1);
  }

  void next()
  {
    set_position_int((position_int()+1)%files->size());
  }

  virtual void page_up() = 0;
  virtual void page_down() = 0;

  // playlist specific
  void move_up()
  {
    if (playlist.size() != 1) {
      if (playlist_pos_int != 0) {
	T r;
	typename std::vector<T>::iterator j = playlist.begin() + playlist_pos_int;

	r = (*j);
	playlist.erase(j); --j;
	playlist.insert(j++, r);
	--j;

	// move the view
	playlist_pos_int = playlist_pos_int - 1;
      } else {
	typename std::vector<T>::iterator j = playlist.end(); --j;
	T r_end = playlist.back();
	T r_start = playlist.front();

	playlist.erase(j);
	playlist.erase(playlist.begin());
	playlist.push_back(r_start);
	playlist.insert(playlist.begin(), r_end);
	j = playlist.end(); --j;

	// move the view
	playlist_pos_int = playlist.size()-1;
      }

      save_playlist("last");
    }
  }

  void move_down()
  {
    if (playlist.size() != 1) {
      if ((playlist_pos_int + 1) != playlist.size()) {
	T r;
	typename std::vector<T>::iterator j = playlist.begin() + playlist_pos_int + 1;

	r = (*j);
	playlist.erase(j);
	playlist.insert(--j, r);

	// move the view
	playlist_pos_int = playlist_pos_int + 1;
      } else {
	typename std::vector<T>::iterator j = playlist.end(); --j;
	T r_end = playlist.back();
	T r_start = playlist.front();

	playlist.erase(j);
	playlist.erase(playlist.begin());
	playlist.push_back(r_start);
	playlist.insert((playlist.begin()), r_end);
	j = playlist.begin();

	// move the view
	playlist_pos_int = 0;
      }
      save_playlist("last");
    }
  }

  virtual void remove_track_from_playlist(const T& track)
  {
  }

  virtual void remove_all_tracks_from_playlist()
  {
  }

  void delete_track()
  {
    assert(playlist_pos_int < playlist.size());

    typename std::vector<T>::iterator i = playlist.begin() + playlist_pos_int;

    if (files->size() == 1) {

      if (audio_conf->p_audio_stop_on_clear()) {
	audio_state->p->stop();
	Simplefile empty;
	audio_state->p->set_cur_nr(empty);
      }

      // remove from db
      remove_track_from_playlist(*i);

      // remove track if queued
      audio_state->remove_track_from_queue(*i);

      audio_state->remove_track_from_played(*i);

      playlist.clear();
      shuffle_list.clear();

      change_mode = true;

    } else {

      if (audio_state->p->p_cur_nr() == *i) {
	if (!audio_state->p_playing()) {
	  Simplefile empty;
	  audio_state->p->set_cur_nr(empty);
	} else {
	  set_buffering_timeout();
	  audio_state->p->next();
	  update_playlist_view();
	}
      }

      // remove from db
      remove_track_from_playlist(*i);

      // remove track if queued
      audio_state->remove_track_from_queue(*i);

      audio_state->remove_track_from_played(*i);

      shuffle_list.remove_track(*i);
      playlist.erase(i);

      // this is playlist.size(), not -1, because we just deleted a track
      if (playlist_pos_int == playlist.size())
	// the last element of the list
	playlist_pos_int = playlist_pos_int - 1;
    }

    save_playlist("last");
  }

  void queue_track()
  {
    Simplefile cur_track = vector_lookup(*files, position_int());
    if (audio_state->queue_pos(cur_track) == 0)
      audio_state->add_track_to_queue(cur_track);
    else
      audio_state->remove_track_from_queue(cur_track);
  }

  void clear_playlist()
  {
    if (audio_conf->p_audio_stop_on_clear()) {
      audio_state->p->stop();
      Simplefile empty;
      audio_state->p->set_cur_nr(empty);
    }

    audio_state->remove_queued_tracks();

    audio_state->empty_played_tracks();

    remove_all_tracks_from_playlist();

    playlist.clear();
    shuffle_list.clear();

    save_playlist("last");

    change_mode = true;
  }

  void play()
  {
    if (playlist.size() > 0) {
      if (opts.shuffle() == dgettext("mms-audio", "off")) {
	if (audio_state->p->p_cur_nr().path.empty())
	  audio_state->p->set_cur_nr(vector_lookup(playlist, playlist_pos_int));
      } else {
	audio_state->p->set_cur_nr(next_helper(true));
      }

      audio_state->p->set_streaming(audio_state->p->p_cur_nr().type == "web");

      audio_state->p->play();

      // update the screen with the playing track
      update_playlist_view();
    }
  }

  int get_real_playlist_pos()
  {
    Simplefile empty;
    Simplefile cur_track = audio_state->p->p_cur_nr();

    if (cur_track != empty) {
      typename std::vector<T>::iterator j = playlist.begin();

      int p = 0;
      while (j != playlist.end()) {
	if (j->id == cur_track.id)
	  return p;
	else {
	  ++j;
	  ++p;
	}
      }

      return playlist_pos_int;

    } else {
      return playlist_pos_int;
    }
  }

  void toggle_playlist()
  {
    if (input_master->current_map() == "search")
      return;

    if (input_master->current_map() == "playlist" && !in_choose_playlist) { // choose_playlist uses playlist also

      in_playlist = false;

      exit_loop = true;

    } else if (playlist.size() != 0) {

      std::string old_map = input_master->current_map();

      bool pictures_fullscreen_was_running = false;
      bool audio_fullscreen_was_running = false;
      bool epg_was_running = false;

      // disable pictures slideshow when entering audio playlist
      ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
      if (screen_updater->timer.status("pictures")) {
        pictures_fullscreen_was_running = true;
        screen_updater->timer.deactivate("pictures");
        if (!screen_updater->timer.status("audio_fullscreen"))
	  screen_updater->timer.activate("audio_fullscreen");
      }

      if (screen_updater->timer.status("epg")) {
        epg_was_running = true;
        screen_updater->timer.deactivate("epg");
        if (!screen_updater->timer.status("audio_fullscreen"))
          screen_updater->timer.activate("audio_fullscreen");
      }

      bool old_show_track = render->show_audio_track;
      render->show_audio_track = true;

      bool mode_was_add = false;
      if (mode == ADD)
        mode_was_add = true;

      in_playlist = true;

      mainloop(PLAY);

      // restore
      render->show_audio_track = old_show_track;

      if (mode_was_add) {
        files = &audio_list;
        mode = ADD;
      }

      input_master->set_map(old_map);

      if (pictures_fullscreen_was_running) {
        screen_updater->timer.deactivate("audio_fullscreen");
        screen_updater->timer.activate("pictures");
      }

      if (epg_was_running) {
        screen_updater->timer.deactivate("audio_fullscreen");
        screen_updater->timer.activate("epg");
      }

    } else if (playlist.size() == 0)
      DialogWaitPrint pdialog(dgettext("mms-audio", "No tracks in playlist"), 1000);
  }

  void toggle_load_playlist()
  {
    if (list_playlists().size() >= 1) {

      if (choose_playlist()) {
	mode = PLAY;
	files = &playlist;

	playlist_pos_int = 0;
      }

      if (mode == PLAY)
	input_master->set_map("playlist");
      else
	input_master->set_map("audio");

    } else
      Print pdialog(dgettext("mms-audio", "No saved playlists found"), Print::SCREEN);
  }

  virtual Simplefile intelligent_random_next() = 0;

  Simplefile next_helper(bool modify)
  {
    if (playlist_size() == 0) {
      Simplefile s;
      return s;
    }

    Rand *rand = S_Rand::get_instance();

    typename std::vector<T>::iterator last_element = playlist.end();
    typename std::vector<T>::iterator j = playlist.begin();

    --last_element;

    Simplefile cur_nr = audio_state->p->p_cur_nr();
    Simplefile next_track;

    if (audio_state->queue_size() != 0) {

      next_track = audio_state->next_in_queue(modify);

    } else if (opts.shuffle() == dgettext("mms-audio", "completely random")) {

      if (playlist.size() > 1) {
	int r = rand->number(playlist.size());

	while (cur_nr == vector_lookup(playlist, r))
	  r = rand->number(playlist.size());

	next_track = vector_lookup(playlist, r);
	audio_state->direction = Audio_s::ADDED;
      }

    } else if (opts.shuffle() == dgettext("mms-audio", "simple random")) {

      if (modify) {
	while ((next_track = shuffle_list.next_track()) == cur_nr && playlist.size() != 1)
	  ;
      } else {
	while ((next_track = shuffle_list.peek_next_track()) == cur_nr && playlist.size() != 1)
	  ;
      }	

    } else if (opts.shuffle() == dgettext("mms-audio", "intelligent")) {

#ifdef use_imms
      next_track = intelligent_random_next();
#endif

    } else {

      if (cur_nr != (*last_element) || conv::stob(opts.repeat()) && !conv::stob(opts.shutdown())) {

	// the currently selected track was deleted
	if (cur_nr.id == 0) {
	  int p = playlist_pos_int;
	  if (playlist_pos_int >= playlist_size()-1)
	    p = 0;
	  next_track = vector_lookup(playlist, p);
	  return next_track;
	}

	Simplefile c = cur_nr;
        unsigned int t = 0;

	if (!(c == *last_element && conv::stob(opts.repeat()))) {
	
          while (++t < playlist.size()){
            if (j->id == c.id)
              break;
            j++;
          }

          if ( t < playlist.size())
	    j++;
	}

	next_track = *j;
      }
    }

    if (modify && audio_state->p->p_cur_nr().id != 0) // deleted track
      audio_state->add_track_to_played(audio_state->p->p_cur_nr());

    return next_track;
  }

  virtual void intelligent_random_prev() = 0;

  void prev_helper()
  {
    if (playlist_size() == 0) {
      Simplefile s;
      audio_state->p->set_cur_nr(s);
      return;
    }

    Rand *rand = S_Rand::get_instance();

    typename std::vector<T>::iterator j = playlist.begin();

    Simplefile cur_nr = audio_state->p->p_cur_nr();

    if (opts.shuffle() == dgettext("mms-audio", "completely random")) {

      Simplefile prev_track_played = audio_state->prev_track_played();

      if (audio_state->direction == Audio_s::ADDED && prev_track_played.id != 0) {
	prev_track_played = audio_state->prev_track_played();
	audio_state->direction = Audio_s::TAKEN;
      }

      if (prev_track_played.id != 0) {

	audio_state->p->set_cur_nr(prev_track_played);

      } else {

	if (playlist.size() > 1) {
	  // empty
	  int r = rand->number(playlist.size());

	  while (cur_nr == vector_lookup(playlist, r))
	    r = rand->number(playlist.size());

	  audio_state->p->set_cur_nr(vector_lookup(playlist, r));
	  audio_state->direction = Audio_s::ADDED;
	}
      }

    } else if (opts.shuffle() == dgettext("mms-audio", "simple random")) {

      Simplefile prev_nr;
      while ((prev_nr = shuffle_list.prev_track()) == cur_nr && playlist.size() != 1)
	;
      audio_state->p->set_cur_nr(prev_nr);

    } else if (opts.shuffle() == dgettext("mms-audio", "intelligent")) {

#ifdef use_imms
      Simplefile prev_track_played = audio_state->prev_track_played();

      if (audio_state->direction == Audio_s::ADDED && prev_track_played.id != 0) {
	prev_track_played = audio_state->prev_track_played();
	audio_state->direction = Audio_s::TAKEN;
      }

      if (prev_track_played.id != 0) {

	audio_state->p->set_cur_nr(prev_track_played);

      } else {

	intelligent_random_prev();

      }
#endif

    } else {

      Simplefile c = cur_nr;

      while (j != playlist.end()) {

	if (j->id == c.id)
	  break;
	else
	  ++j;
      }

      if (cur_nr == (*playlist.begin()))
	j = playlist.end();

      --j;

      audio_state->p->set_cur_nr(*j);
    }

    audio_state->add_track_to_played(audio_state->p->p_cur_nr());
  }

  // The following function for parsing audio dirs.
  std::vector<T> parse_dir(const std::list<std::string>& dirs)
  {
    std::vector<T> cur_files;

    foreach (const std::string& dir, dirs)
      {
	// Add files
	assert(dir.size() > 0);
	std::vector<T> tempfiles = rdir(dir);
	cur_files.insert(cur_files.end(), tempfiles.begin(), tempfiles.end());
      }

    return cur_files;
  }

  void load_current_dirs()
  {
    audio_list = parse_dir(folders.top().first);

    if (audio_folders.size() >= 1)
      std::sort(audio_list.begin(), audio_list.end(), file_sort());
  }

  void cd_menu()
  {
    input_master->set_map("audio");

    int tracksize = cd->get_nr_tracks_cd();

    if (tracksize == -1) {
      Print pdialog(dgettext("mms-audio", "The disc does not contain any recognizable files"),
		    Print::SCREEN);
      return;
    }

    ExtraMenu em("Audio CD playback");

    em.add_item(ExtraMenuItem(dgettext("mms-audio", "Add CD to playlist and start playback"), "",
			      boost::bind(&AudioTemplate<T>::add_audio_cd, this)));

    em.add_item(ExtraMenuItem(dgettext("mms-audio", "Examine CD"), "",
			      boost::bind(&AudioTemplate<T>::examine_audio_cd, this)));

    em.mainloop();
  }

  void examine_audio_cd()
  {
    std::vector<T> old_audio_list = audio_list;
    std::stack<std::pair<std::list<std::string>, int > > old_folders = folders;

    std::vector<T> audiolist_new = get_audio_cd_list();

    if (audiolist_new.size() == 0) {
      Print pdialogs(dgettext("mms-audio", "The disc does not contain any recognizable files"),
		     Print::SCREEN);
    } else {

      audio_list.clear();
      while (folders.size() > 0)
	folders.pop();

      foreach (T& item, audiolist_new)
	audio_list.push_back(item);

      std::list<std::string> dirs;
      dirs.push_back(cd->get_mount_point());
      folders.push(std::make_pair(dirs, 0));

      mainloop(ADD);

      // restore old setting
      audio_list = old_audio_list;
      folders = old_folders;
    }
  }

  void add_audio_cd()
  {
    std::vector<T> audiolist_new = get_audio_cd_list();

    if (audiolist_new.size() == 0) {
      Print pdialogs(dgettext("mms-audio", "The disc does not contain any recognizable files"),
		     Print::SCREEN);
    } else {

      int playlist_pos = playlist.size();

      add_tracks_to_playlist(audiolist_new);

      playlist_pos_int = playlist_pos;

      // play the first track
      mode = PLAY;
      files = &playlist;
      play_track();

      mainloop(PLAY);
    }
  }

  void add_data_cd()
  {
    std::vector<T> old_audio_list = audio_list;
    std::stack<std::pair<std::list<std::string>, int > > old_folders = folders;

    //mount the disc
    run::external_program("mount '" + cd->get_mount_point() + "'");

    std::vector<T> audiolist_new = rdir(cd->get_mount_point());

    if (audiolist_new.size() == 0) {
      Print pdialog(dgettext("mms-audio", "The disc does not contain any recognizable files"),
		    Print::SCREEN);

      //umount the disc
      run::external_program("umount '" + cd->get_mount_point() + "'");

    } else {

      audio_list.clear();
      while (folders.size() > 0)
	folders.pop();

      audio_list = audiolist_new;

      std::list<std::string> dirs;
      dirs.push_back(cd->get_mount_point());
      folders.push(std::make_pair(dirs, 0));

      mainloop(ADD);

      //umount the disc
      run::external_program("umount '" + cd->get_mount_point() + "'");

      // restore old setting
      audio_list = old_audio_list;
      folders = old_folders;
    }
  }

  void save_playlist(const std::string& playlistfile, bool overwrite = false)
  {
    std::vector<T> cur_files = playlist;

    if (playlistfile == "") {
      DialogWaitPrint pdialog(3000);
      pdialog.add_line(dgettext("mms-audio", "Can not save playlist with an empty name"));
      pdialog.add_line("");
      pdialog.add_line(dgettext("mms-audio", "Please use another name"));
      pdialog.print();
      return;
    }

    std::string _str;
    // doesn't make any sense to save nothing
    if ((cur_files.size() != 0 || playlistfile == "last") && recurse_mkdir(conf->p_var_data_dir(),"playlists",&_str)) {

      if (!overwrite && playlistfile != "last" && file_exists(_str + playlistfile)) {
	DialogWaitPrint pdialog(3000);
	pdialog.add_line(dgettext("mms-audio", "Playlist with the chosen name already exists"));
	pdialog.add_line("");
	pdialog.add_line(dgettext("mms-audio", "Please use another name"));
	pdialog.print();
	return;
      }

      std::ofstream file;

      std::string plist = conf->p_var_data_dir() + "playlists/" + playlistfile;

      file.open(plist.c_str());

      if (!file) {
	print_critical(dgettext("mms-audio", "Could not write playlist to file: ") + plist, "AUDIO");
      } else {

	foreach (T& cur_file, cur_files) {

	  if (cur_file.type == "web")
	    file << cur_file.type << "," << cur_file.path << ";" << cur_file.name << std::endl;
	  else if (cur_file.type == "media-track")
	    file << cur_file.type << "," << cur_file.media_id << ";" << cur_file.path << std::endl;
	  else
	    file << cur_file.type << "," << cur_file.path << std::endl;

	}
      }

      file.close();
    }
  }

  void load_media_track(const std::string& filename, const std::string& cdid,
			std::vector<T>& cur_files,
			bool& cdrom_set, bool& media_checked, bool& media_ok)
  {
    T file;
    file.id = ++playlist_id;
    std::string _str, _artist="", _album="", _title="", _filetitle="", _id="", _tracknum="";

    // init it if it's not already done
    if (!audio_conf->p_audio_started())
      audio_state = S_Audio_s::get_instance();

#if 0  // FIXME
    if (!cdrom_set) {
      audio_state->p->update_cdrom(cd->get_device());
      cdrom_set = true;
    }
#endif

    if (!media_checked) {

      media_checked = true;
      bool opened = false;

      if (cd->cdrom == -1)  {
	if (!cd->open()) {
	  media_ok = false;
	  return;
	}
	opened = true;
      }

      Cd::cdstatus cdstatus = cd->check_cddrive();
      if (cdstatus == Cd::OK)
	media_ok = true;
      else
	media_ok = false;

      if (opened)
	cd->close();
    }

    CD_Tag * cdtag = new CD_Tag;
    cdtag->verbosity = VERBOSE_ALL;
    if (media_ok && (cdtag->TagCD(cd->get_device().c_str()) != -1)) {
      _id = cdtag->CDDB_Id();
      if (_id != cdid){
	delete cdtag;
	print_warning(dgettext("mms-audio", "Skipping track not present on the current media"), "AUDIO");
	return;
      }

      size_t _pos = 0;
      if (filename.find("Track ") == 0)
	_pos=6;
      else if (filename.find("cdda://") == 0)
	_pos=7;
      else if (filename.find("cdda:/") == 0)
	_pos=6;
      else {
	delete cdtag;
	print_warning(dgettext("mms-audio", "Playlist malformed, skipping line"), "AUDIO");
	return;
      }

      _tracknum=filename.substr(_pos, 2);
      //Sanity checks.....
      if (_tracknum.size() < 1 || _tracknum[0] < '0' || _tracknum[0]> '9') {
	delete cdtag;
	print_warning(dgettext("mms-audio", "Playlist malformed, skipping line"), "AUDIO");
	return;
      }

      if (_tracknum.size()==2 && (_tracknum[1] < '0' || _tracknum[1]>'9')) {
	delete cdtag;
	print_warning(dgettext("mms-audio", "Playlist malformed, skipping line"), "AUDIO");
	return;
      }

      int a = conv::atoi(_tracknum);

      recurse_mkdir(conf->p_var_data_dir(),"cddb_data", &_filetitle);
      _filetitle += _id;

      if (file_exists(_filetitle) && cdtag->LoadEntryData(_filetitle.c_str())){
	cdtag->GetEntryInfo(0, "artist", &_artist);
	cdtag->GetEntryInfo(0, "album", &_album);
	std::ostringstream _buf;
	_buf << "TTITLE"<< a-1;
	cdtag->GetEntryInfo(0, _buf.str().c_str(), &_title);
      }

      if (_artist.empty() && _album.empty() && _title.empty()) {
	file.name = "Track " + _tracknum;
      } else
	file.name = _artist + " - " + _album + " - " + _title;

      file.lowercase_name = string_format::lowercase(file.name);

      file.path = filename;
      file.type="media-track";
      file.media_id= cdid;

      cur_files.push_back(file);
    }

    delete cdtag;
  }

  void load_media_file(const std::string& filename, std::vector<T>& cur_files,
		       bool& media_mounted, bool& media_checked, bool& media_ok)
  {
    if (!media_checked) {
      media_checked = true;
      bool opened = false;

      if (cd->cdrom == -1) {
	if (!cd->open()) {
	  media_ok = false;
	  return;
	}
	opened = true;
      }

      Cd::cdstatus cdstatus = cd->check_cddrive();
      if (cdstatus == Cd::OK)
	media_ok = true;
      else
	media_ok = false;

      if (opened)
	cd->close();
    }

    if (media_ok) {
      // mount the drive if isn't mounted yet
      if (!media_mounted) {
	run::external_program("mount '" + cd->get_mount_point() + "'");
	media_mounted = true;
      }

      if (file_exists(filename)) {
	T file(addsimplefile(filename, check_type(filename, audio_conf->p_filetypes_a()), true));
	file.id = ++playlist_id;

	cur_files.push_back(file);
      }
    }
  }

  void load_radio_file(const std::string& filename, const std::string& name,
		       const std::string& type, std::vector<T>& cur_files)
  {
    if ((filename.find("rtsp://") == 0 || filename.find("rtp://") == 0) && !audio_state->p->supports_rtp())
      return;

    T file;
    file.id = ++playlist_id;
    file.name = name;
    file.lowercase_name = string_format::lowercase(name);
    file.path = filename;
    file.type = type;

    cur_files.push_back(file);
  }

  bool load_last_playlist()
  {
    std::cout << "loading last saved playlist" << std::endl;
    bool status = load_playlist("last");
    save_playlist("last"); /* this purges the saved playlist from CD track entries that are no longer current */
    std::pair<int, bool> loaded_values = audio_state->p->load_runtime_settings();

    playlist_pos_int = loaded_values.first;
    play_now_warning = loaded_values.second;

    // sanity check
    if (playlist_pos_int >= playlist.size())
      playlist_pos_int = 0;

    return status;
  }

  // set db_id
  virtual void ids(T& p)
  {
  }

  bool load_playlist(const std::string& playlistfile)
  {
    std::vector<T> cur_files;

    std::ostringstream playlist_path;
    playlist_path << conf->p_var_data_dir() + "playlists/" << playlistfile;

    std::ifstream In(playlist_path.str().c_str());

    std::string str, type = "", filename, name, cd_id;

    bool media_mounted = false;
    bool cdrom_set = false;

    bool media_checked = false;
    bool media_ok = false;

    if(!In){
      print_critical(dgettext("mms-audio", "Could not open playlist: ") + playlist_path.str(), "AUDIO");
    } else {

      while (getline(In, str)) {

	std::string::size_type pos_comma = str.find(",");

	if (pos_comma != std::string::npos) {
	  type = str.substr(0, pos_comma);

	  if (type == "web") {
	    int pos_semi = str.rfind(";");
	    if (pos_semi == std::string::npos) {
	      print_warning(dgettext("mms-audio", "Playlist malformed, skipping line"), "AUDIO");
	      continue;
	    } else {
	      filename = str.substr(pos_comma+1, pos_semi-(pos_comma+1));
	      name = str.substr(pos_semi+1);
	    }
	  } else if (type == "media-track"){
	    int pos_semi = str.rfind(";");
	    if (pos_semi == std::string::npos) {
	      print_warning(dgettext("mms-audio", "Playlist malformed, skipping line"), "AUDIO");
	      continue;
	    }
	    else {
	      cd_id  = str.substr(pos_comma+1, pos_semi-(pos_comma+1));
	      filename = str.substr(pos_semi+1);
	    }
	  }  else
	    filename = str.substr(pos_comma+1);
	} else
	  continue;

	if (type == "media-track" && conf->p_media()) {

	  load_media_track(filename, cd_id, cur_files, cdrom_set, media_checked, media_ok);

	} else if (type == "media-file" && conf->p_media()) {

	  load_media_file(filename, cur_files, media_mounted, media_checked, media_ok);

	} else if (type == "web") {

	  load_radio_file(filename, name, type, cur_files);

	} else if (type != "media-track" && type != "media-file") {
	  if (file_exists(filename)) {
	    T file(addsimplefile(filename, check_type(filename, audio_conf->p_filetypes_a())));

	    ids(file);

	    file.id = ++playlist_id;

	    cur_files.push_back(file);
	  }
	}
      }

      if (media_mounted)
	run::external_program("umount '" + cd->get_mount_point() + "'");

      if (cur_files.size() != 0) {

	set_playlist(cur_files);

	return true;

      } else {

	return false;

      }

    }
    return false;
  }

  void set_playlist(const std::vector<T>& cur_files)
  {
    shuffle_list.clear();
    audio_state->remove_queued_tracks();
    playlist.clear();

    Simplefile s;
    audio_state->p->set_cur_nr(s);

    new_playlist();
    
    add_tracks_to_playlist(cur_files); // also adds to shuffle list
  }

  void check_audio_state(){
    if (audio_conf->p_audio_started()) {

      audio_state->p->gather_info();

      if (audio_state->p->delegate_eo_track())
        next_audio_track();
    }
  }

  void next_audio_track()
  {
    bool is_buffering = buffering_time_end != -1 && buffering_time_end - time(0) > 0;

    if (is_buffering && audio_state->p->is_playing() && audio_state->p_playing())
      buffering_time_end = -1;

    // should we play the next track?
    if (!audio_state->p->streaming() &&
        (audio_state->p->play_next() ||
        !audio_state->p->is_playing() && audio_state->p_playing() &&
        !audio_state->p_pause() && !is_buffering)) {

      set_buffering_timeout();
      if (audio_state->suspended_playback())
        audio_state->restore_playback();
      
      // random modes takes care of stopping when the last track has been played
      else if (!audio_state->is_external_source &&
	       ((opts.shuffle() == dgettext("mms-audio", "off") &&
		 audio_state->p->p_cur_nr() != playlist.back()) ||
                opts.shuffle() != dgettext("mms-audio", "off") || 
                conv::stob(opts.repeat()) ||
                audio_state->queue_size() > 0)) {

	// next takes care of checking random modes
        audio_state->p->next();
        update_playlist_view();
      } else {
        if (audio_state->is_external_source)
          audio_state->is_external_source = false;
        
        audio_state->p->stop();

	// we reached the end stop playing
        audio_state->set_playing(false);
        update_playlist_view();
        

	// make pressing play start from the first track again
        playlist_pos_int = 0;
        Simplefile s;
        audio_state->p->set_cur_nr(s);
        check_shutdown();
      }
    }
  }

  bool is_ssaver_running(){
    pthread_mutex_lock(&ssaver_mut);
    bool ret = saverrunning;
    pthread_mutex_unlock(&ssaver_mut);
    return ret;
  }

  void init_ssaver_thread(){
    pthread_create(&thread_run, NULL, (void *(*)(void *))(pre_run), this);
  }

  void kill_ssaver_thread(){
    if(!ssaver_thread_initted)
      return;
    pthread_mutex_lock(&ssaver_mut);
    ssaver_die = true;
    pthread_cond_broadcast(&ssaver_switch);
    pthread_mutex_unlock(&ssaver_mut);
    pthread_join(thread_run, NULL);
  }

  static void * pre_run(void * ptr){
    AudioTemplate * thisobj = reinterpret_cast<AudioTemplate*>(ptr);
    thisobj->print_audio_fullscreen();
    return NULL;
  }
  
  void print_audio_fullscreen(){
    /* Usual loop... */
//    fprintf(stderr, "ssaver thread started\n");
    ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
    ssaver_thread_initted = true;
    if (conf->p_idle_time() <= 0) /* Screensaver is disabled... oh well, it's been quick but intense */
      return;
    pthread_mutex_lock(&ssaver_mut);
    saverrunning = false;
    bool was_saverrunning = saverrunning;
    struct timespec ts = time_helper::compute_interval(conf->p_idle_time()*60*1000);
    ssaver_bool = false;

    while(!ssaver_die){
      int ret = pthread_cond_timedwait(&ssaver_switch, &ssaver_mut, &ts);

      if(ssaver_die)
        break;
      if (ret != ETIMEDOUT && !ssaver_bool)
        continue;
      if (ret == ETIMEDOUT && !ssaver_bool){
        /* Timeout. If the screensaver is not running, check the last time a key was pressed and, in case,
        switch the ssaver on */
        if (!saverrunning && !was_saverrunning && !screen_updater->timer.status("pictures") && (time(NULL)-conf->p_last_key()) >= 60*conf->p_idle_time() && audio_state->p_playing()){
          saverrunning = true;
          was_saverrunning = true;
          prepare_audio_fullscreen();
        }
        if (saverrunning && audio_state->p_playing()){
          if (opts.lyrics() == dgettext("mms-audio", "yes")) {
            string buffer, artist, album, title;
            get_audiotrack_info(buffer, artist, album, title);
            S_Lyrics::get_instance()->find_lyric(artist, title);
          }
          was_saverrunning = saverrunning;
          audio_state->fullscreen_info = true;
          global->playback_in_fullscreen = true;
          audio_state->playback_fullscreen_shown = true;
          print_audio_screensaver();
          ts = time_helper::compute_interval(250); /* next loop in 1/4 second */
          continue;
        }
        else { /* we got a time-out but either there has been a new key pressed, or mms is not playing */
          if (saverrunning){ /* mms stopped playing */
            cleanup_audio_fullscreen();
            saverrunning = false;
            was_saverrunning = saverrunning;
            S_Config::get_instance()->s_last_key(time(NULL)); /* reset the last key pressed timer */
          }
          int64_t ms = (conf->p_idle_time()*60 - (time(NULL) - conf->p_last_key())) * 1000;
          if (ms < 0) /* mms is not playing */
            ms = conf->p_idle_time()*60*1000;
          ts = time_helper::compute_interval(ms);
          continue;
        }
      }
      else { /* ssaver_bool is true and no timeout. */
        ssaver_bool = false;
        if (!saverrunning && was_saverrunning){ /* screensaver was killed externally */
          cleanup_audio_fullscreen();
          saverrunning = false;
          was_saverrunning = saverrunning;
          //S_Config::get_instance()->s_last_key(time(NULL)); /* reset the last key pressed timer */
          ts = time_helper::compute_interval(conf->p_idle_time()*60*1000);
        }
        else if (!saverrunning)
          ts = time_helper::compute_interval(250); /* 1/4 second just to be sure */
      }
    } /* while(!ssaver_die) */
    pthread_mutex_unlock(&ssaver_mut);
  }

  void prepare_audio_fullscreen(){
    render->device->animation_section_begin();
    curlayer = render->device->get_current_layer();
    render->device->switch_to_layer(10);
    render->device->animation_fade(0, 1, 100, 10);
    render->device->animation_fade(1, 0, 100, curlayer);
    render->device->animation_section_end();
    std::vector<std::string> vect;
    vect.push_back("prev_track");
    vect.push_back("next_track");
    vect.push_back("play");
    vect.push_back("pause");
    vect.push_back("stop");
    vect.push_back("ff");
    vect.push_back("fb");
    vect.push_back("mute");
    vect.push_back("vol+");
    vect.push_back("vol-");
    vect.push_back("mute");
    input_master->set_filter(vect);
    S_Touch::get_instance()->enabled = false;
  }

  void cleanup_audio_fullscreen()
  {
    //render->device->switch_to_layer(10);
    render->device->animation_section_begin();

    render->device->animation_fade(1, 0, 100, 10);
    render->device->animation_fade(0, 1, 100, curlayer);

    render->device->switch_to_layer(curlayer);
    render->device->animation_section_end();
    input_master->unset_filter();
    S_Touch::get_instance()->enabled = true;

    if (audio_state->playback_fullscreen_shown) {
      render->wait_and_aquire();
      audio_state->playback_fullscreen_shown = false;
      audio_state->fullscreen_info = false;
      global->playback_in_fullscreen = false;
      audio_state->playback_fullscreen.cleanup();
      render->complete_redraw();
      render->image_mut.leaveMutex();
      if (in_playlist)
        print(*files);

      #ifdef use_notify_area
      // enable notify area if are disabled by screensaver
      if (!audio_conf->p_na_on_scr() && is_screensaver_lyrics())
        nArea->enable();
      #endif
    }
  }

  void stop_ssaver(){
    pthread_mutex_lock(&ssaver_mut);
    ssaver_bool = true;
    saverrunning = false;
    pthread_cond_broadcast(&ssaver_switch); /* wakes thread up */
    pthread_mutex_unlock(&ssaver_mut);
  }
  
  void start_ssaver(){
    pthread_mutex_lock(&ssaver_mut);
    ssaver_bool = true;
    S_Config::get_instance()->s_last_key(0); /* TODO: find a better way, this can't be called by an input hook */
    pthread_cond_broadcast(&ssaver_switch); /* wakes thread up */
    pthread_mutex_unlock(&ssaver_mut);
  }

  virtual std::vector<T> rdir(const std::string& argv) = 0;

  std::vector<T> *files;

  // for changing mode
  std::vector<T> audio_list;

  std::vector<T> playlist;

public:

  virtual ~AudioTemplate(){
    kill_ssaver_thread();
    pthread_mutex_destroy(&ssaver_mut);
    pthread_cond_destroy(&ssaver_switch);
  }

  void reset()
  {
    for (int i = 0, size = folders.size(); i < size; ++i)
      folders.pop();

    folders.push(std::make_pair(audio_folders, 0));

    files = &audio_list;
    mode = ADD;
  }

  std::string get_playlist_element(int index)
  {
    return vector_lookup(playlist, index).path;
  }

  int audiolist_size() { return audio_list.size(); }
  int playlist_size() { return playlist.size(); }

  bool last_element_in_playlist()
  {
    return (audio_state->p->p_cur_nr() == playlist.back());
  }
};

#endif
