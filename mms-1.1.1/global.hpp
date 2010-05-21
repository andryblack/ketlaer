#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include "singleton.hpp"

#include "extra_menu.hpp"
#include "plugins/input/input.hpp"

#include "boost.hpp"

#include <vector>
#include <string>

class GlobalCommandBase
{
public:
  std::string command;
  bool check_func_base(const Input& i) const;
};

class GlobalCommand : public GlobalCommandBase
{
public:
  typedef boost::function<void (void)> callback_func;
  typedef boost::function<bool (const Input&, bool)> check_func;

  GlobalCommand(const callback_func& f, const check_func& c);
  GlobalCommand(const callback_func& f, const std::string& c, const std::string& n);

  std::string name;
  callback_func callback;
  check_func check;
};

class Playback
{
public:
  Playback(const std::string& name);

  // possible commands are: play, pause, stop, ff, fb
  void add_command(const GlobalCommand& c);
  // check input against registered playback commands
  bool check_commands(const Input& i) const;

  void enable();
  void disable();

  void clean_up();

  std::string name() const;

  std::vector<GlobalCommand> commands;

private:
  bool enabled;
  std::string playback_name;
};

// Global controls the GlobalCommands and thus it will automaticly delete
// them, when they are not needed anymore

class Global
{
 public:

  bool autostarting;

  void clean_up();

  // add a global command that all modules will check
  void add_command(const GlobalCommand& c);
  // check input against all global commands
  bool check_commands(const Input& i, bool disable_mode_change = false);

  void add_playback(const Playback& p);
  // sets the playback to the player with the specified name
  bool set_playback(const std::string& name);
  bool is_playback(const std::string& name);

  bool playback_in_fullscreen;

  // any plugin can offset the playback representation in the vertical direction
  void set_playback_offset(int offset);
  int get_playback_offset();

  typedef boost::function<void (void)> reconfigure;

  // sdl hack
  void sdl_input_ready();
  void sdl_wait_for_input_ready();

  // callback for reconfiguring device on change
  void reconfigure_playback();
  void register_reconfigure(const reconfigure& callback);

  // for cancelation
  void reset_stop_bit();
  void stop_bit_input_check(const Input& i);
  bool check_stop_bit();

  // remote controls
  void set_valid_keys(const std::string& keys);
  std::string get_valid_keys();

  // lcd output functions
  bool lcd_output_possible();
  void lcd_add_output(const std::string& message);
  void lcd_print();
  void set_lcd_resolution(int cols, int rows);
  int lcd_pixels();
  int lcd_rows();

  // keypress registration
  void register_keypress_filter(boost::function<bool (Input)> func);
  bool check_keypress_filters(Input input);

  // audio
  typedef boost::function<void (void)> toggle_playlist_callback;
  toggle_playlist_callback toggle_playlist;

  typedef boost::function<void (const std::string&)> lcd_output;
  typedef boost::function<void (void)> lcd_output_print;

  void register_lcd_output_func(const lcd_output& lcd_out);
  void register_lcd_print_output_func(const lcd_output_print& lcd_print);

  // movie playback functions
  bool movie_playback_possible();
  void play_movie(const std::string& path);

  typedef boost::function<void (const std::string&)> play_movie_callback;

  void register_movie_playback_func(const play_movie_callback& movie_playback);

  // global menu items
  void add_second_menu_item(const ExtraMenuItem& e);
  std::vector<ExtraMenuItem> menu_items;

  static pthread_mutex_t singleton_mutex;

  Global();
  ~Global();

  int getBottomHeight();

 private:

  int bottom_height;

  ost::Event input_idle;

  std::list<boost::function<bool (Input)> > filter_keypress_functions;

  bool check_shutdown_command(const Input& input, bool disable_mode_change);
  void do_shutdown();

  void playlist_commands();
  bool check_playlist_commands(const Input& input, bool disable_mode_change);

  // cancelation
  bool cancel;

  // remote
  std::string valid_keys;

  std::vector<GlobalCommand> commands;
  std::vector<Playback> playbacks;
  std::string playback_name;

  int offset;

  // Audio
  reconfigure reconfigure_callback;

  // LCD
  lcd_output lcd_output_func;
  lcd_output_print lcd_print_output_func;
  bool registered_lcd_output;
  int pixels;
  int rows;

  // Movie
  play_movie_callback movie_playback_func;
  bool registered_movie_playback;
};

typedef Singleton<Global> S_Global;

#endif
