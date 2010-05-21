#ifndef MOVIE_PLAYERS_HPP
#define MOVIE_PLAYERS_HPP

#include "config.hpp"
#include "options.hpp"
#include <string>

class MovieConfig;

class MoviePlayer
{
protected:
  // playback abilities
  bool movie_p, vcd_p, dvd_p, control_p;

  MoviePlayer(bool mp, bool vp, bool dp, bool cp);

  Config *conf;
  MovieConfig *movie_conf;

public:

  virtual ~MoviePlayer() {};

  virtual bool wants_exclusivity() = 0;

  // playback abilities
  bool movie_playback() { return movie_p; }
  bool vcd_playback() { return vcd_p; }
  bool dvd_playback() { return dvd_p; }
  bool control_playback() { return control_p; }

  virtual void play_movie(const std::string& paths, bool window = false) = 0;
  virtual void play_disc(const std::string& play_cmd) = 0;
  virtual void play_vcd(const std::string& device) = 0;
  virtual void play_dvd(const std::string& device) = 0;

  virtual bool create_thumbnail(const std::string& UNUSED(file), const std::string& UNUSED(thumbnail), int UNUSED(width), int UNUSED(height)) { return false; };

  virtual std::string audio_setup() { return ""; }

  // control players must override
  virtual void play() {}
  virtual void pause() {}
  virtual void stop() {}
  virtual void ff() {}
  virtual void fb() {}
  virtual void switch_screen_size() {}
  virtual bool fullscreen() { return false; }
  virtual bool playing() { return false; }
  virtual bool provides_options() { return false; }

  class PlayerOpts : public Options
  {
  public:
    void add_opt(Option *_opt) { val.push_back(_opt); }
    void set_header(const std::string &_str) { header =_str; }
    void set_save_name(const std::string &_str) { save_name =_str; }
  };

  PlayerOpts opts;
  PlayerOpts* get_opts() { return &opts; }
};

#endif
