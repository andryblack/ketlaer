#ifndef PLUGINS_HPP
#define PLUGINS_HPP

#include "plugin.hpp"

#define MMS_PLUGIN_INFO "mms_plugin_info"

#include "singleton.hpp"

#include <vector>
#include <string>
#include <iostream>

class Plugins
{
private:

  Plugin *add_plugin(const std::string& filename);
  bool _dlclose(const std::string&);
  void _destroy(Plugin * plugin);

  std::vector<std::string> ppnames;
  Config *conf;

public:

  static pthread_mutex_t singleton_mutex;

  bool exclusive_access;

  void delete_plugins();

  void scan();
  void scan_input_output();

  std::vector<InputPlugin*> ip_data;
  std::vector<OutputPlugin*> op_data;
  std::vector<FeaturePlugin*> fp_data;
  std::vector<AudioPlayerPlugin*> audio_players;
  std::vector<MoviePlayerPlugin*> movie_players;
  std::vector<PrinterPlugin*> printers;

  template<typename P>
  P* find(const std::vector<P*>& data, const std::string& name)
  {
    for (int i = 0, size = data.size(); i < size; ++i)
      if (name == vector_lookup(data, i)->plugin_name())
	return vector_lookup(data, i);

    return 0;
  }

  Plugin* find(const std::string& name);
  Plugin* load(const std::string& filename);
};

typedef Singleton<Plugins> S_Plugins;

#endif
