// used for #ifdef
#include "config.h"

#include "libfspp.hpp"

#include "audio.hpp"

#include "common.hpp"
#include "graphics.hpp"
#include "print.hpp"
#include "plugins.hpp"
#include "gettext.hpp"

#include "updater.hpp"
#include "extra_menu.hpp"

#include "input.hpp"
#include "printer.hpp"
#include "resolution.hpp"

#include "boost.hpp"

// cd
#include <errno.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "kernelheaders.hpp"

// time
#include <time.h>

// sleep
#include <unistd.h>

#ifdef use_nls
#include <langinfo.h>
#include <locale.h>
#endif

using std::list;
using std::vector;
using std::string;

Audio::Audio()
  : exit_loop(false), change_mode(false),
    printing_information(false),
    mounted_media(false),
    playlist_id(0), opts_offset(46),
    was_playing(false), was_paused(false), was_started(false),
    radio_stations_downloaded(false),
    disabled_busy_indicator(false), display_message(true),
    exit_choose_playlist_loop(false), in_choose_playlist(false),
    navigating_media(false), buffering_time_end(-1)
{
  audio_conf = S_AudioConfig::get_instance();
  radio_conf = S_RadioConfig::get_instance();

  mode = ADD;

  audio_state = S_Audio_s::get_instance();
  audio_state->set_audio(this);

  set_folders();

  res_dependant_calc();

  S_ResolutionManagement::get_instance()->register_callback(boost::bind(&Audio::res_dependant_calc, this));
}

void Audio::res_dependant_calc()
{
  header_font = graphics::resolution_dependant_font_wrapper(28, conf);
  search_font = graphics::resolution_dependant_font_wrapper(22, conf);
  search_select_font = graphics::resolution_dependant_font_wrapper(17, conf);
  normal_font = graphics::resolution_dependant_font_wrapper(13, conf);
  position_font = graphics::resolution_dependant_font_wrapper(18, conf);
  dialog_header = graphics::resolution_dependant_font_wrapper(20, conf);
  information_bigger_font = graphics::resolution_dependant_font_wrapper(20, conf);
  information_font = graphics::resolution_dependant_font_wrapper(18, conf);
  list_font = graphics::resolution_dependant_font_wrapper(16, conf);

  button_playback_font = graphics::resolution_dependant_font_wrapper(17, conf);
  volume_font = graphics::resolution_dependant_font_wrapper(22, conf);

  screensaver_lyrics_artist_font = graphics::resolution_dependant_font_wrapper(24, conf);
  screensaver_lyrics_album_font = graphics::resolution_dependant_font_wrapper(21, conf);
  screensaver_lyrics_normal_font = graphics::resolution_dependant_font_wrapper(18, conf);
  screensaver_lyric_font = graphics::resolution_dependant_font_wrapper(22, conf);

  screensaver_artist_font = graphics::resolution_dependant_font_wrapper(28, conf);
  screensaver_album_font = graphics::resolution_dependant_font_wrapper(25, conf);
  screensaver_normal_font = graphics::resolution_dependant_font_wrapper(22, conf);

  button_playback_height = graphics::calculate_font_height(button_playback_font, conf);
  list_font_height = graphics::calculate_font_height(list_font, conf);
  normal_font_height = graphics::calculate_font_height(normal_font, conf);

  header_size = string_format::calculate_string_size("abcltuwHPMjJg", header_font);
  header_box_size = static_cast<int>(header_size.second * 0.75);

  screensaver_artist_font_height = graphics::calculate_font_height(screensaver_artist_font, conf);
  screensaver_album_font_height = graphics::calculate_font_height(screensaver_album_font, conf);
  screensaver_normal_font_height = graphics::calculate_font_height(screensaver_normal_font, conf);

  screensaver_lyrics_artist_font_height = graphics::calculate_font_height(screensaver_lyrics_artist_font, conf);
  screensaver_lyrics_album_font_height = graphics::calculate_font_height(screensaver_lyrics_album_font, conf);
  screensaver_lyrics_normal_font_height = graphics::calculate_font_height(screensaver_lyrics_normal_font, conf);
}

void Audio::set_folders()
{
  audio_folders.clear();
  list<string> audio_dirs = audio_conf->p_audio_dirs(); // make compiler happy

  // make sure audio folders are not malformed
  foreach (string& dir, audio_dirs) {
    if (dir[dir.size()-1] != '/') {
      audio_folders.push_back(dir + '/');
    } else
      audio_folders.push_back(dir);
  }

#ifdef use_inotify
  S_Notify::get_instance()->register_plugin("audio", audio_folders,
					    boost::bind(&Audio::fs_change, this, _1, _2));
#endif

  list<string> audio_dirs_no_watch = audio_conf->p_audio_dirs_no_watch(); // make compiler happy
  // make sure audio folders are not malformed
  foreach (string& dir, audio_dirs_no_watch) {
    if (dir[dir.size()-1] != '/') {
      audio_folders.push_back(dir + '/');
    } else
      audio_folders.push_back(dir);
  }
}

void Audio::save_runtime_settings()
{
  std::ofstream file;
  recurse_mkdir(conf->p_var_data_dir(), "options", NULL);
  string path = conf->p_var_data_dir() + "options/AudioRuntime";

  file.open(path.c_str());

  if (!file) {
    print_critical(dgettext("mms-audio", "Could not write options to file ") + path, "AUDIO");
  } else {
    if (audio_state->p != 0) {
      file << "volume," << audio_state->p->getvol() << std::endl;
      file << "playlist_pos," << get_real_playlist_pos() << std::endl;
    }
    file << "play_now_warning," << play_now_warning << std::endl;
  }

  file.close();
}

void Audio::begin_external()
{
  if (!was_started) { // protect against calling this function twice
    was_playing = false;
    was_started = false;
  }

  if (audio_conf->p_audio_started() && audio_state->p->loaded()) {
    was_started = true;

    if (audio_state->p->is_playing()) {
      was_playing = true;
      was_paused = audio_state->p_pause();
      audio_state->suspend_playback();
    }

    audio_state->p->release_device();
  }
}

void Audio::end_external()
{
  audio_state->p->restore_device();

  if (was_started) {

    was_started = false;

    if (was_playing) {
      audio_state->restore_playback();

      // restore volume
      audio_state->p->setvol(audio_state->p->getvol());

      if (was_paused)
	audio_state->p->pause();
    }
  }
}

void Audio::check_mount_before(const std::string& type)
{
  if (!mounted_media && type == "media-file") {
    run::external_program("mount '" + cd->get_mount_point() + "'");
    mounted_media = true;
  }
}

void Audio::check_mount_after(const std::string& type)
{
  if (mounted_media && type != "media-file") {
    run::external_program("umount '" + cd->get_mount_point() + "'");
    mounted_media = false;
  }
}

void Audio::pos_umount()
{
  if (mounted_media) {
    run::external_program("umount '" + cd->get_mount_point() + "'");
    mounted_media = false;
  }
}

bool Audio::file_sort::operator()(const Simplefile& lhs, const Simplefile& rhs)
{
  return sort_order(lhs.lowercase_name, rhs.lowercase_name,
		    lhs.path, rhs.path, lhs.type == "dir", rhs.type == "dir",
		    get_class<Audio>(dgettext("mms-audio", "Audio"))->get_opts()->dir_order());
}

// commands
void Audio::exit()
{
  exit_loop = true;
}

void Audio::exit_playlist()
{
  exit_loop = true;
  exit_fall_through = true;
}

void Audio::leave_dir()
{
  if (folders.size() > 1) {
    folders.pop();

    load_current_dirs();
  }
}

string save_file_helper(const string& text)
{
  return dgettext("mms-audio", "Enter filename: ") + text;
}

void Audio::save_playlist_func()
{
  S_BusyIndicator::get_instance()->idle();

  string name = "";
  list<Input> exit_inputs;
  Input i;
  i.command = "back"; i.mode = "general";
  exit_inputs.push_back(i);
  Overlay o("fixed size dialog");

  std::list<string> empty;
  empty.push_back("");

  search_mode = true;
  search_top_size = (conf->p_v_res()-20)/2;

  print_marked_search_letter();

  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
  screen_updater->timer.deactivate("audio_fullscreen");

  screen_updater->trigger.add(TriggerElement("search marker", empty,
					     boost::bind(&Audio::print_marked_search_letter, this),
					     boost::bind(&Audio::clean_up_search_print, this)));
  screen_updater->timer.add(TimeElement("search marker",
					boost::bind(&Audio::check_search_letter, this),
					boost::bind(&Audio::print_marked_search_letter, this)));

  std::pair<bool, Input> res = input_master->generate_string(name, search_help_offset, boost::bind(&print_dialog_fixed_size, boost::bind(&save_file_helper, _1), conf->p_h_res()/2, boost::ref(o)), exit_inputs);

  screen_updater->trigger.del("search marker");
  screen_updater->timer.del("search marker");

  force_search_print_cleanup();

  screen_updater->timer.activate("audio_fullscreen");

  search_mode = false;

  Render *render = S_Render::get_instance();
  render->wait_and_aquire();
  o.cleanup();
  render->image_mut.leaveMutex();

  mmsUsleep(500*1000);
  if (res.first)
    save_playlist(name);
}

