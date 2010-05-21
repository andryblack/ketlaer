#include "plugin.hpp"
#include "genericplayer.hpp"

class GenericPlayerPlugin : public MoviePlayerPlugin
{
public:

  GenericPlayerPlugin() 
  {
    Config *conf = S_Config::get_instance();
    GenericPlayerConfig *genericp_conf = S_GenericPlayerConfig::get_instance();
    genericp_conf->parse_configuration_file(conf->p_homedir());

    player = new GenericPlayer();
  }

  std::string plugin_name() const { return "GenericPlayer"; }
};

MMS_PLUGIN(GenericPlayerPlugin)
