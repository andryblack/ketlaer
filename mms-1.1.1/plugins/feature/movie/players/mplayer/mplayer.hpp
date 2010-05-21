#ifndef MPLAYER_HPP
#define MPLAYER_HPP

#include "movie_player.hpp"

#include "mplayer_config.hpp"

#include "common-feature.hpp"

#include <cc++/thread.h>

class PlaybackChecker;

class Mplayer : public MoviePlayer
{
public:
  Mplayer();
  ~Mplayer();

  bool wants_exclusivity();

  void play_movie(const std::string& paths, bool window = false);
  void play_disc(const std::string& play_cmd);
  void play_vcd(const std::string& device);
  void play_dvd(const std::string& device);

  bool create_thumbnail(const std::string& file, const std::string& thumbnail, int width, int height);

  void play();
  void pause();
  void stop();
  void ff();
  void fb();
  void switch_screen_size();
  bool fullscreen();
  bool playing();

  void send_command(const std::string& command);

  Pipe p;
  bool paused, running, in_fullscreen;
  float pos;
  bool stop_playback_ok;
  bool provides_options() { return true; }

  Option  *gen_head_opt_p, *dvd_head_opt_p, *vcd_head_opt_p, *mov_head_opt_p,
    *dvd_audio_lang_p, *sub_lang_p, *sub_font_p, *sub_size_p, *vcdtitle_p, *dvdtitle_p, *monitor_aspect_p,
    *force_geometry_p, *full_screen_p;

private:

  pid_t external_pid;
  std::string thumbnail_path;

  void add_thumbnail(const std::string& command, const std::string& thumb_path);

  MplayerConfig *mplayer_conf;

  void killall_thumbnailers();

  int check_playback_timeout();
  void check_playback();

  void stop_playback();

  std::string get_audio_mode();

  void setup_playback(const std::pair<bool, Pipe>& result, bool window);
  void playback_ended();

  void stop_and_switch();

  void exclusive_top();
  void setup_movie_in_slave_mode(const std::string& audio_mode, const std::string& sub_font, const std::string& sub_size, const std::string& sub_lang,
				 const std::string& audio_lang, const std::string& paths,
				 bool paths_find, bool window);
  void playback(const std::string& paths, bool paths_find, const std::string& audio_lang,
		const std::string& opts, bool window);
  std::pair<bool, Pipe> play_movie_in_slave_mode(char *buffer, const std::string& sub_font, const std::string& sub_size, const std::string& sub_lang,
						 const std::string& audio_lang,
						 const std::string& audio_mode, const std::string& paths,
						 bool paths_find);

  bool broken_pipe;

  enum PlaybackType { MOVIE, MOVIE_DISC, VCD, DVD };

  // for switching between fullscreen and window
  PlaybackType type;
  std::string path;

  PlaybackChecker *checker;
};

class PlaybackChecker : public ost::Thread
{
public:
  PlaybackChecker(Mplayer *mplayer);

private:
  void run();
  Mplayer *player;
};

#endif
