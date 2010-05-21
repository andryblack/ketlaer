#ifndef XINE_HPP
#define XINE_HPP

#include "movie_player.hpp"

#include "xine_config.hpp"

class XineMovie : public MoviePlayer
{
public:
  XineMovie();

  bool wants_exclusivity() { return true; }

  void play_movie(const std::string& paths, bool window = false);
  void play_disc(const std::string& play_cmd);
  void play_vcd(const std::string& device);
  void play_dvd(const std::string& device);

private:

  std::string input;

  XineConfig *xine_conf;
};

#endif
