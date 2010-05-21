#include "plugin.hpp"

#include "mplayer.hpp"

class MplayerMoviePlayerPlugin : public MoviePlayerPlugin
{
public:

  MplayerMoviePlayerPlugin()
  {
    Config *conf = S_Config::get_instance();
    MplayerConfig *mplayer_conf = S_MplayerConfig::get_instance();
    mplayer_conf->parse_configuration_file(conf->p_homedir());

    player = new Mplayer();
  }

  std::string plugin_name() const { return "MPlayer"; }
};

MMS_PLUGIN(MplayerMoviePlayerPlugin)
