#ifndef INPUT_HPP
#define INPUT_HPP

#include "config.h"

#include "common.hpp"
#include "config.hpp"
#include "updater.hpp"
#include "extra_menu.hpp"
#include "touch.hpp"
#include "input_device.hpp"

#include "singleton.hpp"

#include "boost.hpp"

#include <cc++/thread.h>

#include "libfspp.hpp"

#include <string>
#include <map>
#include <vector>
#include <queue>

#include <iostream>

// structure used for input keys


class Input
{
public:
  std::string mode, key, command, presentation, name;
  bool multiple;

  bool operator==(const Input& i) const
  {
    return mode == i.mode && command == i.command;
  }

  std::string to_string() const;
  // create from string
  Input(std::string& s);
  Input(const std::string& command, const std::string mode);

  Input();
};

class InputHook
{
  public:
    std::string hooked_command;
    std::string hook_name;
    typedef boost::function<void(const Input&)> callback_hook;
    callback_hook cb_hook;
    bool override_filters;
    InputHook(const std::string &hookname, const std::string &command, const callback_hook& hook, bool ignore_filters = false){
      cb_hook = hook;
      hooked_command = command;
      hook_name = hookname;
      override_filters = ignore_filters;
    }
    virtual ~InputHook(){};
};


// forward decleration
class BusyIndicator;

class InputMaster
{
private:

  std::string saved_map, cur_map;
  bool process_input;

  std::list<InputDevice*> devices;

  std::queue<std::pair<Input, std::string> > inputs;
  ost::Mutex inputs_mutex;
  ost::Event input_ready;

  void register_keypress(Input input);

  std::string default_device;

  boost::function<void (void)> cancel_callback;
  void run_cancel_callback();

  std::string last_input_plugin;

  std::vector<InputHook> input_hooks;

public:
  
  void check_cancel();

  void enable_all_input()
  {
    foreach (InputDevice* device, devices)
      device->enable_all_input();
  }

  void disable_all_input()
  {
    foreach (InputDevice* device, devices)
      device->disable_all_input();
  }

  std::pair<bool, Input> generate_string(std::string& str, int& search_help_offset,
					 const boost::function<void (const std::string&)>& print_func,
					 std::list<Input> exit_inputs)
  {
    save_map();
    set_map("search");

    Input device_input;

    std::string lowercase_str = string_format::lowercase(str);

    bool save_playlist = false;

    foreach (InputDevice* device, devices) {
      device->enable_all_input();
      device->generate_search_top();
    }

    while (true) {

      bool value_found = false;

      foreach (Input& input, exit_inputs)
	if (input == device_input) {
	  value_found = true;
	  break;
	}

      if (value_found)
	break;

      print_func(str);

      device_input = get_input();

      if (device_input.key == "touch_input") {
	S_Touch::get_instance()->run_callback();
	break;
      }

      if (device_input.command == "action") {
	save_playlist = true;
	break;
      } else {
	foreach (InputDevice* device, devices)
	  if (device->name() == last_input_plugin)
	    device->generate_search_input(str, lowercase_str, search_help_offset, device_input);
      }

      trigger_input(device_input);
    }

    foreach (InputDevice* device, devices)
      device->disable_all_input();

    restore_map();

    return std::make_pair(save_playlist, device_input);
  }

