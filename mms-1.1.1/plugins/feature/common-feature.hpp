#ifndef COMMON_FEATURE_HPP
#define COMMON_FEATURE_HPP

#include "plugins.hpp"

#include "boost.hpp"

#include <string>

struct Pipe
{
  int inpipe0;
  int inpipe1;
  int outpipe0;
  int outpipe1;
  int pipefl;
  pid_t pid;
};

namespace graphics
{
  std::string resolution_dependant_font_wrapper(int size, Config *conf);
  int calculate_font_height(std::string font, Config *conf);
}

namespace run
{
  void aquire_exclusive_access();
  void release_exclusive_access();

  // just start an external program
  pid_t external_program(const std::string& command, bool wait = true);

  // start an external program that will have exclusive access to shared
  // ressources such as sound card
  bool exclusive_external_program(const std::string& command);

  // start an exclusive program and create a pipe to talk to the external program
  std::pair<bool, Pipe> exclusive_external_program_pipe(const std::string& command);

  void release_exclusive_plugin_access();
  void close_pipe(const Pipe& p);

  // helper function
  std::pair<bool, Pipe> external_program_pipe(const std::string& command);
}

template<typename T>
T* get_class(const std::string& plugin_name)
{
  Plugins *plugins = S_Plugins::get_instance();
  foreach (FeaturePlugin *plugin, plugins->fp_data)
    if (plugin->plugin_name() == plugin_name) {
      if (plugin->module != 0)
	return static_cast<T*>(plugin->module);
      else
	return 0;
    }

  return 0;
}

#endif