string Audio::format_time(int cur_time, int total_time)
{
  char buffer [512];
  if (opts.time_mode() == dgettext("mms-audio", "from start")) {
    snprintf(buffer, 512, "%02d:%02d / %02d:%02d", seconds_to_minutes(cur_time),
	   remaining_seconds(cur_time), seconds_to_minutes(total_time), remaining_seconds(total_time));
  } else {
    int regresive_time = total_time - cur_time;
    snprintf(buffer, 512, "%02d:%02d / %02d:%02d", seconds_to_minutes(regresive_time),
	   remaining_seconds(regresive_time), seconds_to_minutes(total_time), remaining_seconds(total_time));
  }
  return buffer;
}

string Audio::format_time(int time)
{
  char buffer [512];
  snprintf(buffer, 512, "%02d:%02d", seconds_to_minutes(time), remaining_seconds(time));
  return buffer;
}

string Audio::lcd_format_time(int cur_time, int total_time)
{
  char buffer [512];
  char rotating_slash[4];

  rotating_slash[0] = '-';
  rotating_slash[1] = '\\';
  rotating_slash[2] = '|';
  rotating_slash[3] = '/';

  snprintf(buffer, 512, "%02d:%02d \\%01c %02d:%02d", seconds_to_minutes(cur_time),
    remaining_seconds(cur_time), rotating_slash[cur_time%4], seconds_to_minutes(total_time), remaining_seconds(total_time));
  return buffer;
}

const int Audio::position_int()
{
  if (mode == ADD)
    return folders.top().second;
  else
    return playlist_pos_int;
}

void Audio::set_position_int(int new_pos)
{
  if (mode == ADD)
    folders.top().second = new_pos;
  else
    playlist_pos_int = new_pos;
}

bool Audio::choose_playlist()
{
  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();

  input_master->save_map();
  input_master->set_map("playlist");

  Shutdown *sd = S_Shutdown::get_instance();

  Input input;

  saved_playlists = list_playlists();

  if (audio_conf->p_last_fm_username() != "") {
    saved_playlists.push_back(dgettext("mms-audio", "last.fm top tracks"));
    saved_playlists.push_back(dgettext("mms-audio", "last.fm loved tracks"));
  }

  saved_playlist_pos = 0;

  returned = false;
  return_value = false;

  bool update_needed = true;

  in_choose_playlist = true;

  while (!exit_choose_playlist_loop)
    {
      if (update_needed)
	print_choose_playlist(saved_playlists, vector_lookup(saved_playlists, saved_playlist_pos), saved_playlist_pos);

      input = input_master->get_input_busy_wrapped(busy_indicator);

      if (sd->is_enabled()) {
      	sd->cancel();
      	continue;
      }

      if (input.key == "touch_input") {
	S_Touch::get_instance()->run_callback();
	continue;
      }

      update_needed = true;

      if (input.command == "back" && input.mode == "general")
	{
	  return_value = false;
	  exit_choose_playlist();
	}
      else if (input.command == "prev")
 	{
 	  if (saved_playlist_pos != 0)
	    --saved_playlist_pos;
	  else
	    saved_playlist_pos = saved_playlists.size()-1;
 	}
      else if (input.command == "next")
 	{
 	  if (saved_playlist_pos != saved_playlists.size()-1)
	    ++saved_playlist_pos;
	  else
	    saved_playlist_pos = 0;
 	}
      else if (input.command == "page_up")
 	{
	  if (saved_playlists.size() > conf->p_jump()) {
	    int new_pos = saved_playlist_pos-conf->p_jump();
	    if (saved_playlist_pos == 0)
	      saved_playlist_pos = saved_playlists.size()-1+new_pos;
	    else if (new_pos < 0)
	      saved_playlist_pos = 0;
	    else
	      saved_playlist_pos = new_pos;
	  }
 	}
      else if (input.command == "page_down")
 	{
	  if (saved_playlists.size() > conf->p_jump()) {
	    if (saved_playlist_pos > (saved_playlists.size() - conf->p_jump()) && saved_playlist_pos != (saved_playlists.size()-1))
	      saved_playlist_pos = saved_playlists.size()-1;
	    else
	      saved_playlist_pos = (saved_playlist_pos+conf->p_jump())%saved_playlists.size();
	  }
 	}
      else if (input.command == "delete")
	{
	  delete_playlist();

	  if (returned)
	    break;
	}
      else if (input.command == "action")
	{
	  string currently_selected_row = vector_lookup(saved_playlists, saved_playlist_pos);
	  
	  if (currently_selected_row == dgettext("mms-audio", "last.fm loved tracks")) {
	    if (load_playlist_from_db(LastFM::lookup_loved_tracks(audio_conf->p_last_fm_username()))) {
	      returned = true;
	      return_value = true;
	    }
	  } else if (currently_selected_row == dgettext("mms-audio", "last.fm top tracks")) {
	    if (load_playlist_from_db(LastFM::lookup_top_tracks(audio_conf->p_last_fm_username()))) {
	      returned = true;
	      return_value = true;
	    }
	  } else
	    action_load_playlist();

	  if (returned)
	    break;
	}
      else if (input.command == "playlist_mode")
	{
	  return_value = false;
	  exit_choose_playlist();
	}
      else
	update_needed = !global->check_commands(input);
    }

  exit_choose_playlist_loop = false;
  in_choose_playlist = false;

  input_master->restore_map();
  return return_value;
}

void Audio::action_load_playlist()
{
  if (load_playlist(vector_lookup(saved_playlists, saved_playlist_pos))) {
    audio_state->p->stop();
    returned = true;
    return_value = true;
  } else
    DialogWaitPrint pdialog(dgettext("mms-audio", "Playlist is corrupt"), 1000);
}

void Audio::delete_playlist()
{
  // delete the file from drive
  std::ostringstream del;
  del << "rm \"" + conf->p_var_data_dir() + "playlists/" << vector_lookup(saved_playlists, saved_playlist_pos) << "\"" << std::endl;
  run::external_program(del.str());

  vector<string>::iterator i = saved_playlists.begin() + saved_playlist_pos;
  saved_playlists.erase(i);

  // this is saved_playlists.size(), not -1, because we just deleted a track
  if (saved_playlist_pos == saved_playlists.size())
    --saved_playlist_pos;

  // if we removed the last element then bail
  if (saved_playlists.size() == 0) {
    returned = true;
    return_value = false;
  }
}

