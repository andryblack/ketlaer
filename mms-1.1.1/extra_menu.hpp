#ifndef EXTRA_MENU_HPP
#define EXTRA_MENU_HPP

#include "image.hpp"

#include "gettext.hpp"

#include "boost.hpp"

#include <list>
#include <string>
#include <vector>

class Input;

class ExtraMenuItem
{
public:
  typedef boost::function<void (void)> callback_func;

  ExtraMenuItem(const std::string c, const std::string& s, const callback_func& cb,
		bool clean_after_calling = false);

  std::string command;
  std::string shortcut;
  bool cleanup_after_calling;

  callback_func callback;

  bool operator == (const ExtraMenuItem& rhs);
};

// ExtraMenu controls the ExtraMenuItems and thus it will automaticly delete
// them, when they are not needed anymore, so be sure to add new'ed items

class ExtraMenu
{
public:
  // items added using this function will be cleaned up upon destruction
  void add_item(const ExtraMenuItem& e);
  // items added using this function will NOT be cleaned up upon destruction
  void add_persistent_item(const ExtraMenuItem& e);

  int mainloop();

  ExtraMenu(const std::string& h = gettext("Extra Menu"), bool drop_down = false);

  void set_drop_down_pos(int x, int y);

  void add_exit_input(const Input& input);

  int starting_layer;

  std::string header_font;
  std::string element_font;

  int element_height;

private:
  std::list<Input> exit_inputs;

  std::string header;

  bool drop_down;
  int dd_x, dd_y;

  void print();
  void cleanup();

  bool audio_fullscreen_was_running;

  void wrapped_callback(const ExtraMenuItem& i);

  typedef std::pair<ExtraMenuItem, bool> persistence_pair;

  // item, persistent
  std::vector<persistence_pair> items;
  int pos;
  Overlay o;
};

#endif