  // returns db id
  template<typename T>
  int graphical_search(const std::vector<T>& current_files,
		       const boost::function<void (const std::vector<T>&)>& print_func,
		       const boost::function<std::vector<std::pair<std::string, int> > (const std::string&)>& gen_list,
		       const boost::function<std::string (int)>& id_to_filename,
		       const boost::function<bool (const std::vector<T>&)>& is_graphical,
		       int row_size, bool& search_mode,
		       std::string& search_str, std::string& lowercase_search_str,
		       std::string& search_depth, int& offset, int& search_help_offset,
		       BusyIndicator *busy, bool show_right_selection = true)
  {
    Config *conf = S_Config::get_instance();

    ScreenUpdater *screen_updater = S_ScreenUpdater::get_instance();
    screen_updater->timer.deactivate("audio_fullscreen");

    save_map();
    set_map("search");

    last_search_word_list_size_wrapper.clear();

    // reset
    search_str = "";
    lowercase_search_str = "";
    offset = -1;
    search_help_offset = -1;

    search_mode = true;

    Input device_input;

    int pos = -1;

    foreach (InputDevice* device, devices) {
      device->enable_all_input();
      device->search_top();
    }

    ExtraMenu em("", true);

    if (show_right_selection) {

      em.starting_layer = 5;

      std::string search_select_font = "Vera/" + conv::itos(resolution_dependant_font_size(17, conf->p_v_res()));
      std::pair<int, int> search_select_sizes = string_format::calculate_string_size(gettext("current folder"), search_select_font);

      em.set_drop_down_pos(conf->p_h_res()-search_select_sizes.first-25, 75);
      em.add_item(ExtraMenuItem(gettext("current folder"), "",
				boost::bind(&InputMaster::set_depth, this,
					    gettext("current folder"))));
      em.add_item(ExtraMenuItem(gettext("all folders"), "",
				boost::bind(&InputMaster::set_depth, this,
					    gettext("all folders"))));
      em.add_exit_input(Input("left", "general"));

    }

    bool first_time = true;

    bool in_select_depth = false;
    depth = gettext("current folder");

    search_depth = depth;

    std::vector<T> results;

    std::string last_search_str = "1"; // different from ""

    bool first_run = true;

    while (device_input.command != "back" && device_input.command != "search") {

      device_input.key = "";

      if (!in_select_depth) {

	if (first_run || device_input.command == "action" && offset == -1) {
	  first_run = false;

	  std::vector<std::pair<std::string, int> > db_results = gen_list(search_str);

	  results.clear();

	  if (depth == gettext("current folder")) {
	    foreach (const T& file, current_files) {
	      foreach (GraphicalPair& pair, db_results) {
		if (file.db_id == pair.second) {
		  results.push_back(file);
		  break;
		}
	      }
	    }
	  } else {
	    foreach (GraphicalPair& pair, db_results) {
	      std::string filename = id_to_filename(pair.second);
	      T tmp(filename);
	      tmp.name = pair.first;
	      tmp.db_id = pair.second;
	      if (isDirectory(filename))
		tmp.type = "dir";
	      else
		tmp.type = "file";
	      results.push_back(tmp);
	    }
	  }

	  if (first_time) {
	    first_time = false;
	    offset = -1;
	  } else if (results.size() > 0)
	    offset = 0;

	  last_search_str = search_str;
	}

	print_func(results);

	while (device_input.key.empty())
	  device_input = get_input();

	if (device_input.key == "touch_input") {
	  S_Touch::get_instance()->run_callback();

	  if (!touch_action)
	    continue;
	  else {
	    touch_action = false; // reset
	    pos = -1;
	    break;
	  }
	}

	if (device_input.command == "action" && offset != -1) {

	  if (results.size() > 0) {
	    T result = vector_lookup(results, offset % results.size());

	    if (depth == gettext("current folder")) {
	      int i = 0;
	      foreach (const T& file, current_files) {
		if (file == result) {
		  pos = i;
		  break;
		} else
		  ++i;
	      }
	    } else
	      pos = result.db_id;
	    break;
	  }

	} else if ((offset == -1 || !is_graphical(results)) && device_input.command == "right") {

	  if (show_right_selection)
	    in_select_depth = true;

	  continue;

	} else {
	  std::string old_search_str = search_str;

	  foreach (InputDevice* device, devices)
	    if (device->name() == last_input_plugin)
	      device->search_graphical_input(search_str, lowercase_search_str, device_input, offset, pos,
					     search_help_offset, is_graphical(results), row_size, results.size());

	  if (old_search_str != search_str)
	    device_input.command = "";
	}

      } else if (in_select_depth) {

	em.mainloop();

	if (search_depth != depth) {
	  search_depth = depth;
	  last_search_str = search_str + "1"; // force redraw
	}

	busy_idle(busy);

	print_func(results);

	in_select_depth = false;
	continue;

      }

      trigger_input(device_input);
    }

    foreach (InputDevice* device, devices)
      device->disable_all_input();

    restore_map();

    search_mode = false;

    screen_updater->timer.activate("audio_fullscreen");

    return pos;
  }

