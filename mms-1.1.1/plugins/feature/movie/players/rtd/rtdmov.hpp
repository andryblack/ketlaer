#ifndef RTDMOV_HPP
#define RTDMOV_HPP

#include "movie_player.hpp"

class RtdMov : public MoviePlayer
{
public:
  RtdMov();
  ~RtdMov();

  bool wants_exclusivity() { return true; }
  void play_movie(const std::string& paths, bool window = false);
  void play_disc(const std::string& play_cmd);
  void play_vcd(const std::string& device);
  void play_dvd(const std::string& device);
  bool provides_options() { return false; }
private:
};

#endif
