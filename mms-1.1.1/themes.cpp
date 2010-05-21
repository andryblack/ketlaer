#include "themes.hpp"

#include "common.hpp"
#include "config.hpp"
#include "theme.hpp"
#include "gettext.hpp"

#include <fstream>

using conv::atoi;
using std::string;

pthread_mutex_t Themes::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

Themes::Themes()
  : resolution_dependant_background_needed(false)
{}

string Themes::resolution_dependant_background(const string& bg)
{
  if (!resolution_dependant_background_needed)
    return bg;

  Config *config = S_Config::get_instance();
  bool verbosity = (config->p_debug_level() > 1);

  int i = bg.rfind('.');

  if (i != string::npos) {
    string name = bg.substr(0, i);
    string ext = bg.substr(i, bg.size()-i);

    if ((config->p_h_res()/16.0)/(config->p_v_res()/10) == 1)
      return name + "-16-10" + ext;
    else if ((config->p_h_res()/16.0)/(config->p_v_res()/9) == 1)
      return name + "-16-9" + ext;
    else if ((config->p_h_res()/4.0)/(config->p_v_res()/3) == 1)
      return name + "-4-3" + ext;
    else {
      if (verbosity)
	std::cerr << "WARNING: unsupported resolution, trying to find the best ratio" << std::endl;
      float ratio = (config->p_h_res()*1.0)/config->p_v_res();
      if (verbosity)
	std::cerr << "WARNING: ratio is : " << ratio << std::endl;
      if (ratio < 1.47) {
	if (verbosity)
	  std::cerr << "WARNING: using 4/3 ratio" << std::endl;
        return name + "-4-3" + ext;
      }
      else if ((1.47 <= ratio) and (ratio < 1.69)) {
	if (verbosity)
	  std::cerr << "WARNING: using 16/10 ratio" << std::endl;
        return name + "-16-10" + ext;
      }
      else {
	if (verbosity)
	  std::cerr << "WARNING: using 16/9 ratio" << std::endl;
        return name + "-16-9" + ext;
      }
    }
  }

  std::cerr << "ERROR: can't find . in " << bg << std::endl;
  return "";
}