void Audio::choose_radio()
{
  BusyIndicator *busy_indicator = S_BusyIndicator::get_instance();

  // init it if it's not already done
  if (!audio_conf->p_audio_started())
    audio_state = S_Audio_s::get_instance();

  Input input;

  input_master->set_map("audio");

  cur_radio_files.clear();
  print_radio_files.clear();

  Genre my_radio_genre(dgettext("mms-audio", "My Radio Stations"), "");
  print_radio_files.push_back(my_radio_genre.first);
  cur_radio_files.push_back(my_radio_genre);

  Shoutcast *shoutcast = Shoutcast::get_instance();

  if (radio_conf->p_download_radio()) {
    if (!radio_stations_downloaded) {
      DialogWaitPrint pdialog(dgettext("mms-audio", "Downloading radio stations"), 2000);
    }

    list<Genre> shoutcast_genrelist = shoutcast->get_genrelist();

    if (shoutcast_genrelist.size() == 0)
      DialogWaitPrint pdialog(dgettext("mms-audio", "Failed to download radio stations"), 2000);
    else {
      foreach (Genre& genre_pair, shoutcast_genrelist) {
	print_radio_files.push_back(genre_pair.first);
	cur_radio_files.push_back(genre_pair);
      }
      radio_stations_downloaded = true;
    }
  }

  if (cur_radio_files.size() == 0) {
    Print pdialog(Print::SCREEN);
    pdialog.add_line(dgettext("mms-audio", "Could not find any internet radio stations"));
    pdialog.print();
  }

  radio_pos = 0;
  old_radio_pos = 0;

  radio_update_needed = true;
  exit_radio = false;
  in_genre = true;

  int curlayer = render->device->get_current_layer();
  render->device->switch_to_layer(curlayer + 1);
  render->device->animation_section_begin(true);
  render->device->reset_layout_attribs_nowait();
  render->device->set_layout_alpha(0.0, curlayer + 1);
  render->device->animation_zoom(0,0,1,1,20,curlayer +1);
  render->device->animation_spin(-360,0,40,curlayer +1);
  render->device->animation_fade(0,1,40,curlayer +1);
  render->device->animation_fade(1,0,40,curlayer);
  render->device->animation_section_end();

  while (!exit_radio)
    {
      if (radio_update_needed)
	print_radio_list(print_radio_files, vector_lookup(cur_radio_files, radio_pos).first, radio_pos);

      input = input_master->get_input_busy_wrapped(busy_indicator);

      if (input.key == "touch_input") {
	S_Touch::get_instance()->run_callback();
	continue;
      }

      radio_update_needed = true;

      if (input.command == "prev")
 	{
 	  if (radio_pos != 0)
	    --radio_pos;
	  else
	    radio_pos = cur_radio_files.size()-1;
 	}
      else if (input.command == "next")
 	{
 	  if (radio_pos != cur_radio_files.size()-1)
	    ++radio_pos;
	  else
	    radio_pos = 0;
 	}
      else if (input.command == "page_up")
 	{
	  if (cur_radio_files.size() > conf->p_jump()) {
	    int new_radio_pos = radio_pos-conf->p_jump();
	    if (radio_pos == 0)
	      radio_pos = cur_radio_files.size()-1+new_radio_pos;
	    else if (new_radio_pos < 0)
	      radio_pos = 0;
	    else
	      radio_pos = new_radio_pos;
	  }
 	}
      else if (input.command == "page_down")
 	{
	  if (cur_radio_files.size() > conf->p_jump()) {
	    if (radio_pos > (cur_radio_files.size() - conf->p_jump()) && radio_pos != (cur_radio_files.size()-1))
	      radio_pos = cur_radio_files.size()-1;
	    else
	      radio_pos = (radio_pos+conf->p_jump())%cur_radio_files.size();
	  }
	}
      else if (input.command == "second_action")
	{
	  ExtraMenu em;

	  foreach (ExtraMenuItem& item, global->menu_items)
	    em.add_persistent_item(item);

	  string action_text;
	  if (in_genre)
	    action_text = dgettext("mms-audio", "Enter genre");
	  else
	    action_text = dgettext("mms-audio", "Play station now");

	  em.add_item(ExtraMenuItem(action_text,
				    input_master->find_shortcut("action"),
				    boost::bind(&Audio::radio_action, this)));

	  if (!in_genre) {
	    em.add_item(ExtraMenuItem(dgettext("mms-audio", "Add station to playlist"),
				      input_master->find_shortcut("radio_playlist_add"),
				      boost::bind(&Audio::radio_playlist_add, this)));

	    if (old_radio_pos == 0)
	      em.add_item(ExtraMenuItem(dgettext("mms-audio", "Remove station from my radio stations"),
					input_master->find_shortcut("radio_my_stations_remove"),
					boost::bind(&Audio::radio_my_stations_remove, this)));
	    else
	      em.add_item(ExtraMenuItem(dgettext("mms-audio", "Add station to my radio stations"),
					input_master->find_shortcut("radio_my_stations_add"),
					boost::bind(&Audio::radio_my_stations_add, this)));
	  }

	  em.add_item(ExtraMenuItem(dgettext("mms-audio", "Options"),
				    input_master->find_shortcut("options"),
				    boost::bind(&Audio::options, this)));
	  string back_text;
	  if (in_genre)
	    back_text = dgettext("mms-audio", "Return to startmenu");
	  else
	    back_text = dgettext("mms-audio", "Go up one directory");

	  em.add_item(ExtraMenuItem(back_text, input_master->find_shortcut("back"),
				    boost::bind(&Audio::back_radio, this)));

	  em.mainloop();
	}
      else if (input.command == "options")
	{
	  options();
	}
      // play specific
      else if (input.command == "action")
	{
	  radio_action();
        }
      else if (input.command == "radio_playlist_add")
	{
	  radio_playlist_add();
	}
      else if (input.command == "radio_my_stations_add")
	{
	  if (old_radio_pos != 0)
	    radio_my_stations_add();
	}
      else if (input.command == "radio_my_stations_remove")
	{
	  if (old_radio_pos == 0)
	    radio_my_stations_remove();
	}
      else if(input.command == "back")
	{
	  back_radio();
	}
      else
	radio_update_needed = !global->check_commands(input);
    }

    render->device->layer_active(false);
    render->device->switch_to_layer(curlayer);
    render->device->animation_section_begin();
    render->device->reset_layout_attribs_nowait();
    render->device->animation_fade(0,1,80,curlayer);
    render->device->animation_section_end();

    input_master->restore_map();
}

void Audio::radio_my_stations_add()
{
  Shoutcast *shoutcast = Shoutcast::get_instance();
  RadioStation station = vector_lookup(cur_radio_files, radio_pos);

  radio_stations.push_back(station);

  save_radio_stations();
}

void Audio::radio_my_stations_remove()
{
  vector<RadioStation>::iterator i = radio_stations.begin() + radio_pos;
  radio_stations.erase(i);

  vector<RadioStation>::iterator i2 = cur_radio_files.begin() + radio_pos;
  cur_radio_files.erase(i2);

  vector<string>::iterator i3 = print_radio_files.begin() + radio_pos;
  print_radio_files.erase(i3);

  save_radio_stations();

  if (print_radio_files.size() == 0)
    back_radio();
  else if (radio_pos == print_radio_files.size())
    radio_pos = print_radio_files.size()-1;
}

void Audio::load_radio_stations()
{
  string path = conf->p_var_data_dir() + "/RadioStations";
  
  std::ifstream in;

  in.open(path.c_str(), std::ios::in);

  if (!in.is_open()) {
    print_warning(dgettext("mms-audio", ("Could not open radio stations file " + path).c_str()), "AUDIO");
  } else {
    string str;
    for (int line=1; getline(in, str); line++) {
      string::size_type p = str.rfind(",");
      if (p != string::npos) {
	radio_stations.push_back(std::make_pair(str.substr(0, p), str.substr(p+1)));
      }
    }
  }
}

void Audio::save_radio_stations()
{
  string path = conf->p_var_data_dir() + "/RadioStations";
  
  std::ofstream file;
  file.open(path.c_str());

  if (!file) {
    print_critical(dgettext("mms-audio", "Could not write radio stations to file ") + path, "AUDIO");
  } else {
    foreach (RadioStation& station, radio_stations) {
      file << station.first << "," << station.second << std::endl;
    }
  }

  file.close();
}

void Audio::back_radio()
{
  if (in_genre)
    exit_radio = true;
  else {

    Shoutcast *shoutcast = Shoutcast::get_instance();

    print_radio_files.clear();
    cur_radio_files.clear();

    std::pair<string, string> my_radio_genre(dgettext("mms-audio", "My Radio Stations"), "");

    print_radio_files.push_back(my_radio_genre.first);
    cur_radio_files.push_back(my_radio_genre);

    if (radio_conf->p_download_radio()) {
      list<Genre> shoutcast_genrelist = shoutcast->get_genrelist();

      foreach (Genre& genre_pair, shoutcast_genrelist) {
	print_radio_files.push_back(genre_pair.first);
	cur_radio_files.push_back(genre_pair);
      }
    }

    radio_pos = old_radio_pos;
    radio_update_needed = true;
    in_genre = true;

  }
}

void Audio::print_radio_string_element(const string& r, const string& position, int y)
{
  string name = r;
  string_format::format_to_size(name, list_font, conf->p_h_res()-160, true);

  int width = conf->p_h_res()-(2*67);

  PObj *p = new PFObj(themes->general_marked_large, 70, y,
		      width, list_font_height, 2, true);

  if (r == position)
    render->current.add(p);

  S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), 2,
						   boost::bind(&Audio::find_radio_element_and_run_callback, this, r)));

  if (conf->p_convert())
    name = string_format::convert(name);

  render->current.add(new TObj(name, list_font, 75, y-2, themes->audio_font1, themes->audio_font2, themes->audio_font3, 3));
}

