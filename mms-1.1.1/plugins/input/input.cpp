// for prefix

//#define DLOG

#include "config.h"

#include "input.hpp"

#include "config.hpp"
#include "common.hpp"

#include "updater.hpp"

#include "touch.hpp"
#include "busy_indicator.hpp"
#include "global.hpp"
#include "replay.hpp"

#include "gettext.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

using std::string;

pthread_mutex_t InputMaster::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

string Input::to_string() const
{
  return mode + "," + key + "," + command + "," + name + "," + conv::btos(multiple);
}

Input::Input(const string& c, const string m)
  : multiple(false)
{
  command = c;
  mode = m;
}

Input::Input()
  : multiple(false)
{}

Input::Input(string& s)
{
  string::size_type comma = s.find(",");
  mode = s.substr(0, comma);
  s = s.substr(comma+1);

  comma = s.find(",");
  key = s.substr(0, comma);
  s = s.substr(comma+1);

  comma = s.find(",");
  command = s.substr(0, comma);
  s = s.substr(comma+1);

  comma = s.find(",");
  name = s.substr(0, comma);
  s = s.substr(comma+1);

  multiple = conv::stob(s);
}

InputMaster::InputMaster()
  : cancel_callback(0), touch_action(false), depth(""), do_filter(false)
{
  process_input = false;
}

void InputMaster::input_devices_loaded()
{
  foreach (InputDevice* device, devices) {
    device->parse_keys("default", "6");
    device->parse_keys("search", "3");
  }

  process_input = true;
}

void InputMaster::set_default_device(const string& name)
{
  default_device = name;
}

string InputMaster::find_shortcut(const string& command)
{
  if (!process_input)
    return "";

  foreach (InputDevice* device, devices) {
    if (device->name() == default_device) {
      foreach (const Input& input, device->cur_list())
	if (command == input.command)
	  return input.presentation;
    }
  }

  std::cerr << "something wrong:" << command << " not found" << std::endl;

  return "";
}

bool InputMaster::restore_map_empty()
{
  return saved_map.empty();
}

void InputMaster::save_map()
{
  saved_map = cur_map;
}

void InputMaster::restore_map()
{
  cur_map = saved_map;
}

void InputMaster::set_map(const string& name)
{
  cur_map = name;
}

string InputMaster::current_saved_map()
{
  return saved_map;
}

string InputMaster::current_map()
{
  return cur_map;
}

Input InputMaster::get_input_busy_wrapped(BusyIndicator *indicator)
{
  indicator->idle();
  Input return_value = get_input();
  indicator->busy();

  return return_value;
}

void InputMaster::check_cancel()
{
  if (input_ready.wait(0)) {
    Input input;

    inputs_mutex.enterMutex();
    if (inputs.size() > 0)
      input = inputs.front().first;
    inputs_mutex.leaveMutex();

    if (input.command == "cancel") {
      run_cancel_callback();
      S_Global::get_instance()->stop_bit_input_check(input);
    }
  }
}

Input InputMaster::get_input()
{
  S_Global::get_instance()->reset_stop_bit();

  Input input;

  input_ready.wait();
  input_ready.reset();

  inputs_mutex.enterMutex();
  if (inputs.size() > 0) {
    input = inputs.front().first;
    last_input_plugin = inputs.front().second;
    inputs.pop();
  }
  inputs_mutex.leaveMutex();

  if (input.command == "cancel")
    run_cancel_callback();

  S_Global::get_instance()->stop_bit_input_check(input);

  // FIXME: check if plugin is available?
  ReplaySaver *r = S_ReplaySaver::get_instance();
  r->add_event(input);

  register_keypress(input);
  
  inputs_mutex.enterMutex();
  Input original_input = input;

  bool found = false;
  if (do_filter){
    foreach (std::string& exc, filter_exceptions)
      if (exc == input.command){
	found = true;
	break;
      }
  }

  if (!do_filter || found){
    for (size_t t = 0; t < input_hooks.size(); ++t)
      if (input_hooks[t].hooked_command.empty() || input_hooks[t].hooked_command == input.command){
	dprintf("Executing callback %s for command %s\n", input_hooks[t].hook_name.c_str(), input.command.c_str());
	input_hooks[t].cb_hook(input);
      }
  }

  else{
    input.command = "_IGNORE_";
    for (size_t t = 0; t < input_hooks.size(); ++t)
      if (input_hooks[t].override_filters &&
	  (input_hooks[t].hooked_command.empty() || input_hooks[t].hooked_command == original_input.command)){
	dprintf("Executing callback %s for command %s\n", input_hooks[t].hook_name.c_str(), input.command.c_str());
	input_hooks[t].cb_hook(original_input);
      }
  }

  inputs_mutex.leaveMutex();
  dprintf("Input command: %s -> %s\n", input.command.c_str(), input.mode.c_str());
  return input;
}

void InputMaster::register_keypress(Input input)
{
  if (!S_Global::get_instance()->check_keypress_filters(input)) {
    // update time of last keypress
    S_Config::get_instance()->s_last_key(time(0));
  }
}

void InputMaster::add_input_device(InputDevice *device)
{
  devices.push_back(device);
}

void InputMaster::add_input(Input input, const std::string& plugin)
{
  inputs_mutex.enterMutex();
  if (inputs.size() == 0) {
    inputs.push(std::make_pair(input, plugin));
    input_ready.signal();
  }
  inputs_mutex.leaveMutex();
}

void InputMaster::start_devices()
{
  foreach (InputDevice* device, devices)
    device->start();
}

void InputMaster::suspend()
{
  foreach (InputDevice* device, devices)
    device->suspend();
}

void InputMaster::wake_up()
{
  foreach (InputDevice* device, devices)
    device->wake_up();
}

bool InputMaster::parse_keys(const std::string& plugin, const std::string& version)
{
  bool status = false;

  foreach (InputDevice* device, devices)
    status = device->parse_keys(plugin, version);

  return status;
}

bool InputMaster::extend_default(const std::string& plugin, const std::string& version)
{
  bool status = false;

  foreach (InputDevice* device, devices)
    status = device->extend_default(plugin, version);

  return status;
}

void InputMaster::register_cancel_callback(const boost::function<void (void)>& callback)
{
  cancel_callback = callback;
}

void InputMaster::clear_cancel_callback()
{
  cancel_callback = 0;
}

void InputMaster::run_cancel_callback()
{
  if (cancel_callback != 0)
    cancel_callback();
}

void InputMaster::trigger_input(Input input)
{
  S_ScreenUpdater::get_instance()->trigger.trigger_input(input.command);
}

void InputMaster::busy_idle(BusyIndicator *indicator)
{
  indicator->idle();
}

void InputMaster::add_input_hook(const InputHook &ih){
  inputs_mutex.enterMutex();
  input_hooks.push_back(ih);
  inputs_mutex.leaveMutex();
}

void InputMaster::del_input_hook(const std::string hookname){
  inputs_mutex.enterMutex();
  for (std::vector<InputHook>::iterator i = input_hooks.begin(); i != input_hooks.end(); )
    if (i->hook_name == hookname)
      i = input_hooks.erase(i);
    else
      ++i;

  inputs_mutex.leaveMutex();
}

void InputMaster::clear_input_hooks(){
  inputs_mutex.enterMutex();
  input_hooks.clear();
  inputs_mutex.leaveMutex();
}