  template<typename T>
  void epg_search(const std::vector<T>& list, int *position,
		  const boost::function<void (const std::vector<T>&)>& print_func,
		  const boost::function<bool (const T&, const std::string&)>& compare,
		  const boost::function<std::string (const T&)>& representation, bool& search_mode,
		  std::string& search_str, std::string& lowercase_search_str, int& offset,
		  const boost::function<void (const std::vector<T>&)>& action, BusyIndicator *busy_indicator)
  {
		
    save_map();
    set_map("search");

    search_str = "";
    lowercase_search_str = "";
    offset = 0;
    search_mode = true;

    Input device_input;

    bool canceled = true;

    foreach (InputDevice* device, devices) {
      device->enable_all_input();
      device->search_top();
    }

		std::vector<T> result_list;
		
		result_list = gen_search(list, lowercase_search_str, compare);
		
		print_func(result_list);

    while (device_input.command != "back") {

			device_input = get_input_busy_wrapped(busy_indicator);
			if (device_input.key == "touch_input") {
				foreach (InputDevice* device, devices)
					device->disable_all_input();

				restore_map();
				
				search_mode = false;

				S_Touch::get_instance()->run_callback();

				save_map();
				set_map("search");

				foreach(InputDevice* device, devices) {
					device->enable_all_input();
					device->search_top();
				}

				search_mode = true;

		    	print_func(result_list);

				continue;
			}

			if (device_input.command == "action") {
				foreach (InputDevice* device, devices)
					device->disable_all_input();

				restore_map();
				
				search_mode = false;

				action(result_list);

				save_map();
				set_map("search");

				foreach(InputDevice* device, devices) {
					device->enable_all_input();
					device->search_top();
				}
		    	print_func(result_list);

				search_mode = true;
			} else if (device_input.key != "") {
//				printf("Key is %s, search_str >%s<\n", device_input.key.c_str(), lowercase_search_str.c_str());

		    	foreach (InputDevice* device, devices)
					if (device->name() == last_input_plugin)
						device->search_input(search_str, lowercase_search_str, device_input, offset,
								boost::bind(&InputMaster::gen_search_string_list<T>, this, list, _1, compare, representation)
						);
		    	result_list = gen_search(list, lowercase_search_str, compare);

		    	print_func(result_list);

			}
		}

		foreach (InputDevice* device, devices)
			device->disable_all_input();

		restore_map();

		search_mode = false;
  }
  
  template<typename T>
  void search(const std::vector<T>& list, int *position,
	      const boost::function<void (const std::vector<T>&)>& print_func,
	      const boost::function<bool (const T&, const std::string&)>& compare,
	      const boost::function<std::string (const T&)>& representation, bool& search_mode,
	      std::string& search_str, std::string& lowercase_search_str, int& offset)
  {
    save_map();
    set_map("search");

    search_str = "";
    lowercase_search_str = "";
    offset = 0;
    search_mode = true;

    Input device_input;

    bool canceled = true;

    foreach (InputDevice* device, devices) {
      device->enable_all_input();
      device->search_top();
    }

    print_func(gen_search(list, lowercase_search_str, compare));

    while (device_input.command != "back") {

      device_input = get_input();
      if (device_input.key == "touch_input") {
	S_Touch::get_instance()->run_callback();
	continue;
      }

      if (device_input.command == "action") {
	canceled = false;
	break;
      } else if (device_input.key != "") {
	foreach (InputDevice* device, devices)
	  if (device->name() == last_input_plugin)
	    device->search_input(search_str, lowercase_search_str, device_input, offset,
				 boost::bind(&InputMaster::gen_search_string_list<T>, this, list, _1, compare, representation));

	print_func(gen_search(list, lowercase_search_str, compare));
      }
    }

    foreach (InputDevice* device, devices)
      device->disable_all_input();

    // find position in the original list
    if (!canceled) {
      std::vector<std::string> searched = gen_search_string_list(list, lowercase_search_str, compare, representation);

      if (searched.size() > 0) {
	std::string element = vector_lookup(searched, offset%searched.size());

	for (int i = 0, size = list.size(); i < size; ++i)
	  if (representation(vector_lookup(list, i)) == element) {
	    *position = i;
	    break;
	  }
      }
    }

    restore_map();

    search_mode = false;
  }