void Audio::print_choose_playlist_string_element(const string& r, const string& position, int y)
{
  string name = r;
  string_format::format_to_size(name, list_font, conf->p_h_res()-160, true);

  int width = conf->p_h_res()-(2*67);

  if (r == position){
    PObj *p = new PFObj(themes->general_marked_large, 70, y,
	width, list_font_height, 2, true);
    render->current.add(p);
  }

  S_Touch::get_instance()->register_area(TouchArea(rect(67, y, width, list_font_height), 2, boost::bind(&Audio::find_choose_playlist_element_and_run_callback, this, r)));

  if (conf->p_convert())
    name = string_format::convert(name);

  pair<int, int> name_size = string_format::calculate_string_size(name, list_font);

  render->current.add(new TObj(name, list_font, 75, y + (list_font_height - name_size.second) / 2,
			       themes->audio_font1, themes->audio_font2, themes->audio_font3, 3));
}

void Audio::print_top()
{
  render->prepare_new_image();

#if 0
  render->current.add(new PObj(themes->audio_background, 0, 0, 0, SCALE_FULL));
#else
  render->current.add(new BgRObj(0));
#endif

  render->current.add(new PFObj(themes->startmenu_music_dir, 25, 10, header_box_size, header_box_size, 2, true));

  if (themes->show_header) {

    string header = dgettext("mms-audio", "Audio Library");

    if (mode == PLAY) {

      header = dgettext("mms-audio", "Audio - Playlist");

    } else if (folders.size() > 1) {

      string top_folder = folders.top().first.front();

      if (top_folder[top_folder.size()-1] == '/')
	top_folder = top_folder.substr(0, top_folder.size()-1);

      assert(top_folder.rfind('/') != string::npos);
      header += " - " + top_folder.substr(top_folder.rfind('/')+1);
      string_format::format_to_size(header, header_font, conf->p_h_res()-220, false);
    }

    render->current.add(new TObj(header, header_font, 25 + header_box_size + 10, 10 + (header_box_size - header_size.second)/2,
				 themes->audio_header_font1, themes->audio_header_font2,
				 themes->audio_header_font3, 2));
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
}

void Audio::print_buttom(int cur_files_size, int position)
{
  std::ostringstream pos_in_list;

  if (search_mode)
    if (cur_files_size > 0)
      pos_in_list << position + 1 << "/" << cur_files_size;
    else
      pos_in_list << "";
  else
    pos_in_list << position+1 << "/" << cur_files_size;

  int x = string_format::calculate_string_width(pos_in_list.str(), position_font);

  render->current.add(new TObj(pos_in_list.str(), position_font,
			       conf->p_h_res()-(60+x), 20,
			       themes->audio_font1, themes->audio_font2,
			       themes->audio_font3, 3));

  render->draw_and_release("Audio");
}

void Audio::exit_choose_playlist()
{
  exit_choose_playlist_loop = true;
}

void Audio::find_choose_playlist_element_and_run_callback(const string& element)
{
  int p = 0;
  foreach (string& el, saved_playlists) {
    if (el == element) {
      saved_playlist_pos = p;
      break;
    } else
      ++p;
  }

  action_load_playlist();

  if (returned)
    exit_choose_playlist();
}

void Audio::find_radio_element_and_run_callback(const string& element)
{
  int p = 0;

  foreach (RadioStation& el, cur_radio_files) {
    if (el.first == element) {
      radio_pos = p;
      break;
    } else
      ++p;
  }

  radio_action();
}

void Audio::print_choose_playlist(const vector<string>& cur_files, const string& position, const int int_position)
{
  render->prepare_new_image();

#if 0
  render->current.add(new PObj(themes->audio_background, 0, 0, 0, SCALE_FULL));
#else
  render->current.add(new BgRObj(0));
#endif

  PObj *p = new PFObj(themes->audio_playlist_icon, 25, 10, header_box_size, header_box_size, 2, true);
  render->current.add(p);

  S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&Audio::exit_choose_playlist, this)));

  string header = dgettext("mms-audio", "Audio - Choose Playlist");

  TObj *t = new TObj(header, header_font, 25 + header_box_size + 10, 10 + (header_box_size-header_size.second)/2,
		     themes->audio_header_font1, themes->audio_header_font2,
		     themes->audio_header_font3, 2);
  S_Touch::get_instance()->register_area(TouchArea(rect(t->x, t->y, t->w, t->h), t->layer, boost::bind(&Audio::toggle_playlist, this)));
  render->current.add(t);

  print_range<string>(cur_files, position, int_position, boost::bind(&Audio::print_choose_playlist_string_element, this, _1, _2, _3), list_font_height);

  render->draw_and_release("Audio choose playlist");
}

void Audio::print_radio_list(const vector<string>& cur_files, const string& position, const int int_position)
{
  render->prepare_new_image();

#if 0
  render->current.add(new PObj(themes->audio_background, 0, 0, 0, SCALE_FULL));
#else
  render->current.add(new BgRObj(0));
#endif

  PObj *p = new PFObj(themes->startmenu_radio, 25, 10, header_box_size, header_box_size, 2, true);
  render->current.add(p);

  S_Touch::get_instance()->register_area(TouchArea(rect(p->x, p->y, p->w, p->h), p->layer, boost::bind(&Audio::back_radio, this)));

  string header = dgettext("mms-audio", "Audio - Radio");

  TObj *t = new TObj(header, header_font, 25 + header_box_size + 10, 10 + (header_box_size - header_size.second)/2,
		     themes->audio_header_font1, themes->audio_header_font2,
		     themes->audio_header_font3, 2);
  S_Touch::get_instance()->register_area(TouchArea(rect(t->x, t->y, t->w, t->h), t->layer, boost::bind(&Audio::toggle_playlist, this)));
  render->current.add(t);

  print_range<string>(cur_files, position, int_position, boost::bind(&Audio::print_radio_string_element, this, _1, _2, _3), list_font_height);

  render->draw_and_release("Audio radio");
}

void Audio::detect_cdformat()
{
  navigating_media = true;
  top_media_folders.clear();
  top_media_folders.push_back(cd->get_mount_point());

  int cdrom = cd->cdrom;

  switch(ioctl(cdrom, CDROM_DISC_STATUS)) {

  case CDS_NO_INFO:
  case CDS_NO_DISC:
  case CDS_TRAY_OPEN:
  case CDS_DRIVE_NOT_READY:
  case CDS_DISC_OK:
    {
      Print pdialog(dgettext("mms-audio", "The disc does not contain any recognizable files"),
		    Print::SCREEN);
    }
    break;
  case  CDS_MIXED:
  case CDS_AUDIO:
    cd_menu();
    break;
  case CDS_DATA_1:
  case CDS_DATA_2:
  case CDS_XA_2_1:
  case CDS_XA_2_2:
    add_data_cd();
    break;
  default:
    {
      Print pdialog(dgettext("mms-audio", "The disc does not contain any recognizable files"),
		    Print::SCREEN);
    }
    break;
  }

  navigating_media = false;
}

vector<string> Audio::list_playlists()
{
  vector<string> cur_files;

  string argv = conf->p_var_data_dir() + "playlists/";

  libfs_set_folders_first(opts.dir_order());

  for (file_iterator<file_t, default_order> i (argv); (i != i.end ()); i.advance(false))
    {
      string filename = i->getName();
      cur_files.push_back(filename.substr(argv.size()+1));
    }

  return cur_files;
}

#ifdef use_notify_area
void Audio::nArea_activate()
{
  if (audio_state->p_playing() && audio_state->p->is_playing()) {
    nArea->activate("audio_playtrack");
    nArea->activate("audio_options");
  } else {
    nArea->deactivate("audio_options");
    nArea->deactivate("audio_playtrack");
  }
}
#endif

