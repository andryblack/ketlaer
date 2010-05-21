#ifndef THEMES_HPP
#define THEMES_HPP

#include "singleton.hpp"

#include <string>

class Themes
{
private:

  std::string resolution_dependant_background(const std::string& bg);

  bool resolution_dependant_background_needed;

public:

  static pthread_mutex_t singleton_mutex;

  Themes();

  // theme def

  bool show_header;

  int general_rectangle_color1;
  int general_rectangle_color2;
  int general_rectangle_color3;
  int general_rectangle_color4;
  int general_marked_picture_color1;
  int general_marked_picture_color2;
  int general_marked_picture_color3;
  int general_marked_picture_color4;
  std::string general_search;
  int general_search_rectangle_color1;
  int general_search_rectangle_color2;
  int general_search_rectangle_color3;
  int general_search_rectangle_color4;
  std::string general_marked_large;
  std::string general_marked;
  std::string general_normal_dir;

  int startmenu_font1;
  int startmenu_font2;
  int startmenu_font3;
  std::string startmenu_background;
  std::string startmenu_marked_icon;

  std::string startmenu_music_dir;
  std::string startmenu_music_cd;
  std::string startmenu_radio;
  std::string startmenu_movie_dir;
  std::string startmenu_movie_cd;
  std::string startmenu_pictures_dir;
  std::string startmenu_pictures_cd;
  std::string startmenu_game_dir;
  std::string startmenu_game_cd;
  std::string startmenu_epg;
  std::string startmenu_timer;
  std::string startmenu_weather;
  std::string startmenu_vbox;
  std::string startmenu_tv;
  std::string startmenu_open_close;
  std::string startmenu_options;
  std::string startmenu_search;
  std::string startmenu_themes;
  std::string startmenu_quit;

  int notify_area_font1;
  int notify_area_font2;
  int notify_area_font3;

  int audio_playback_font1;
  int audio_playback_font2;
  int audio_playback_font3;

  int audio_font1;
  int audio_font2;
  int audio_font3;
  int audio_marked_font1;
  int audio_marked_font2;
  int audio_marked_font3;
  int audio_header_font1;
  int audio_header_font2;
  int audio_header_font3;
  std::string audio_background;
  std::string audio_dir;
  std::string audio_dir_marked;
  std::string audio_marked_left;
  std::string audio_marked;
  std::string audio_marked_right;
  std::string audio_marked_small_left;
  std::string audio_marked_small;
  std::string audio_marked_small_right;
  std::string audio_playlist_icon;
  std::string audio_playback_pause;
  std::string audio_playback_play;
  std::string audio_mute;
  std::string audio_repeat;
  std::string audio_shutdown;
  std::string audio_volume;
  std::string audio_volume_bar;
  std::string audio_file;
  std::string audio_random;
  std::string audio_randomi;
  std::string audio_randomw;
  std::string audio_normal;

  int movie_font1;
  int movie_font2;
  int movie_font3;
  int movie_marked_font1;
  int movie_marked_font2;
  int movie_marked_font3;
  int movie_header_font1;
  int movie_header_font2;
  int movie_header_font3;
  std::string movie_background;
  std::string movie_dir_marked;
  std::string movie_marked;
  std::string movie_marked_left;
  std::string movie_marked_right;
  std::string movie_mcover;
  std::string movie_rating_icon;
  std::string movie_rating_icon_half;

  int pictures_font1;
  int pictures_font2;
  int pictures_font3;
  int pictures_marked_font1;
  int pictures_marked_font2;
  int pictures_marked_font3;
  int pictures_header_font1;
  int pictures_header_font2;
  int pictures_header_font3;
  std::string pictures_background;
  std::string pictures_dir_marked;

  int game_font1;
  int game_font2;
  int game_font3;
  int game_marked_font1;
  int game_marked_font2;
  int game_marked_font3;
  int game_header_font1;
  int game_header_font2;
  int game_header_font3;
  std::string game_background;
  std::string game_dir_marked;
  std::string game_file;

