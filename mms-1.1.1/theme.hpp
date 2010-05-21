#ifndef THEME_HPP
#define THEME_HPP

#include "module.hpp"
#include "singleton.hpp"

class Theme : public Module
{
private:
  void print();
  std::vector<std::string> list_of_themes;
  int pos;

  void find_theme_and_use(const std::string& theme_name);

  std::vector<std::string> list_themes();
  void print_string_element(const std::string& r, const std::string& position, int y);

  bool exit_loop;

  void use_theme();
  void exit();

  std::string header_font;
  std::string element_font;
  std::string position_font;
  int element_height;

  void res_dependant_calc();

public:

  static pthread_mutex_t singleton_mutex;

  Theme();

  Options* get_opts()
  { return 0; }

  void startup_updater();

  void load();
  void save();

  std::string cur_theme;

  std::string mainloop();
};

typedef Singleton<Theme> S_Theme;

#endif