void Audio::print_audiotrack()
{
  if (audio_conf->p_audio_started()) {

    if (!render->show_audio_track && audio_state->has_played) {
      render->wait_and_aquire();
      audio_state->has_played = false;
      audio_state->playback.cleanup();
      render->image_mut.leaveMutex();

      return;
    } else if (!render->show_audio_track)
      return;

    if (audio_state->p_playing() && audio_state->p->is_playing()
	&& !audio_state->fullscreen_info && !audio_state->p->is_buffering()) {

      render->wait_and_aquire();

      audio_state->has_played = true;

      if (audio_state->playback.elements.size() > 0)
	audio_state->playback.partial_cleanup();

      int x_pos = 7;
      string buffer, artist, album, title;

      print_audiotrack_helper(x_pos);
      get_audiotrack_info(buffer, artist, album, title);

      int text_height = (2 * button_playback_height + 5);

      int y_position = conf->p_v_res() - (2 * button_playback_height + 5);

      if (audio_state->p_pause()) {
	PObj *p = new PObj(themes->audio_playback_pause, x_pos, y_position, 0, NOSCALING);
	PObj *p2 = new PObj(themes->audio_playback_pause, x_pos + (53 - p->real_w)/2,
			    y_position + (text_height - p->real_h)/2, 0, NOSCALING);
	delete p;
	audio_state->playback.add(p2);
      } else {
	PObj *p = new PObj(themes->audio_playback_play, x_pos, y_position, 0, NOSCALING);
	PObj *p2 = new PObj(themes->audio_playback_play, x_pos + (53 - p->real_w)/2,
			    y_position + (text_height - p->real_h)/2, 0, NOSCALING);
	delete p;
	audio_state->playback.add(p2);
      }

#ifdef use_notify_area
      int max_size = conf->p_h_res()-(53+x_pos)-nArea->getRealWidth();
#else
      int max_size = conf->p_h_res()-(53+x_pos)-opts_offset+50;
#endif

      // print artist info
      if ((!artist.empty() && !title.empty()) ||
	  (audio_state->p->p_cur_nr().type == "web" && !title.empty())) {

	if (artist == "SCRIPT") {
	  artist = "";
	  album = "";
	}

	if (artist.empty() && audio_state->p->p_cur_nr().type == "web")
	  artist = audio_state->p->p_cur_nr().name; // radio station name

	int y_delta = 0;

	if (artist.empty() && album.empty())
	  y_delta = - 15;
	else {

	  int artist_x_size = string_format::calculate_string_width(artist, button_playback_font);
	  int album_x_size = string_format::calculate_string_width(album, button_playback_font);

	  int max_artist_size = max_size/2;
	  int max_album_size = max_size/2;

	  if (artist_x_size < max_size/2)
	    max_album_size = max_size-artist_x_size;
	  else if (album_x_size < max_size/2)
	    max_artist_size = max_size-album_x_size;

	  string_format::format_to_size(artist, button_playback_font, max_artist_size, true);
	  string_format::format_to_size(album, button_playback_font, max_album_size, true);

	  string artist_album = artist;
	  if (artist.empty() || album.empty())
	    artist_album += album;
	  else
	    artist_album += (" - " + album);

	  audio_state->playback.add(new TObj(artist_album,
					     button_playback_font, 53+x_pos, y_position,
					     themes->audio_playback_font1, themes->audio_playback_font2,
					     themes->audio_playback_font3, 0));
	}

	int x_size = string_format::calculate_string_width(string("     ") + buffer, button_playback_font);

	string_format::format_to_size(title, button_playback_font, max_size-x_size, true);

	audio_state->playback.add(new TObj(title + "     " + buffer,
					   button_playback_font, 53+x_pos, y_position + button_playback_height + y_delta,
					   themes->audio_playback_font1, themes->audio_playback_font2,
					   themes->audio_playback_font3, 0));


      } else {

	Simplefile cur_pos = audio_state->p->p_cur_nr();

	string name = cur_pos.name  + "     " + buffer;

	int y_position = conf->p_v_res() - button_playback_height - 2;

	string_format::format_to_size(name, button_playback_font, max_size, true);
	audio_state->playback.add(new TObj(name, button_playback_font, 53+x_pos, y_position,
					   themes->audio_playback_font1, themes->audio_playback_font2,
					   themes->audio_playback_font3, 0));

      }

#ifndef use_notify_area
      opts_offset = 46;

      if (S_ScreenUpdater::get_instance()->timer.status("pictures"))
	opts_offset = 146;

      // check if repeat is on then display
      if (opts.repeat()== dgettext("mms-audio", "yes") && opts.shutdown()== gettext("no")) {
	audio_state->playback.add(new PObj(themes->audio_repeat, conf->p_h_res()-opts_offset ,conf->p_v_res()-52, 3, NOSCALING));
	opts_offset += 50;
      }

      // check if shutdown is on then display
      if (opts.shutdown()== dgettext("mms-audio", "yes")) {
	audio_state->playback.add(new PObj(themes->audio_shutdown, conf->p_h_res()-opts_offset ,conf->p_v_res()-52, 3, NOSCALING));
	opts_offset += 50;
      }

      // display shuffle mode
      if (opts.shuffle()== dgettext("mms-audio", "completely random")) {
	audio_state->playback.add(new PObj(themes->audio_random, conf->p_h_res()-opts_offset, conf->p_v_res()-52, 3, NOSCALING));
	opts_offset += 50;
      }
      else if (opts.shuffle()== dgettext("mms-audio", "intelligent")) {
	audio_state->playback.add(new PObj(themes->audio_randomi, conf->p_h_res()-opts_offset, conf->p_v_res()-52, 3, NOSCALING));
	opts_offset += 50;
      }
      else if (opts.shuffle()== dgettext("mms-audio", "simple random")) {
	audio_state->playback.add(new PObj(themes->audio_randomw,  conf->p_h_res()-opts_offset, conf->p_v_res()-52, 3, NOSCALING));
	opts_offset += 50;
      }
#endif

      render->draw_and_release("Audio print track", true);

    } else if (audio_state->has_played) {

      render->wait_and_aquire();

      audio_state->has_played = false;
      audio_state->playback.cleanup();

      render->image_mut.leaveMutex();
    }
  }
}

void Audio::print_lcd_audiotrack()
{
  if (audio_conf->p_audio_started()) {

    if (audio_state->p_playing() && audio_state->p->is_playing() &&
	time(0)-conf->p_last_key() > 5) {

      string buffer = lcd_format_time(audio_state->p->p_cur_time(), audio_state->p->p_total_time());

      if (!audio_state->p->p_artist().empty() && !audio_state->p->p_title().empty()) {
	if (global->lcd_rows() > 3) global->lcd_add_output(string_format::pretty_print_string_copy(audio_state->p->p_artist(),global->lcd_pixels()));
	if (global->lcd_rows() > 2) global->lcd_add_output(string_format::pretty_print_string_copy(audio_state->p->p_album(),global->lcd_pixels()));
	if (global->lcd_rows() > 1) global->lcd_add_output(audio_state->p->p_title());
      } else {
	Simplefile cur_pos = audio_state->p->p_cur_nr();
	if (global->lcd_rows() > 1) global->lcd_add_output(cur_pos.name);
	if (global->lcd_rows() > 2) global->lcd_add_output("");
	if (global->lcd_rows() > 3) global->lcd_add_output("");
      }
      global->lcd_add_output(string(buffer));
      global->lcd_print();
    }
  }
}

int Audio::print_audio_time()
{
  return 300; // ms
}

int Audio::check_audio_time()
{
  return 100; // ms
}

void Audio::print_audio_screensaver()
{
  if (audio_state->p_playing() && audio_state->p->is_playing()) {

#ifdef use_notify_area
     if (!audio_conf->p_na_on_scr() && is_screensaver_lyrics())
       nArea->disable(); // disable if have a lyrics
#endif

    render->wait_and_aquire();

    if (audio_state->playback_fullscreen.elements.size() > 0)
      audio_state->playback_fullscreen.partial_cleanup();

    audio_state->playback_fullscreen.add(new RObj(0, 0, conf->p_h_res(), conf->p_v_res(), 0, 0, 0, 255, 0));

    string buffer, artist, album, title;

    get_audiotrack_info(buffer, artist, album, title);

    if (is_screensaver_lyrics())
      print_audio_screensaver_lyrics(artist, album, title, buffer);
    else
      print_audio_screensaver_standard(artist, album, title, buffer);
  }
}