  int epg_font1;
  int epg_font2;
  int epg_font3;
  int epg_header_font1;
  int epg_header_font2;
  int epg_header_font3;
  int epg_time_bar1;
  int epg_time_bar2;
  int epg_time_bar3;
  int epg_time_bar4;
  int epg_bar1;
  int epg_bar2;
  int epg_bar3;
  int epg_bar4;
  int epg_marked1;
  int epg_marked2;
  int epg_marked3;
  int epg_marked4;
  int epg_timer_marked1;
  int epg_timer_marked2;
  int epg_timer_marked3;
  int epg_timer_marked4;
  int epg_marked_font1;
  int epg_marked_font2;
  int epg_marked_font3;
  int epg_boxes1;
  int epg_boxes2;
  int epg_boxes3;
  int epg_boxes4;
  int epg_timer_boxes1;
  int epg_timer_boxes2;
  int epg_timer_boxes3;
  int epg_timer_boxes4; 
  int epg_boxes_font1;
  int epg_boxes_font2;
  int epg_boxes_font3;
  std::string epg_background;

  std::string epg_b_epg_16_9;
  std::string epg_b_epg_bw;
  std::string epg_b_epg_hd;
  std::string epg_b_epg_live;
  std::string epg_b_epg_movie;
  std::string epg_b_epg_series;
  std::string epg_b_epg_stereo;
  std::string epg_b_epg_dual;
  std::string epg_b_epg_dd_2_0;
  std::string epg_b_epg_dd_5_1;
  std::string epg_b_epg_hoerfilm;
  std::string epg_b_epg_numb;
  std::string epg_unchecked;
  std::string epg_checked;
  std::string epg_recording;

  int weather_font1;
  int weather_font2;
  int weather_font3;
  int weather_header_font1;
  int weather_header_font2;
  int weather_header_font3;
  int weather_time_bar1;
  int weather_time_bar2;
  int weather_time_bar3;
  int weather_time_bar4;
  int weather_bar1;
  int weather_bar2;
  int weather_bar3;
  int weather_bar4;
  int weather_marked1;
  int weather_marked2;
  int weather_marked3;
  int weather_marked4;
  int weather_marked_font1;
  int weather_marked_font2;
  int weather_marked_font3;
  int weather_boxes1;
  int weather_boxes2;
  int weather_boxes3;
  int weather_boxes4;
  int weather_boxes_font1;
  int weather_boxes_font2;
  int weather_boxes_font3;
  std::string weather_background;

  int vbox_font1;
  int vbox_font2;
  int vbox_font3;
  int vbox_header_font1;
  int vbox_header_font2;
  int vbox_header_font3;
  int vbox_time_bar1;
  int vbox_time_bar2;
  int vbox_time_bar3;
  int vbox_time_bar4;
  int vbox_bar1;
  int vbox_bar2;
  int vbox_bar3;
  int vbox_bar4;
  int vbox_marked1;
  int vbox_marked2;
  int vbox_marked3;
  int vbox_marked4;
  int vbox_marked_font1;
  int vbox_marked_font2;
  int vbox_marked_font3;
  int vbox_boxes1;
  int vbox_boxes2;
  int vbox_boxes3;
  int vbox_boxes4;
  int vbox_boxes_font1;
  int vbox_boxes_font2;
  int vbox_boxes_font3;
  std::string vbox_background;

  int dialog_font1;
  int dialog_font2;
  int dialog_font3;
  int dialog_background1;
  int dialog_background2;
  int dialog_background3;
  int dialog_background4;
  int dialog_marked1;
  int dialog_marked2;
  int dialog_marked3;
  int dialog_marked4;

  int options_font1;
  int options_font2;
  int options_font3;
  int options_header_font1;
  int options_header_font2;
  int options_header_font3;
  std::string options_background;

  int themes_font1;
  int themes_font2;
  int themes_font3;
  int themes_marked_font1;
  int themes_marked_font2;
  int themes_marked_font3;
  int themes_header_font1;
  int themes_header_font2;
  int themes_header_font3;
  std::string themes_background;
  std::string themes_thumbnail;

  int search_font1;
  int search_font2;
  int search_font3;
  int search_marked_font1;
  int search_marked_font2;
  int search_marked_font3;
  int search_module_font1;
  int search_module_font2;
  int search_module_font3;
  int search_header_font1;
  int search_header_font2;
  int search_header_font3;
  std::string search_background;

  std::string progressbar_bar;

  bool parser(const std::string& cur_theme, bool startup);
};

typedef Singleton<Themes> S_Themes;

#endif
