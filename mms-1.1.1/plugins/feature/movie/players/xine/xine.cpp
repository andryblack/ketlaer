#include "xine.hpp"

#include "common-feature.hpp"
#include "movie_config.hpp"

using std::string;

XineMovie::XineMovie()
  : MoviePlayer(true, true, true, false)
{
  xine_conf = S_XineConfig::get_instance();

  input = " -i kybd";
  if (list_contains(conf->p_input(), string("lirc")))
    input = " -i lirc";
}

void XineMovie::play_movie(const std::string& paths, bool window)
{
  run::exclusive_external_program(xine_conf->p_cxfe_path() + ' ' + xine_conf->p_cxfe_opts() +
				  input + ' ' + paths);
}

void XineMovie::play_disc(const std::string& play_cmd)
{
  string command = play_cmd + xine_conf->p_cxfe_path() + ' ' + xine_conf->p_cxfe_opts() +
    input + ' ';

  run::exclusive_external_program(command);
}

void XineMovie::play_vcd(const std::string& device)
{
  run::exclusive_external_program(xine_conf->p_cxfe_path() + ' ' + xine_conf->p_cxfe_opts() +
				  input + " -dev " + device + ' ' + "vcd://");
}

void XineMovie::play_dvd(const std::string& device)
{
  run::exclusive_external_program(xine_conf->p_cxfe_path() + ' ' + xine_conf->p_cxfe_opts() +
				  input + " dvd://'" + device + "'");
}