void Audio::print_audio_screensaver_lyrics(string artist, string album,
					   string title, string buffer)
{
  int max_y_pic = conf->p_v_res() / 4;
  int max_x_pic = max_y_pic;

  if (conf->p_v_res() == 405) {
    max_x_pic = 135;
    max_y_pic = 150;
  }

  int x_pos = max_x_pic+60+30;
  int y = 0;

  if ((!artist.empty() && !title.empty())
      || (audio_state->p->p_cur_nr().type == "web" && !title.empty())) {

    y = 35;

    if (print_audio_screensaver_helper(x_pos, y, max_x_pic, max_y_pic, false))
      x_pos = max_x_pic + 60 + 30;

    if (artist == "SCRIPT") {
      artist = "";
      album = "";
    }

    if (artist.empty() && audio_state->p->p_cur_nr().type == "web")
      artist = audio_state->p->p_cur_nr().name; // radio station name

    string_format::format_to_size(artist, screensaver_lyrics_artist_font, conf->p_h_res()-x_pos-30, true);
    audio_state->playback_fullscreen.add(new TObj(artist, screensaver_lyrics_artist_font, x_pos, y-18,
						  themes->audio_playback_font1, themes->audio_playback_font2,
						  themes->audio_playback_font3, 3));

    y += screensaver_lyrics_artist_font_height;

    string_format::format_to_size(album, screensaver_album_font, conf->p_h_res()-x_pos-30, true);
    audio_state->playback_fullscreen.add(new TObj(album, screensaver_lyrics_album_font, x_pos, y-24,
						  themes->audio_playback_font1, themes->audio_playback_font2,
						  themes->audio_playback_font3, 3));

    y += screensaver_lyrics_album_font_height;

    string_format::format_to_size(title, screensaver_lyrics_normal_font, conf->p_h_res()-x_pos-30, true);
    audio_state->playback_fullscreen.add(new TObj(title, screensaver_lyrics_normal_font, x_pos, y-24,
						  themes->audio_playback_font1, themes->audio_playback_font2,
						  themes->audio_playback_font3, 3));

    y += screensaver_lyrics_normal_font_height;

    audio_state->playback_fullscreen.add(new TObj(buffer, screensaver_lyrics_normal_font, x_pos, y-24,
						  themes->audio_playback_font1, themes->audio_playback_font2,
						  themes->audio_playback_font3, 3));

    if (audio_state->p_pause()) {
      int width = string_format::calculate_string_width(buffer, screensaver_normal_font);
      audio_state->playback_fullscreen.add(new PObj(themes->audio_playback_pause, x_pos + width, y-24, 3, NOSCALING));
    }

    y += screensaver_lyrics_normal_font_height;

  } else {

    y = (conf->p_v_res()-165)/2;

    if (print_audio_screensaver_helper(x_pos, y, max_x_pic, max_y_pic, false))
      x_pos = max_x_pic + 60 + 30;

    y = 50;

    Simplefile cur_pos = audio_state->p->p_cur_nr();

    string name = cur_pos.name;

    string_format::format_to_size(name, screensaver_normal_font, conf->p_h_res()-x_pos-30, true);
    audio_state->playback_fullscreen.add(new TObj(name, screensaver_lyrics_normal_font, x_pos, y,
						  themes->audio_playback_font1, themes->audio_playback_font2,
						  themes->audio_playback_font3, 3));

    y += screensaver_lyrics_normal_font_height;

    audio_state->playback_fullscreen.add(new TObj(buffer, screensaver_lyrics_normal_font, x_pos, y,
						  themes->audio_playback_font1, themes->audio_playback_font2,
						  themes->audio_playback_font3, 3));

    if (audio_state->p_pause()) {
      int width = string_format::calculate_string_width(buffer, screensaver_normal_font);
      audio_state->playback_fullscreen.add(new PObj(themes->audio_playback_pause, x_pos + width, y, 0, NOSCALING));
    }

     y += screensaver_lyrics_normal_font_height;
  }

  if (audio_state->p->p_cur_nr().type != "web") {
    std::ostringstream played_info;
    played_info << dgettext("mms-audio", "Playing track: ");

    if (opts.shuffle() == dgettext("mms-audio", "simple random"))
      played_info << shuffle_list.shuffle_pos + 1;
    else if (opts.shuffle() != dgettext("mms-audio", "off"))
      played_info << (audio_state->played_tracks.size() % playlist_size()) + 1;
    else
      played_info << get_real_playlist_pos() + 1;

    played_info << "/" << playlist_size();

    audio_state->playback_fullscreen.add(new TObj(played_info.str(), screensaver_lyrics_normal_font, x_pos, y-24,
						  themes->audio_playback_font1, themes->audio_playback_font2,
						  themes->audio_playback_font3, 3));

  }
  
  y += screensaver_lyrics_normal_font_height*1/3;
  audio_state->playback_fullscreen.add(new PObj("shadow-up.png", 0 , y, conf->p_h_res(), 60, true, 2));
  audio_state->playback_fullscreen.add(new PObj("shadow-down.png", 0 , conf->p_v_res() - 60, conf->p_h_res(), 60, true, 2));
  if (audio_state->p->p_cur_nr().type != "web")
    y += screensaver_lyrics_normal_font_height;
  y += 5;

  string current_lyric;

  Lyrics *lyrics = S_Lyrics::get_instance();

  lyrics->set_total_time(audio_state->p->p_total_time());

  if (lyrics->is_enabled()) {
    lyrics->set_display_height(conf->p_v_res() - y - 60); // Fixed : - calculate with bottom border as for shadow

    std::vector<std::string>::iterator itVectorData;
    itVectorData = lyrics->vecLyric.begin();

    if (audio_state->p_playing()) {
      if (!audio_state->p_pause() ) {
        lyrics->play();
	lyrics->set_current_time(audio_state->p->p_cur_time());
      } else {
        lyrics->pause();
      }
    }

    int y_pos, y_curr = 0;
    if (lyrics->get_displace()) {

      current_lyric = (lyrics->get_displace() > 0 ? "+" : "") + conv::itos(lyrics->get_displace());

      string_format::format_to_size(current_lyric, screensaver_lyric_font, conf->p_h_res()-50, false);

      std::pair<int, int> size = string_format::calculate_string_size(current_lyric, screensaver_lyric_font);

      audio_state->playback_fullscreen.add(new TObj(current_lyric, screensaver_lyric_font,
#ifdef use_notify_area
		(conf->p_h_res() - size.first - 10) , y + (conf->p_v_res() - y - NotifyArea::height) /2 - size.second/2,
#else
		(conf->p_h_res() - size.first - 10) , y + (conf->p_v_res() - y) /2 - size.second/2,
#endif
		themes->audio_playback_font1,
		themes->audio_playback_font2,
		themes->audio_playback_font3, 1));
    }

    unsigned int t;
    for(t = lyrics->get_line();
	t < lyrics->vecLyric.size() && t < lyrics->get_line() + lyrics->display_lines();
	t++){

      y_pos = y + y_curr - lyrics->get_anim();

      current_lyric = lyrics->vecLyric[t]; 

      string_format::format_to_size(current_lyric, screensaver_lyric_font, conf->p_h_res()-50, false);

      std::pair<int, int> size = string_format::calculate_string_size(current_lyric, screensaver_lyric_font);

      audio_state->playback_fullscreen.add(new TObj(current_lyric, screensaver_lyric_font,
						    (conf->p_h_res() /2 - size.first/2), y_pos,
						    themes->audio_playback_font1,
						    themes->audio_playback_font2,
						    themes->audio_playback_font3, 1));
      y_curr += size.second;
    }

    // Use shadow-area for next line to display if there is one ...
    if (t <= lyrics->get_line() + lyrics->display_lines() && t < lyrics->vecLyric.size()) {
        y_pos = y + y_curr - lyrics->get_anim();

        current_lyric = lyrics->vecLyric[t]; 

        string_format::format_to_size(current_lyric, screensaver_lyric_font, conf->p_h_res()-50, false);

        std::pair<int, int> size = string_format::calculate_string_size(current_lyric, screensaver_lyric_font);

        audio_state->playback_fullscreen.add(new TObj(current_lyric, screensaver_lyric_font,
                              (conf->p_h_res() /2 - size.first/2), y_pos,
                              themes->audio_playback_font1,
                              themes->audio_playback_font2,
                              themes->audio_playback_font3, 1));
        y_curr += size.second;
    }
  }

  render->draw("screensaver", true);
  render->image_mut.leaveMutex();
}

