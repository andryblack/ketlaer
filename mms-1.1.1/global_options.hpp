#ifndef GLOBAL_OPTIONS_HPP
#define GLOBAL_OPTIONS_HPP

#include "option.hpp"
#include "singleton.hpp"


typedef struct {
  unsigned int pos;
  std::string dev_name;
  bool dev_digital;
  int dev_channels;
  std::string dev_output;
  std::string dev_path;
} global_audio_device;
  

class GlobalOptions 
{
public:
  static pthread_mutex_t singleton_mutex;

  GlobalOptions();

  Option *dir_order();

  //global
  Option *cd_device;
  Option *audio_device_audio;
  Option *audio_device_video;
  
  global_audio_device get_audio_device(int pos);
  global_audio_device get_audio_device_audio();
  global_audio_device get_audio_device_video();

  ~GlobalOptions();
  void save();
private:
  void load();
  std::vector<Option*> val;

};

typedef Singleton<GlobalOptions> S_GlobalOptions;

#endif
