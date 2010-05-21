#include "global.hpp"
#include "input.hpp"
#include "updater.hpp"
#include "common-feature.hpp"

#include "shutdown.hpp"

#include "boost.hpp"

pthread_mutex_t Global::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

Playback::Playback(const std::string& n)
  : enabled(false), playback_name(n)
{}

void Playback::add_command(const GlobalCommand& c)
{
  commands.push_back(c);
}

bool Playback::check_commands(const Input& input) const
{
  bool status = false;

  foreach (const GlobalCommand& command, commands) {
    if (command.check_func_base(input)) {
      command.callback();
      status = true;
      break;
    }
  }

  S_ScreenUpdater::get_instance()->trigger.trigger_input(input.command);

  return status;
}

void Playback::enable()
{
  enabled = true;
}

void Playback::disable()
{
  enabled = false;
}

std::string Playback::name() const
{
  return playback_name;
}

void Playback::clean_up()
{
}

void Global::clean_up()
{
}

bool GlobalCommandBase::check_func_base(const Input& i) const
{
  return (i.command == command);
}

GlobalCommand::GlobalCommand(const callback_func& f, const check_func& c)
  : callback(f), check(c)
{}

GlobalCommand::GlobalCommand(const callback_func& f, const std::string& c, const std::string& n)
  : name(n), callback(f)
{
  command = c;
}

int Global::getBottomHeight()
{
  return bottom_height;
}

void Global::add_command(const GlobalCommand& c)
{
  commands.push_back(c);
}

bool Global::check_commands(const Input& input, bool disable_mode_change)
{
  foreach (GlobalCommand& command, commands) {
    bool ok = false;

    if (command.check != 0)
      ok = command.check(input, disable_mode_change);
    else
      ok = command.check_func_base(input);

    if (ok)
      command.callback();
  }

  foreach (Playback& playback, playbacks)
    if (playback.name() == playback_name)
      return playback.check_commands(input);

  return false;
}

void Global::add_playback(const Playback& p)
{
  playbacks.push_back(p);
}

bool Global::set_playback(const std::string& name)
{
  foreach (Playback& playback, playbacks) {
    if (playback.name() == name) {
      playback_name = name;
      return true;
    }
  }
  return false;
}

bool Global::is_playback(const std::string& name)
{
  return name == playback_name;
}

void Global::set_playback_offset(int off)
{
  offset = off;
}

int Global::get_playback_offset()
{
  return offset;
}

void Global::reconfigure_playback()
{
  if (reconfigure_callback)
    reconfigure_callback();
  else
    std::cerr << "ERROR: no playback reconfigure function bound" << std::endl;
}

void Global::register_reconfigure(const reconfigure& callback)
{
  reconfigure_callback = callback;
}

void Global::stop_bit_input_check(const Input& i)
{
  if (i.mode == "general" && i.command == "cancel")
    cancel = true;
}

void Global::reset_stop_bit()
{
  cancel = false;
}

bool Global::check_stop_bit()
{
  S_InputMaster::get_instance()->check_cancel();

  bool tmp = cancel;
  cancel = false;
  return tmp;
}

void Global::set_valid_keys(const std::string& keys)
{
  valid_keys = keys;
}

std::string Global::get_valid_keys()
{
  return valid_keys;
}

// LCD

void Global::register_lcd_output_func(const lcd_output& lcd_out)
{
  lcd_output_func = lcd_out;
}

void Global::register_lcd_print_output_func(const lcd_output_print& lcd_print)
{
  lcd_print_output_func = lcd_print;
  registered_lcd_output = true;
}

bool Global::lcd_output_possible()
{
  return registered_lcd_output;
}

void Global::set_lcd_resolution(int cols, int r)
{
  rows = r;
  pixels = cols;
}

int Global::lcd_rows()
{
  return rows;
}

int Global::lcd_pixels()
{
  return pixels;
}

void Global::lcd_add_output(const std::string& message)
{
  lcd_output_func(message);
}

void Global::lcd_print()
{
  lcd_print_output_func();
}

// keypress
void Global::register_keypress_filter(boost::function<bool (Input)> func)
{
  filter_keypress_functions.push_back(func);
}

bool Global::check_keypress_filters(Input input)
{
  foreach (boost::function<bool (Input)>& filter_func, filter_keypress_functions)
    if (filter_func(input))
      return true;

  return false;
}

// Movie

bool Global::movie_playback_possible()
{
  return registered_movie_playback;
}

void Global::register_movie_playback_func(const play_movie_callback& movie_playback)
{
  movie_playback_func = movie_playback;
  registered_movie_playback = true;
}

void Global::play_movie(const std::string& path)
{
  movie_playback_func(path);
}

void Global::add_second_menu_item(const ExtraMenuItem& e)
{
  menu_items.push_back(e);
}

Global::Global()
  : autostarting(false), playback_in_fullscreen(false), cancel(false), 
    playback_name(""), registered_lcd_output(false), registered_movie_playback(false)
{
  input_idle.reset();

  InputMaster *input_master = S_InputMaster::get_instance();
  input_master->set_map("default");
  add_second_menu_item(ExtraMenuItem(gettext("Playback control"),
				     input_master->find_shortcut("playback_commands"),
				     boost::bind(&Global::playlist_commands, this)));

  add_command(GlobalCommand(boost::bind(&Global::playlist_commands, this),
			    boost::bind(&Global::check_playlist_commands, this, _1, _2)));

  add_command(GlobalCommand(boost::bind(&Global::do_shutdown, this),
			    boost::bind(&Global::check_shutdown_command, this, _1, _2)));

  Config *conf = S_Config::get_instance();

  bottom_height = 2 * graphics::calculate_font_height(graphics::resolution_dependant_font_wrapper(17, conf), conf) + 5;
}

Global::~Global()
{
  reconfigure_callback = 0;
  lcd_output_func = 0;
  lcd_print_output_func = 0;
  movie_playback_func = 0;
}

bool Global::check_shutdown_command(const Input& input, bool disable_mode_change)
{
  return input.command == "shutdown";
}

void Global::do_shutdown()
{
  Shutdown *sd = S_Shutdown::get_instance();

  if (sd->is_enabled())
    sd->cancel();
  else
    sd->enable();
}

bool Global::check_playlist_commands(const Input& input, bool disable_mode_change)
{
  return (!disable_mode_change && input.command == "playback_commands");
}

void Global::playlist_commands()
{
  if (playback_name.empty())
    return;

  ExtraMenu em = ExtraMenu(gettext("Playlist Commands"));

  InputMaster *input_master = S_InputMaster::get_instance();

  foreach (Playback& playback, playbacks) {
    if (playback.name() == playback_name) {
      foreach (GlobalCommand& command, playback.commands) {
	em.add_persistent_item(ExtraMenuItem(command.name,
					     input_master->find_shortcut(command.command),
					     command.callback));
      }
      break;
    }
  }

  em.mainloop();
}

void Global::sdl_input_ready()
{
  input_idle.signal();
}

void Global::sdl_wait_for_input_ready()
{
  input_idle.wait();
  input_idle.reset();
}