void Audio::print_audio_screensaver_standard(string artist, string album,
					     string title, string buffer)
{
  int max_x_pic = static_cast<int>(conf->p_h_res()/3.0);
  int max_y_pic = static_cast<int>(conf->p_v_res()/2.5);

  int x_pos = max_x_pic+60+30;
  int y = 0;

  if ((!artist.empty() && !title.empty())
      || (audio_state->p->p_cur_nr().type == "web" && !title.empty())) {

    y = (conf->p_v_res()-max_y_pic)/2;

    if (print_audio_screensaver_helper(x_pos, y, max_x_pic, max_y_pic, true))
      x_pos = max_x_pic + 60 + 30;

    // FIXME: this is a bug in one of the audio players (alsaplayer, so move
    // it there
    if (artist == "SCRIPT") {
      artist = "";
      album = "";
    }

    y -= 10;

    if (artist.empty() && audio_state->p->p_cur_nr().type == "web")
      artist = audio_state->p->p_cur_nr().name; // radio station name

    string_format::format_to_size(artist, screensaver_artist_font, conf->p_h_res()-x_pos-30, true);
    audio_state->playback_fullscreen.add(new TObj(artist, screensaver_artist_font, x_pos, y,
						  themes->audio_playback_font1, themes->audio_playback_font2,
						  themes->audio_playback_font3, 1));

    y += static_cast<int>(screensaver_artist_font_height*0.90);

    string_format::format_to_size(album, screensaver_album_font, conf->p_h_res()-x_pos-30, true);
    audio_state->playback_fullscreen.add(new TObj(album, screensaver_album_font, x_pos, y,
						  themes->audio_playback_font1, themes->audio_playback_font2,
						  themes->audio_playback_font3, 1));

    y += static_cast<int>(screensaver_album_font_height*1.35);

    string_format::format_to_size(title, screensaver_normal_font, conf->p_h_res()-x_pos-30, true);
    audio_state->playback_fullscreen.add(new TObj(title, screensaver_normal_font, x_pos, y,
						  themes->audio_playback_font1, themes->audio_playback_font2,
						  themes->audio_playback_font3, 1));

    y += screensaver_normal_font_height;

    audio_state->playback_fullscreen.add(new TObj(buffer, screensaver_normal_font, x_pos, y,
						  themes->audio_playback_font1, themes->audio_playback_font2,
						  themes->audio_playback_font3, 1));

    y += screensaver_normal_font_height;

  } else {

    int tmp;

    if (print_audio_screensaver_helper(x_pos, tmp, max_x_pic, max_y_pic, true))
      x_pos = max_x_pic+60+30;

    y = (conf->p_v_res()-80)/2;

    Simplefile cur_pos = audio_state->p->p_cur_nr();

    string name = cur_pos.name;

    string_format::format_to_size(name, screensaver_normal_font, conf->p_h_res()-x_pos-30, true);
    audio_state->playback_fullscreen.add(new TObj(name, screensaver_normal_font, x_pos, y,
						  themes->audio_playback_font1, themes->audio_playback_font2,
						  themes->audio_playback_font3, 1));

    y += screensaver_normal_font_height;

    audio_state->playback_fullscreen.add(new TObj(buffer, screensaver_normal_font, x_pos, y,
						  themes->audio_playback_font1, themes->audio_playback_font2,
						  themes->audio_playback_font3, 1));
    y += screensaver_normal_font_height;
  }

  y += 15;

  if (audio_state->p->p_cur_nr().type != "web") {
    std::ostringstream played_info;
    played_info << dgettext("mms-audio", "Playing track: ");

    if (opts.shuffle() == dgettext("mms-audio", "simple random"))
      played_info << shuffle_list.shuffle_pos + 1;
    else if (opts.shuffle() != dgettext("mms-audio", "off"))
      played_info << (audio_state->played_tracks.size() % playlist_size()) + 1;
    else
      played_info << get_real_playlist_pos() + 1;

    played_info << "/" << playlist_size();
    
    audio_state->playback_fullscreen.add(new TObj(played_info.str(), screensaver_normal_font, x_pos, y,
						  themes->audio_playback_font1, themes->audio_playback_font2,
						  themes->audio_playback_font3, 1));

    y += screensaver_normal_font_height;
  }
  
  audio_screensaver_next_helper(x_pos, y);

  if (audio_state->p_pause()) {
    PObj *p = new PObj(themes->audio_playback_pause, x_pos, y, 0, NOSCALING);
    audio_state->playback_fullscreen.add(p);
    y += p->h;
  }

  render->draw_and_release("screensaver", true);
}

int Audio::check_audio_fullscreen()
{
  Lyrics *lyrics = S_Lyrics::get_instance();

  return (opts.lyrics() == dgettext("mms-audio", "yes") && lyrics->is_enabled() ? static_cast<int>(lyrics->get_period()) : 250); // ms
}

void Audio::show_lyrics()
{
  // hack
  start_ssaver();
}

void Audio::show_mute()
{
  render->wait_and_aquire();

  if (audio_state->mute.elements.size() > 0)
    audio_state->mute.partial_cleanup();

  if (audio_state->p->is_mute()) {
    if (audio_state->fullscreen_info)
      audio_state->mute.add(new PObj(themes->audio_mute, conf->p_h_res()-200, conf->p_v_res()-75,
				     1, NOSCALING));
    else
      if (audio_state->p->is_mute())
	audio_state->mute.add(new PObj(themes->audio_mute, conf->p_h_res()-opts_offset, conf->p_v_res()-52,
 				       3, NOSCALING));
  }

  render->draw_and_release("mute", true);
}

void Audio::show_volume()
{
  render->wait_and_aquire();

  if (audio_state->volume.elements.size() > 0)
    audio_state->volume.partial_cleanup();

  if (audio_state->fullscreen_info) {

    if (audio_state->p->getvol() != 0) {
      int vol_length = string_format::calculate_string_width("VOL: " + conv::itos(audio_state->p->getvol()), volume_font);
	
      audio_state->volume.add(new TObj("VOL: " + conv::itos(audio_state->p->getvol()), volume_font,
#ifdef use_notify_area
				       conf->p_h_res() - (vol_length + 10), 
				       conf->p_v_res() - 75 - NotifyArea::height,
#else
				       conf->p_h_res() - (vol_length + 10), conf->p_v_res() - 75,
#endif
				       themes->audio_playback_font1,
				       themes->audio_playback_font2, themes->audio_playback_font3, 1));
    }
  } else {

#ifdef use_notify_area
    int y_vol = conf->p_v_res() - 140 - NotifyArea::height;
#else
    int y_vol = conf->p_v_res() - 190;
#endif

    int vol = audio_state->p->getvol()/4*4;

    // display sound level
    if (audio_state->p->getvol() > 0)
      audio_state->volume.add(new PObj(themes->audio_volume_bar, conf->p_h_res()-40,
				       int(y_vol + (126  * (1-vol/100.0))),
				       vol/100.0, 4));

    // display skin for sound level
    audio_state->volume.add(new PObj(themes->audio_volume, conf->p_h_res()-40 ,y_vol,
				     3, NOSCALING));

  }

  audio_state->volume_shown = true;

  displayed_volume = time(0);

  render->draw_and_release("volume", true);
}

int Audio::check_volume()
{
  return 300; // ms
}

void Audio::clean_up_volume()
{
  if (audio_state->volume_shown && time(0)-displayed_volume > 2) {

    render->wait_and_aquire();

    audio_state->volume_shown = false;
    audio_state->volume.cleanup();

    render->image_mut.leaveMutex();
  }
}

void Audio::set_audio_player()
{
  bool audio_player_set = false;
  Plugins *plugins = S_Plugins::get_instance();
  foreach (AudioPlayerPlugin *plugin, plugins->audio_players)
    if (plugin->plugin_name() == audio_conf->p_audio_player()) {
      plugin->player->init();
      audio_state->set_audio_player(plugin->player);
      audio_player_set = true;
      break;
    }

  if (!audio_player_set) {
    print_warning(dgettext("mms-audio", "Could not find audio player plugin specified in AudioConfig"), "AUDIO");
    foreach (AudioPlayerPlugin *plugin, plugins->audio_players) {
      plugin->player->init();
      audio_state->set_audio_player(plugin->player);
      audio_player_set = true;
      break;
    }
  }

  if (!audio_player_set) {
    print_critical(dgettext("mms-audio", "Could not find any audio player plugin"), "AUDIO");
    clean_up(0);
  }
}

void Audio::startup_updater()
{
  set_audio_player();

  load_last_playlist();

  ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();

  screen_updater->timer.add(TimeElement("audio_next", boost::bind(&Audio::check_audio_time, this),
					boost::bind(&Audio::check_audio_state, this)));

  if (global->lcd_output_possible())
    screen_updater->timer.add(TimeElement("audio_lcd", boost::bind(&Audio::print_audio_time, this),
					  boost::bind(&Audio::print_lcd_audiotrack, this)));

  screen_updater->timer.add(TimeElement("audio_update", boost::bind(&Audio::print_audio_time, this),
					boost::bind(&Audio::print_audiotrack, this)));
#ifdef use_notify_area
  screen_updater->timer.add(TimeElement("narea_update", boost::bind(&Audio::print_audio_time, this),
					boost::bind(&Audio::nArea_activate, this)));
#endif

/* Let's fire the screensaver thread */
  init_ssaver_thread();
  list<string> triggers;
  triggers.push_back("vol+");
  triggers.push_back("vol-");

  screen_updater->trigger.add(TriggerElement("audio_volume", triggers, boost::bind(&Audio::show_volume, this), 0));

#ifndef use_notify_area
  triggers.clear();
  triggers.push_back("mute");

  screen_updater->trigger.add(TriggerElement("audio_mute", triggers, boost::bind(&Audio::show_mute, this), 0));
#endif

  screen_updater->timer.add(TimeElement("audio_volume", boost::bind(&Audio::check_volume, this),
					boost::bind(&Audio::clean_up_volume, this)));

#ifdef use_notify_area
  nArea->add(NotifyElement("audio_playtrack", boost::bind(&Audio::naAudioPlayTrack, this)),false);
  nArea->add(NotifyElement("audio_options", boost::bind(&Audio::naAudioOptions, this)),false);
#endif
}

