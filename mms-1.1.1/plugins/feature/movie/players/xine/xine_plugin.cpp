#include "plugin.hpp"
#include "xine.hpp"

class XineMoviePlayerPlugin : public MoviePlayerPlugin
{
public:

  XineMoviePlayerPlugin()
  {
    Config *conf = S_Config::get_instance();
    XineConfig *xine_conf = S_XineConfig::get_instance();
    xine_conf->parse_configuration_file(conf->p_homedir());

    player = new XineMovie();
  }

  std::string plugin_name() const { return "cxfe (Xine)"; }
};

MMS_PLUGIN(XineMoviePlayerPlugin)
