#include "plugin.hpp"
#include "alsaplayer.hpp"

class AlsaplayerAudioPlayerPlugin : public AudioPlayerPlugin
{
public:

  AlsaplayerAudioPlayerPlugin() 
  {
    Config *conf = S_Config::get_instance();
    AlsaplayerConfig *alsaplayer_conf = S_AlsaplayerConfig::get_instance();
    alsaplayer_conf->parse_configuration_file(conf->p_homedir());

    player = new Alsaplayer();
  }

  std::string plugin_name() const { return "Alsaplayer"; }
};

MMS_PLUGIN(AlsaplayerAudioPlayerPlugin)
