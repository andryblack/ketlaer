#include "movie_player.hpp"

#include "movie_config.hpp"

MoviePlayer::MoviePlayer(bool mp, bool vp, bool dp, bool cp)
  : movie_p(mp), vcd_p(vp), dvd_p(dp), control_p(cp)
{
  conf = S_Config::get_instance();
  movie_conf = S_MovieConfig::get_instance();
}
