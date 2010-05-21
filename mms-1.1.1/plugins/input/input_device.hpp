#ifndef INPUT_DEVICE_HPP
#define INPUT_DEVICE_HPP

#include "config.h"

#include "common.hpp"
#include "config.hpp"
#include "updater.hpp"

#include <cc++/thread.h>

#include <string>
#include <map>
#include <vector>

class BusyIndicator;
class Input;

typedef std::map<std::string, Input> input_map;
typedef std::vector<Input> input_list;

typedef std::pair<std::string, std::pair<input_map, input_list> > name_input;
typedef std::map<std::string, std::pair<input_map, input_list> > name_input_map;

typedef std::pair<std::string, int> GraphicalPair;

// used as superclass of input devices
class InputDevice : public ost::Thread
{
public:
  // for searching
  void enable_all_input() { accept_all = true; S_ScreenUpdater::get_instance()->trigger.disable(); }
  void disable_all_input() { accept_all = false; S_ScreenUpdater::get_instance()->trigger.enable(); }

  virtual void suspend() = 0;
  virtual void wake_up() = 0;

  virtual bool init() = 0;

  virtual ~InputDevice() {};
  InputDevice();

  // helper functions for input master
  bool parse_keys(const std::string& plugin, const std::string& version);
  bool extend_default(const std::string& plugin, const std::string& version);
  input_list cur_list();

  virtual std::string name() = 0;

  virtual void generate_search_input(std::string& str, std::string& lstr, int& offset, Input input) = 0;

  virtual void search_input(std::string& str, std::string& lowercase_search_str, Input input, int& offset,
			    const boost::function<const std::vector<std::string> (const std::string&)>& list_gen) = 0;

  virtual void search_graphical_input(std::string& str, std::string& lowercase_search_str, Input input,
				      int& offset, int& pos, int& search_help_offset, bool graphical_print,
				      int row_size, int result_size) = 0;

  virtual void generate_search_top() = 0;

  virtual void search_top() = 0;

protected:

  bool check_version(const std::string& input, const std::string& str, const std::string& version);

  std::pair<bool, std::pair<input_map, input_list> > parse_config(const std::string& plugin,
								  const std::string& version, bool extends);
  void complete_string(std::string &search_str, std::string &lowercase_search_str,
		       const boost::function<std::vector<std::string> (const std::string&)>& list_gen)
  {
    std::vector<std::string> searched = list_gen(search_str);

    if (searched.size() != 0) {
      if (searched.size() == 1) {
	search_str = vector_lookup(searched, 0);
	lowercase_search_str = string_format::lowercase(search_str);
      } else {
	std::string gcss = string_format::greatest_common_substring(string_format::lowercase(vector_lookup(searched, 0)),
								    string_format::lowercase(vector_lookup(searched, 1)));
	for (int i = 2; i < searched.size(); ++i)
	  gcss = string_format::greatest_common_substring(gcss, string_format::lowercase(vector_lookup(searched, i)));

	search_str = gcss;
	lowercase_search_str = string_format::lowercase(search_str);
      }
    }
  }

  bool accept_all;

  // maps from input plugin to a pair of key->input map + vector of inputs
  name_input_map maps;

  bool find_input(const std::string& input, Input &i);

private:

  virtual void run() = 0;
};

#endif