void Audio::pause()
{
  audio_state->p->pause();
}

void Audio::stop()
{
  // check if we are already stopped and if so, lets set the position to 0 as a
  // feature
  if (!audio_state->p_playing())
    playlist_pos_int = 0;

  audio_state->p->stop();

  update_playlist_view();

  // if clear on finish = false
  if (playlist_size() == 0) {
    Simplefile empty;
    audio_state->p->set_cur_nr(empty);
  }
}

void Audio::set_buffering_timeout()
{
  buffering_time_end = time(0);

  Simplefile s = audio_state->p->p_cur_nr();

  if (s.path.find("http://") != -1 || s.path.find("rtp://") != -1)
    buffering_time_end += 15; // secs
  else
    buffering_time_end += 2; // secs
}

void Audio::next_track()
{
  set_buffering_timeout();
  audio_state->p->next();
  update_playlist_view();
}

void Audio::prev_track()
{
  set_buffering_timeout();
  audio_state->p->prev();
  update_playlist_view();
}

void Audio::ff()
{
  audio_state->p->ff();
}

void Audio::fb()
{
  audio_state->p->fb();
}

void Audio::vol_plus()
{
  audio_state->p->volup();
}

void Audio::vol_minus()
{
  audio_state->p->voldown();
}

void Audio::mute()
{
  audio_state->p->mute();
}

void Audio::toggle_playlist_helper()
{
  change_mode = false;
  toggle_playlist();
}

bool Audio::check_mode(const Input& input, bool disable_mode_change)
{
  return (!disable_mode_change && (input.command == "mode" && input.mode == "audio_general")
	  || change_mode);
}

bool Audio::check_playlist_mode(const Input& input, bool disable_mode_change)
{
  return (!disable_mode_change && input.command == "playlist_mode" && (input.mode == "audio_add" || input.mode == "playlist"));
}

bool Audio::is_screensaver_lyrics()
{
  return opts.lyrics() == dgettext("mms-audio", "yes") && S_Lyrics::get_instance()->is_enabled();
}


void Audio::add_playback_keys()
{
  Playback p = Playback("Audio");
  p.add_command(GlobalCommand(boost::bind(&Audio::play, this), "play", dgettext("mms-audio", "Play")));
  p.add_command(GlobalCommand(boost::bind(&Audio::pause, this), "pause", dgettext("mms-audio", "Pause")));
  p.add_command(GlobalCommand(boost::bind(&Audio::stop, this), "stop", dgettext("mms-audio", "Stop")));
  p.add_command(GlobalCommand(boost::bind(&Audio::ff, this), "ff", dgettext("mms-audio", "Fast forward")));
  p.add_command(GlobalCommand(boost::bind(&Audio::fb, this), "fb", dgettext("mms-audio", "Fast backward")));
  p.add_command(GlobalCommand(boost::bind(&Audio::next_track, this), "next_track", dgettext("mms-audio", "Next track")));
  p.add_command(GlobalCommand(boost::bind(&Audio::prev_track, this), "prev_track", dgettext("mms-audio", "Previous track")));
  p.add_command(GlobalCommand(boost::bind(&Audio::vol_plus, this), "vol+", dgettext("mms-audio", "Increase volume")));
  p.add_command(GlobalCommand(boost::bind(&Audio::vol_minus, this), "vol-", dgettext("mms-audio", "Decrease volume")));
  p.add_command(GlobalCommand(boost::bind(&Audio::mute, this), "mute", dgettext("mms-audio", "Mute")));
  p.add_command(GlobalCommand(boost::bind(&Audio::show_lyrics, this), "show_screensaver", dgettext("mms-audio", "Show Screensaver")));

  global->add_playback(p);
  global->set_playback("Audio");

  // lyrics
  if (conf->p_idle_time() > 0)
    input_master->add_input_hook(InputHook("Lyrics", "", boost::bind(&Audio::process_ssaver_input, this, _1), true));
  
  global->add_command(GlobalCommand(boost::bind(&Audio::toggle_playlist_helper, this),
				    boost::bind(&Audio::check_mode, this, _1, _2)));

  global->add_command(GlobalCommand(boost::bind(&Audio::toggle_load_playlist, this),
				    boost::bind(&Audio::check_playlist_mode, this, _1, _2)));

  global->add_second_menu_item(ExtraMenuItem(dgettext("mms-audio", "Toggle playlist"),
					     input_master->find_shortcut("mode"),
					     boost::bind(&Audio::toggle_playlist, this)));

  global->toggle_playlist = boost::bind(&Audio::toggle_playlist, this);
}

void Audio::process_ssaver_input(const Input& input){
  if(!is_ssaver_running()) /* nothing to do if the screensaver is off */
    return;
  if (input.command == "page_up" && is_screensaver_lyrics()){
    S_Lyrics::get_instance()->set_displace(-1);
    return;
  }

  else if (input.command == "page_down" && is_screensaver_lyrics()){
    S_Lyrics::get_instance()->set_displace(1);
    return;
  }
  else if ((input.mode != "playback" && input.mode != "audio_general") || (input.mode == "audio_general" && input.command == "mode")) {
    stop_ssaver();
  }
}

void Audio::check_shutdown()
{
  if (conv::stob(opts.shutdown())) {
    Shutdown *sd = S_Shutdown::get_instance();
    sd->enable();
  }
}

#ifdef use_notify_area
void Audio::naAudioPlayTrack()
{
  pair<int, int> size;
  int played_info, ow;
  string strMsg;

  if (opts.shuffle() != dgettext("mms-audio", "off"))
    played_info = audio_state->played_tracks.size();
  else
    played_info = get_real_playlist_pos()+1;

  render->image_mut.enterMutex();

  strMsg = dgettext("mms-audio", "of");
  size = string_format::calculate_string_size(strMsg, "Vera/18");
  ow = size.first;
  nArea->status_overlay.add(new TObj(strMsg, "Vera/18",
				     conf->p_h_res() - NotifyArea::width/4 - size.first/3,
				     conf->p_v_res() - NotifyArea::height/2 - size.second/2 - 10,
				     themes->notify_area_font1,
				     themes->notify_area_font2,
				     themes->notify_area_font3, 3));

  strMsg = conv::itos(played_info);
  size = string_format::calculate_string_size(strMsg, "Vera/32");
  nArea->status_overlay.add(new TObj(strMsg, "Vera/32",
				     conf->p_h_res() - NotifyArea::width/4 - size.first - ow/4,
				     conf->p_v_res() - NotifyArea::height/2 - size.second/2 - 10,
				     themes->notify_area_font1,
				     themes->notify_area_font2,
				     themes->notify_area_font3, 3));

  strMsg = conv::itos(playlist_size());
  size = string_format::calculate_string_size(strMsg, "Vera/22");
  nArea->status_overlay.add(new TObj(strMsg, "Vera/22",
				     conf->p_h_res() - size.first , conf->p_v_res() - size.second,
				     themes->notify_area_font1,
				     themes->notify_area_font2,
				     themes->notify_area_font3, 3));

  render->image_mut.leaveMutex();
}

void Audio::naAudioOptions()
{
  list<string> items;

  if (opts.shutdown()== dgettext("mms-audio", "yes")) {
    items.push_back(themes->audio_shutdown);
  } else if (opts.repeat()== dgettext("mms-audio", "yes")) {
    items.push_back(themes->audio_repeat);
  } // display shuffle mode
  if (opts.shuffle()== dgettext("mms-audio", "completely random")) {
    items.push_back(themes->audio_random);
  }
  else if (opts.shuffle()== dgettext("mms-audio", "intelligent")) {
    items.push_back(themes->audio_randomi);
  }
  else if (opts.shuffle()== dgettext("mms-audio", "simple random")) {
    items.push_back(themes->audio_randomw);
  }

  if (!items.size())
    items.push_back(themes->audio_normal);

  if (audio_state->p->is_mute())
    items.push_back(themes->audio_mute);

  int count = 1;

  render->image_mut.enterMutex();

  for (std::list<string>::iterator iter = items.begin(), end = items.end();
       iter != end; ++iter) {
    nArea->status_overlay.add(new PObj(*iter,
				       conf->p_h_res() - NotifyArea::width * count / (items.size() + 1),
				       conf->p_v_res() - 52, 3, NOSCALING));
    count++;
  }

  render->image_mut.leaveMutex();
}
#endif
