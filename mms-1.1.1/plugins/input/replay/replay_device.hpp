#ifndef REPLAY_DEVICE_HPP
#define REPLAY_DEVICE_HPP

#include "input_device.hpp"
#include "input.hpp"

#include <queue>

class ReplayDevice : public InputDevice
{
private:
  std::queue<std::pair<timespec, Input> > inputs;
  timespec time_start;

  void generate_search_top()
  {}

  void generate_search_input(std::string& UNUSED(str), std::string& UNUSED(lstr), int& UNUSED(offset), Input UNUSED(input))
  {}

  void search_top()
  {}

  void search_input(std::string& UNUSED(search_str), std::string& UNUSED(lowercase_search_str), Input UNUSED(input), int& UNUSED(offset),
		    const boost::function<const std::vector<std::string> (const std::string&)>& UNUSED(list_gen))
  {}

  void search_graphical_input(std::string& UNUSED(str), std::string& UNUSED(lowercase_search_str), Input UNUSED(input),
			      int& UNUSED(offset), int& UNUSED(pos), int& UNUSED(search_help_offset), 
			      bool UNUSED(graphical_print), int UNUSED(row_size), int UNUSED(result_size))
  {}

public:
  ReplayDevice();
  ~ReplayDevice() { terminate(); }

  bool init() { return true; }

  std::string name()
  {
    return "replay";
  }

  void suspend();
  void wake_up();

  void run();
};

#endif
