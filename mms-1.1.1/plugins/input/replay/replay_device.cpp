#include "replay_device.hpp"

#include "config.hpp"
#include "common.hpp"

#include "global.hpp"

#include <fstream>
#include <iostream>

void ReplayDevice::suspend()
{}

void ReplayDevice::wake_up()
{}

ReplayDevice::ReplayDevice()
{
  Config *conf = S_Config::get_instance();

  if (!conf->p_replay_read().empty()) {
    std::ifstream file((conf->p_var_data_dir() + conf->p_replay_read()).c_str());
    std::string str;

    if (!file) {
      print_warning("file not found: " + (conf->p_var_data_dir() + conf->p_replay_read()), "REPLAY");
      return;
    }

    while (getline(file, str)) {

      std::string::size_type i = str.find(",");
      int time_sec = conv::atoi(str.substr(0, i));
      str = str.substr(i+1);

      i = str.find(":");
      int time_msec = conv::atoi(str.substr(0, i));
      str = str.substr(i+1);

      timespec t;
      t.tv_sec = time_sec;
      t.tv_nsec = time_msec;

      Input tmp_input(str);
      inputs.push(std::make_pair(t, tmp_input));
    }
    clock_gettime(0, &time_start);
  }

  print_debug("Read: " + conv::itos(inputs.size()) + " commands to replay", "REPLAY");
}

void ReplayDevice::run()
{
  Config *conf = S_Config::get_instance();
  InputMaster *input_master = S_InputMaster::get_instance();

  std::pair<timespec, Input> element;
  timespec current_time;

  while (true) {

    if (inputs.empty()) {
      print_warning("Warning, no more input to replay", "REPLAY");
      ::exit(0);
    } else {
      element = inputs.front();
      inputs.pop();
    }

    if (!conf->p_benchmark()) {
      clock_gettime(0, &current_time);

      int sleep_time = ((element.first.tv_sec - (current_time.tv_sec-time_start.tv_sec))*1000 +
			(element.first.tv_nsec - (current_time.tv_nsec-time_start.tv_nsec)/(1000*1000)));

      if (sleep_time > 0)
	sleep(sleep_time);
    }

    input_master->add_input(element.second, "replay");
  }
}

