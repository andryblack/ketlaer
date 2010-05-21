#include "config.h"
#include "global_options.hpp"
#include "config.hpp"
#include "common.hpp"
#include "gettext.hpp"
#include "boost.hpp"

#include <string>
#include <vector>

#include <iostream>

#include <cassert>
#include <fstream>


using std::string;
using std::vector;
using std::list;

typedef std::pair<string, string> string_pair;

pthread_mutex_t GlobalOptions::singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

GlobalOptions::GlobalOptions()
{
  Config *conf = S_Config::get_instance();
  vector<string> cd_device_values;

  assert(conf->p_cdpath().size() == conf->p_cdname().size());

  list<string_pair > cd_names = conf->p_cdname();
  list<string_pair > cd_paths = conf->p_cdpath();

  // sanity check
  foreach (string_pair& path_pair, cd_paths) {
    bool found_match = false;
    foreach (string_pair& name_pair, cd_names)
      if (name_pair.first == path_pair.first) {
	found_match = true;
	break;
      }
    if (!found_match) {
      print_critical(gettext("name mismatch in cd configuration, please check your config"));
      exit(0);
    }
  }

  foreach (string_pair& name_pair, cd_names)
    cd_device_values.push_back(name_pair.second);

  cd_device = new Option(false, gettext("cd device"), "cd device",
			 0, cd_device_values, cd_device_values);

  vector<string> audio_device_values;
  audio_device_values.push_back(conf->p_first_dev_name());

  if (!conf->p_second_dev_name().empty())
    audio_device_values.push_back(conf->p_second_dev_name());

  if (!conf->p_third_dev_name().empty())
    audio_device_values.push_back(conf->p_third_dev_name());

  audio_device_audio = new Option(false, gettext("audio device (music)"), "audio device (music)",
		  0, audio_device_values, audio_device_values);

  audio_device_video = new Option(false, gettext("audio device (movies)"), "audio device (movies)",
                            0, audio_device_values, audio_device_values);

  val.push_back(audio_device_audio);
  val.push_back(audio_device_video);
  val.push_back(cd_device);
  
  load();

}

Option *GlobalOptions::dir_order()
{
  vector<string> dir_order_values;
  dir_order_values.push_back(gettext("directories first"));
  dir_order_values.push_back(gettext("by name"));
  dir_order_values.push_back(gettext("by date"));

  vector<string> english_dir_order_values;
  english_dir_order_values.push_back("directories first");
  english_dir_order_values.push_back("by name");
  english_dir_order_values.push_back("by date");

  return new Option(false, gettext("sorting"), "sorting",
		    0, dir_order_values, english_dir_order_values);
}

global_audio_device GlobalOptions::get_audio_device_audio(){
  return get_audio_device(audio_device_audio->pos);
}

global_audio_device GlobalOptions::get_audio_device_video(){
  return get_audio_device(audio_device_video->pos);
}


global_audio_device GlobalOptions::get_audio_device(int pos){
 
  Config *conf = S_Config::get_instance();

  global_audio_device a_d;
  a_d.pos = pos;
  switch (pos){

    case 0:
      a_d.dev_name =  conf->p_first_dev_name();
      a_d.dev_digital = conf->p_first_dev_digital();
      a_d.dev_channels = conf->p_first_dev_channels();
      a_d.dev_output = conf->p_first_dev_output();
      a_d.dev_path = conf->p_first_dev_path();
      break;

    case 1:
      a_d.dev_name =  conf->p_second_dev_name();
      a_d.dev_digital = conf->p_second_dev_digital();
      a_d.dev_channels = conf->p_second_dev_channels();
      a_d.dev_output = conf->p_second_dev_output();
      a_d.dev_path = conf->p_second_dev_path();
      break;

    case 2:
      a_d.dev_name =  conf->p_third_dev_name();
      a_d.dev_digital = conf->p_third_dev_digital();
      a_d.dev_channels = conf->p_third_dev_channels();
      a_d.dev_output = conf->p_third_dev_output();
      a_d.dev_path = conf->p_third_dev_path();
      break;

    default:
      //uhm... we shouldn't be here
      assert(false);
  }

  return a_d;
}

GlobalOptions::~GlobalOptions()
{
  save();
  delete cd_device;
  delete audio_device_audio;
  delete audio_device_video;

}

void GlobalOptions::save(){
  std::ofstream file;
  std::string options_dir;
  Config *conf = S_Config::get_instance();
  if (!recurse_mkdir(conf->p_var_data_dir(), "options/", & options_dir)){
    print_critical(gettext("Could not create options directory: ") + options_dir, "GLOBAL OPTIONS");
    return;
  }
  
  string path = options_dir + "GlobalOptions";
  file.open(path.c_str());
  
  if (!file) {
    print_critical(gettext("Could not write options to file ") + path, "GLOBAL OPTIONS");
    return;
  }
  
  for (unsigned int t = 0; t < val.size(); t++)
    if (val[t]->english_values.size() > val[t]->pos)
      file << val[t]->english_name << "," << 
          val[t]->english_values[val[t]->pos] << std::endl;
  file.close();
  
  //fprintf(stderr, "Global options saved\n");
  
}


void GlobalOptions::load(){
  Config *conf = S_Config::get_instance();
  string path = conf->p_var_data_dir() + "options/" + "GlobalOptions";
  std::ifstream In(path.c_str());
  
  if(!In){
    print_critical(gettext("Could not open options file ") + path, "GLOBAL OPTIONS");
    return;
  }
  
  string name, value, str;
  int i;

  while (getline(In, str)) {

    string::size_type comma = str.find(",");

    name = str.substr(0,comma);
    value = str.substr(comma+1);

    foreach (Option *option, val)
      if (!option->type && option->english_name == name) {
      i = 0;
      option->pos = 0;
      for (unsigned int t = 0; t < option->english_values.size(); t++)
        if (option->english_values[t] == value) {
        option->pos = i;
        break;
        } else
          ++i;
      }
  }
  //fprintf(stderr, "Global options loaded\n");
}