bool Themes::parser(const string& cur_theme, bool startup)
{
  if (cur_theme == "")
    return false;

  Config *config = S_Config::get_instance();

  string theme_def;

  string theme_path = "themes/" + cur_theme + "/theme.conf";

  if (config->p_homedir() != "/etc/mms/" && file_exists(config->p_homedir() + theme_path))
    theme_def = config->p_homedir() + theme_path;
  else
    theme_def = mms_prefix"/share/mms/" + theme_path;

  std::ifstream in;

  in.open(theme_def.c_str(), std::ios::in);

  if (!in.is_open()) {
    if (startup) {
      print_warning(gettext("Could not open theme") + string(" ") + theme_def, "THEMES");
      return false;
    } else {
      Print pdialog(gettext("Could not open theme"), Print::SCREEN);
      return false;
    }
  } else {

    string name, value, str;

    for (int line=1; getline(in, str); line++) {

      string::size_type p0 = str.find_first_not_of("\t ");
      if ((p0 > str.size()) || (str[p0] == '#'))
	continue;

      string::size_type p = str.find("=");

      if (p == string::npos) {
	print_warning(gettext("Illegal line in theme file, missing '=' on line ") + conv::itos(line),
		      "THEMES");
	continue;
      }

      name = str.substr(0,p);
      value = str.substr(p+1);

      name = string_format::trim(name);
      value = string_format::trim(value);

      if (name == "version") {
	if (value != MMS_THEME_VERSION) {
	  if (startup) {
	    print_warning(gettext("Unsupported theme version, got version ") + value +
			  gettext(" expected ") + MMS_THEME_VERSION, "THEMES");
	  } else {
	    Print pdialog(gettext("Unsupported theme version"), Print::SCREEN);
	  }

	  in.close();
	  return false;
	}
      }

      if (name == "show_header") {
	if (value == "yes")
	  show_header = true;
	else
	  show_header = false;
      }

      if (name == "resolution_dependant_background") {
	if (value == "yes")
	  resolution_dependant_background_needed = true;
	else
	  resolution_dependant_background_needed = false;
      }

      if (name == "general.rectangle_color") {
	general_rectangle_color1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	general_rectangle_color2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	general_rectangle_color3 = atoi(value);
	value = value.substr(value.find(",")+1);
	general_rectangle_color4 = atoi(value);
      } else if (name == "general.marked_picture") {
	general_marked_picture_color1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	general_marked_picture_color2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	general_marked_picture_color3 = atoi(value);
	value = value.substr(value.find(",")+1);
	general_marked_picture_color4 = atoi(value);
      } else if (name == "general.search") {
	general_search = value;
      } else if (name == "general.search_rectangle_color") {
	general_search_rectangle_color1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	general_search_rectangle_color2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	general_search_rectangle_color3 = atoi(value);
	value = value.substr(value.find(",")+1);
	general_search_rectangle_color4 = atoi(value);
      } else if (name == "general.marked_large") {
	general_marked_large = value;
      } else if (name == "general.marked") {
	general_marked = value;
      } else if (name == "general.normal_dir") {
	general_normal_dir = value;
      } else if (name == "startmenu.font") {
	startmenu_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	startmenu_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	startmenu_font3 = atoi(value);
      } else if (name == "startmenu.background") {
	startmenu_background = resolution_dependant_background(value);
      } else if (name == "startmenu.marked_icon") {
	startmenu_marked_icon = value;
      } else if (name == "startmenu.music_dir") {
	startmenu_music_dir = value;
      } else if (name == "startmenu.music_cd") {
	startmenu_music_cd = value;
      } else if (name == "startmenu.radio") {
	startmenu_radio = value;
      } else if (name == "startmenu.movie_dir") {
	startmenu_movie_dir = value;
      } else if (name == "startmenu.movie_cd") {
	startmenu_movie_cd = value;
      } else if (name == "startmenu.pictures_dir") {
	startmenu_pictures_dir = value;
      } else if (name == "startmenu.pictures_cd") {
	startmenu_pictures_cd = value;
	  } else if (name == "startmenu.game_dir") {
	startmenu_game_dir = value;
      } else if (name == "startmenu.game_cd") {
	startmenu_game_cd = value;
      } else if (name == "startmenu.epg") {
	startmenu_epg = value;
      } else if (name == "startmenu.timer") {
	startmenu_timer = value;
      } else if (name == "startmenu.weather") {
	startmenu_weather = value;
      } else if (name == "startmenu.vbox") {
	startmenu_vbox = value;
      } else if (name == "startmenu.tv") {
	startmenu_tv = value;
      } else if (name == "startmenu.open_close") {
	startmenu_open_close = value;
      } else if (name == "startmenu.options") {
	startmenu_options = value;
      } else if (name == "startmenu.search") {
	startmenu_search = value;
      } else if (name == "startmenu.themes") {
	startmenu_themes = value;
      } else if (name == "startmenu.quit") {
	startmenu_quit = value;
      } else if (name == "audio.playback_font") {
	audio_playback_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	audio_playback_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	audio_playback_font3 = atoi(value);
      } else if (name == "notify_area.font") {
	notify_area_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	notify_area_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	notify_area_font3 = atoi(value);
      } else if (name == "audio.font") {
	audio_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	audio_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	audio_font3 = atoi(value);
      } else if (name == "audio.marked_font") {
	audio_marked_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	audio_marked_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	audio_marked_font3 = atoi(value);
      } else if (name == "audio.header_font") {
	audio_header_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	audio_header_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	audio_header_font3 = atoi(value);
      } else if (name == "audio.background") {
	audio_background = resolution_dependant_background(value);
      } else if (name == "audio.dir") {
	audio_dir = value;
      } else if (name == "audio.dir_marked") {
	audio_dir_marked = value;
      } else if (name == "audio.marked") {
	audio_marked = value;
      } else if (name == "audio.marked_left") {
	audio_marked_left = value;
      } else if (name == "audio.marked_right") {
	audio_marked_right = value;
      } else if (name == "audio.marked_small") {
	audio_marked_small = value;
      } else if (name == "audio.marked_small_left") {
	audio_marked_small_left = value;
      } else if (name == "audio.marked_small_right") {
	audio_marked_small_right = value;
      } else if (name == "audio.playlist_icon") {
	audio_playlist_icon = value;
      } else if (name == "audio.playback_pause") {
	audio_playback_pause = value;
      } else if (name == "audio.playback_play") {
	audio_playback_play = value;
      } else if (name == "audio.mute_icon") {
 	audio_mute = value;
      } else if (name == "audio.repeat_icon") {
 	audio_repeat = value;
 	  } else if (name == "audio.shutdown_icon") {
 	audio_shutdown = value;
      } else if (name == "audio.volume_icon") {
 	audio_volume = value;
      } else if (name == "audio.volume_icon_fg") {
 	audio_volume_bar = value;
      } else if (name == "audio.random_icon") {
 	audio_random = value;
      } else if (name == "audio.randomi_icon") {
 	audio_randomi = value;
      } else if (name == "audio.randomw_icon") {
 	audio_randomw = value;
      } else if (name == "audio.normal_icon") {
 	audio_normal = value;
      } else if (name == "audio.file") {
 	audio_file = value;
      } else if (name == "movie.font") {
	movie_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	movie_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	movie_font3 = atoi(value);
      } else if (name == "movie.marked_font") {
	movie_marked_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	movie_marked_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	movie_marked_font3 = atoi(value);
      } else if (name == "movie.header_font") {
	movie_header_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	movie_header_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	movie_header_font3 = atoi(value);
      } else if (name == "movie.background") {
	movie_background = resolution_dependant_background(value);
      } else if (name == "movie.dir_marked") {
	movie_dir_marked = value;
      } else if (name == "movie.marked") {
	movie_marked = value;
      } else if (name == "movie.marked_left") {
	movie_marked_left = value;
      } else if (name == "movie.marked_right") {
	movie_marked_right = value;
      } else if (name == "movie.missing_cover") {
	movie_mcover = value;
      } else if (name == "movie.rating_icon") {
	movie_rating_icon = value;
      } else if (name == "movie.rating_icon_half") {
	movie_rating_icon_half = value;
      } else if (name == "pictures.font") {
	pictures_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	pictures_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	pictures_font3 = atoi(value);
      } else if (name == "pictures.marked_font") {
	pictures_marked_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	pictures_marked_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	pictures_marked_font3 = atoi(value);
      } else if (name == "pictures.header_font") {
	pictures_header_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	pictures_header_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	pictures_header_font3 = atoi(value);
      } else if (name == "pictures.background") {
	pictures_background = resolution_dependant_background(value);
      } else if (name == "pictures.dir_marked") {
	pictures_dir_marked = value;
      } else if (name == "epg.font") {
	epg_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_font3 = atoi(value);
      } else if (name == "weather.font") {
	weather_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_font3 = atoi(value);
      } else if (name == "vbox.font") {
	vbox_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_font3 = atoi(value);
      } else if (name == "game.font") {
	game_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	game_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	game_font3 = atoi(value);
      } else if (name == "game.marked_font") {
	game_marked_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	game_marked_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	game_marked_font3 = atoi(value);
      } else if (name == "game.header_font") {
	game_header_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	game_header_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	game_header_font3 = atoi(value);
      } else if (name == "game.background") {
	game_background = resolution_dependant_background(value);
      } else if (name == "game.dir_marked") {
	game_dir_marked = value;
      } else if (name == "game.file") {
	game_file = value;
      } else if (name == "epg.header_font") {
	epg_header_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_header_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_header_font3 = atoi(value);
      } else if (name == "weather.header_font") {
	weather_header_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_header_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_header_font3 = atoi(value);
      } else if (name == "vbox.header_font") {
	vbox_header_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_header_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_header_font3 = atoi(value);
      } else if (name == "epg.time_bar") {
	epg_time_bar1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_time_bar2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_time_bar3 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_time_bar4 = atoi(value);
      } else if (name == "epg.timer_marked") {
	epg_timer_marked1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_timer_marked2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_timer_marked3 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_timer_marked4 = atoi(value);
      } else if (name == "epg.timer_boxes") {
	epg_timer_boxes1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_timer_boxes2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_timer_boxes3 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_timer_boxes4 = atoi(value);
      } else if (name == "epg.b_epg_16_9") {
    	epg_b_epg_16_9 = value;
      } else if (name == "epg.b_epg_bw") {
  		epg_b_epg_bw = value;
      } else if (name == "epg.b_epg_hd") {
		epg_b_epg_hd = value;
      } else if (name == "epg.b_epg_live") {
		epg_b_epg_live = value;
      } else if (name == "epg.b_epg_movie") {
		epg_b_epg_movie = value;
      } else if (name == "epg.b_epg_series") {
		epg_b_epg_series = value;
      } else if (name == "epg.b_epg_stereo") {
		epg_b_epg_stereo = value;
      } else if (name == "epg.b_epg_dual") {
		epg_b_epg_dual = value;
      } else if (name == "epg.b_epg_dd_2_0") {
		epg_b_epg_dd_2_0 = value;
      } else if (name == "epg.b_epg_dd_5_1") {
		epg_b_epg_dd_5_1 = value;
      } else if (name == "epg.b_epg_hoerfilm") {
		epg_b_epg_hoerfilm = value;
      } else if (name == "epg.b_epg_numb") {
		epg_b_epg_numb = value;
      } else if (name == "epg.unchecked") {
		epg_unchecked = value;
      } else if (name == "epg.checked") {
		epg_checked = value;
      } else if (name == "epg.recording") {
		epg_recording = value;
      } else if (name == "weather.time_bar") {
	weather_time_bar1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_time_bar2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_time_bar3 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_time_bar4 = atoi(value);
      } else if (name == "vbox.time_bar") {
	vbox_time_bar1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_time_bar2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_time_bar3 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_time_bar4 = atoi(value);
      } else if (name == "epg.bar") {
	epg_bar1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_bar2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_bar3 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_bar4 = atoi(value);
      } else if (name == "weather.bar") {
	weather_bar1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_bar2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_bar3 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_bar4 = atoi(value);
      } else if (name == "vbox.bar") {
	vbox_bar1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_bar2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_bar3 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_bar4 = atoi(value);
      } else if (name == "epg.marked") {
	epg_marked1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_marked2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_marked3 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_marked4 = atoi(value);
      } else if (name == "weather.marked") {
	weather_marked1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_marked2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_marked3 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_marked4 = atoi(value);
      } else if (name == "vbox.marked") {
	vbox_marked1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_marked2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_marked3 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_marked4 = atoi(value);
      } else if (name == "epg.marked_font") {
	epg_marked_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_marked_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_marked_font3 = atoi(value);
      } else if (name == "weather.marked_font") {
	weather_marked_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_marked_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_marked_font3 = atoi(value);
      } else if (name == "vbox.marked_font") {
	vbox_marked_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_marked_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_marked_font3 = atoi(value);
      } else if (name == "epg.boxes") {
	epg_boxes1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_boxes2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_boxes3 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_boxes4 = atoi(value);
      } else if (name == "weather.boxes") {
	weather_boxes1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_boxes2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_boxes3 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_boxes4 = atoi(value);
      } else if (name == "vbox.boxes") {
	vbox_boxes1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_boxes2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_boxes3 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_boxes4 = atoi(value);
      } else if (name == "epg.boxes_font") {
	epg_boxes_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_boxes_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	epg_boxes_font3 = atoi(value);
      } else if (name == "weather.boxes_font") {
	weather_boxes_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_boxes_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	weather_boxes_font3 = atoi(value);
      } else if (name == "vbox.boxes_font") {
	vbox_boxes_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_boxes_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	vbox_boxes_font3 = atoi(value);
      } else if (name == "epg.background") {
	epg_background = resolution_dependant_background(value);
      } else if (name == "weather.background") {
	weather_background = resolution_dependant_background(value);
      } else if (name == "vbox.background") {
	vbox_background = resolution_dependant_background(value);
      } else if (name == "dialog.font") {
	dialog_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	dialog_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	dialog_font3 = atoi(value);
      } else if (name == "dialog.background") {
	dialog_background1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	dialog_background2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	dialog_background3 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	dialog_background4 = atoi(value);
      } else if (name == "dialog.marked") {
	dialog_marked1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	dialog_marked2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	dialog_marked3 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	dialog_marked4 = atoi(value);
      } else if (name == "options.font") {
	options_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	options_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	options_font3 = atoi(value);
      } else if (name == "options.header_font") {
	options_header_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	options_header_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	options_header_font3 = atoi(value);
      } else if (name == "options.background") {
	options_background = resolution_dependant_background(value);
      } else if (name == "themes.font") {
	themes_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	themes_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	themes_font3 = atoi(value);
      } else if (name == "themes.marked_font") {
	themes_marked_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	themes_marked_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	themes_marked_font3 = atoi(value);
      } else if (name == "themes.header_font") {
	themes_header_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	themes_header_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	themes_header_font3 = atoi(value);
      } else if (name == "themes.background") {
	themes_background = resolution_dependant_background(value);
      } else if (name == "themes.thumbnail") {
	themes_thumbnail = value;
      } else if (name == "search.font") {
	search_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	search_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	search_font3 = atoi(value);
      } else if (name == "search.marked_font") {
	search_marked_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	search_marked_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	search_marked_font3 = atoi(value);
      } else if (name == "search.module_font") {
	search_module_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	search_module_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	search_module_font3 = atoi(value);
      } else if (name == "search.header_font") {
	search_header_font1 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	search_header_font2 = atoi(value.substr(0,value.find(",")));
	value = value.substr(value.find(",")+1);
	search_header_font3 = atoi(value);
      } else if (name == "search.background") {
	search_background = resolution_dependant_background(value);
      } else if (name == "progressbar.bar") {
	progressbar_bar = value;
      }
    }
    in.close();
  }

  return true;
}
