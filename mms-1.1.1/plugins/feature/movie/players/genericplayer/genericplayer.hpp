#ifndef GENERICPLAYER_HPP
#define GENERICPLAYER_HPP

#include "movie_player.hpp"

#include "genericplayer_config.hpp"

class goptstruct{
public:
  char shortcut;
  Option *realopt;
  goptstruct(char s_c, bool t, const std::string& n, const std::string& e_n, int p,
	     const std::vector<std::string>& v, const std::vector<std::string>& e_v);
};

class GenericPlayer : public MoviePlayer
{
public:
  GenericPlayer();
  ~GenericPlayer();
  bool wants_exclusivity() { return true; }

  void play_movie(const std::string& paths, bool window = false);
  void play_disc(const std::string& play_cmd);
  void play_vcd(const std::string& device);
  void play_dvd(const std::string& device);
  bool provides_options() { return initialized; }
private:
  std::string parse_and_complete(const std::string& options, const std::string& optd, const std::string& path="");
  GenericPlayerConfig *genericp_conf;
  bool load_opt_file();
  bool initialized;
  std::string opt_file;
  std::vector<goptstruct> gopt_p;
  std::string find_opt_val(char s_c); //finds the correct option value matching the given shortcut
};

#endif
