#ifndef STARTMENU_HPP
#define STARTMENU_HPP

#include "config.hpp"
#include "options.hpp"
#include "themes.hpp"
#include "singleton.hpp"
#include "global.hpp"

#include "boost.hpp"

class startmenu_item
{
public:
  typedef boost::function<void (void)> callback_func;

  std::string name;
  std::string action;
  std::string pic;
  int id;

  callback_func callback;

  startmenu_item(const std::string& n, const std::string& a, const std::string& p, int i,
		 const callback_func& f);
};

struct startmenu_row
{
  std::string name;
  std::list<startmenu_item> items;
};

class Startmenu
{
private:
  std::list<startmenu_row> startmenu;

  Global *global;

  void print_options(const std::string& marking);
  void print(const startmenu_row& row, const startmenu_item& current_item);
  void print_lcd_menu(const startmenu_row& row, const startmenu_item& current_item);

  class StartmenuOpts : public Options
  {
  public:
    ~StartmenuOpts();

    void set_options();
    void save();
    void load();

  private:
    Option *a_o, *m_o, *p_o, *g_o;
  };

  Config *conf;

  void quit_func();
  bool exit_loop;

  std::string element_font;
  int element_height;

  void res_dependant_calc();

public:

  static pthread_mutex_t singleton_mutex;

  Startmenu();

  void print_status();

  void generate_startmenu();

  std::string mainloop();

  StartmenuOpts opts;
};

typedef Singleton<Startmenu> S_Startmenu;

#endif
