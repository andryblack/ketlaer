#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include "renderer.hpp"
#include "themes.hpp"
#include "input.hpp"
#include "config.hpp"
#include "global_options.hpp"
#include "option.hpp"
#include "global.hpp"

#include <vector>
#include <string>
#include <list>

class Options
{
public:
  bool mainloop();
  void print(const std::string& marking, int pos);
  virtual ~Options() {};

  std::string get_header() { return translated_header; };
  std::string get_english_header() { return header; };
  std::vector<Option*> values() { return val; };

  void save();
  void load();

  Options();

protected:

  void print_lcd_menu(const std::string& marking);

  std::vector<Option*> val;
  std::string header;
  std::string translated_header;
  std::string save_name;

  // implemented in terms of:
  InputMaster *im;
  Render *render;
  Themes *themes;
  Config *conf;
  GlobalOptions *go;

private:
  void print_element(const std::string& element, const std::string& value,
		     bool marked, int x, int y, int max_size, int max_element_size);
  void print_reload_element(bool marked, int x, int y, int max_size, int max_element_size);

  void exit();

  bool exit_loop;

  Global *global;

  std::string header_font;
  std::string element_header_font;
  std::string element_font;
  int element_height;

  void res_dependant_calc();
};

#endif