  void trigger_input(Input input);

  bool touch_action;

  void got_touch_action()
  {
    touch_action = true;
  }

  std::string depth;

  void set_depth(const std::string& mod)
  {
    depth = mod;
  }

  typedef std::pair<std::string, int> search_word_pair;

  std::vector<search_word_pair> last_search_word_list_size_wrapper;

  int graphical_list_size_wrapper(const boost::function<std::vector<std::pair<std::string, int> > (const std::string&)>& list,
				  const std::string& search_word)
  {
    foreach (search_word_pair& pair, last_search_word_list_size_wrapper)
      if (pair.first == search_word)
	return pair.second;

    // not already searched
    int size = list(search_word).size();
    last_search_word_list_size_wrapper.push_back(std::make_pair(search_word, size));
    return size;
  }

  template<typename T>
  const std::vector<std::string> gen_search_string_list(const std::vector<T>& list,
							const std::string& lsearch,
							const boost::function<bool (const T&, const std::string&)>& compare,
					     const boost::function<std::string (const T&)>& representation)
  {
    std::vector<std::string> newlist;

    for (int i = 0, size = list.size(); i < size; ++i)
      if (compare(vector_lookup(list, i), lsearch))
	newlist.push_back(representation(vector_lookup(list, i)));

    return newlist;
  }

  template<typename T>
  const std::vector<T> gen_search(const std::vector<T>& list,
				  const std::string& lsearch,
				  const boost::function<bool (const T&, const std::string&)>& compare)
  {
    std::vector<T> newlist;

    for (int i = 0, size = list.size(); i < size; ++i)
      if (compare(vector_lookup(list, i), lsearch))
	newlist.push_back(vector_lookup(list, i));

    return newlist;
  }

  void busy_idle(BusyIndicator *indicator);


  void register_cancel_callback(const boost::function<void (void)>& callback);
  void clear_cancel_callback();

  bool do_filter;
  std::vector<std::string> filter_exceptions;

  void set_filter(const std::vector<std::string>& exceptions)
  {
    inputs_mutex.enterMutex();
    do_filter = true;
    filter_exceptions = exceptions;
    inputs_mutex.leaveMutex();
  }

  void unset_filter()
  {
    inputs_mutex.enterMutex();
    do_filter = false;
    filter_exceptions.clear();
    inputs_mutex.leaveMutex();
  }

  static pthread_mutex_t singleton_mutex;
  void input_devices_loaded();

  std::string find_shortcut(const std::string& command);

  // used for find shortcut, so that we know what devices is the default
  void set_default_device(const std::string& name);

  bool restore_map_empty();
  void save_map();
  void restore_map();
  void set_map(const std::string& name);
  std::string current_map();
  std::string current_saved_map();

  Input get_input_busy_wrapped(BusyIndicator *busy);
  Input get_input();

  void suspend();
  void wake_up();

  bool parse_keys(const std::string& plugin, const std::string& version);
  bool extend_default(const std::string& plugin, const std::string& version);

  void add_input_device(InputDevice *device);

  void add_input(Input input, const std::string& plugin);

  void start_devices();

  InputMaster();

  /* InputHooks are different from GlobalCommands because:
   * 1) they can be set to bypass the input filter
   * 2) their callbacks are launched from get_input()
   * like GlobalCommands, they require input to be processed
   * with get_input() or get_input_busy_wrapped().
   * 
   * CAVEAT: when the callback function is executed, the input mutex is locked,
   * hence the callback function may NOT process input nor trigger any event that
   * depends on further input events!!!
   * HINTS:
   * a) if you want a InputHook to bypass the filter, set ignore_filters to true at creation.
   * b) If you want a InputHook to receive all unfiltered keypresses, set command to an empty string at creation.
   * c) If you want a InputHook to receive all keypresses and bypass the filter, do both a) and b)
   */
  void add_input_hook(const InputHook &ih);
  void del_input_hook(const std::string hookname);
  void clear_input_hooks();
};

typedef Singleton<InputMaster> S_InputMaster;

#endif
